#define CURL_STATICLIB
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include "curl/curl.h"

using namespace std;

const int GENE = 100; // Our Gene represent the first n of attributes to test
const int POP_SIZE = 50; // How many population size we want to put at the first try 
const float CO_probability = 0.8; // Cross-over chances, smaller the Co_probability will triggered less chance of cross-over
const float MUT_PROBABILITY = 0.2; // Mutation chances, how big or small the chances mutation will occur
const int MAX_GENERATION = 25; // Termination criteria, maximum generation
const int MAX_MUTATE = 3; // The smaller this value, the smaller the chances stuck with local minimum
const int MAX_ALLOW_PARAM = 100; // How many attributes to be activated when we initialize the gene with 1. We can think of how many 1 allow to occur during initialization. 

// declare fitness data structure
double double_result;
double fitness[POP_SIZE];

// declare parents data structure
int parents[2][GENE];

// declare children data structure
int children[2][GENE];

// declare chromosomes data structure
int chromosome[POP_SIZE][GENE];
int newChromosome[POP_SIZE][GENE];
int chromosome_row[GENE];

// declare new chromosome counter
int newChromoCounter = 0;

// Measurement metrics declaration
double bestFitness = 99.9;
double avgFitness = 0.0;
int bestChromosome[GENE];

ofstream bestFitnessFile, avgFitnessFile, bestChromosomeFile;

// Helper Function
string convertToString(int* a, int size)
{
	int i;
	string s = "";
	for (i = 0; i < size; i++) {
		s = s + to_string(a[i]);
	}
	return s;
}

/********************************************************************************************************************************************************/
// CURL Helper function for web POST
static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
	string& text = *static_cast<string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

// Chromosome Evaluation
void evaluateChromosome()
{
	string url, s_a;
	CURL* curl;
	CURLcode res;
	
	// 1.0) For each of the Population Size
	for (int i = 0; i < POP_SIZE; i++)
	{
		// 2.0) For each of the gene in the population size
		for (int j = 0; j < GENE; j++)
		{
			// 3.0) We put the value in chromosome row
			chromosome_row[j] = chromosome[i][j];
		}

		// 4.0) we convert the chromosome row to string value
		s_a = convertToString(chromosome_row, GENE);

		std::string str = s_a;

		// 5.0) make the data as a constant
		const char* data = str.c_str();

		string result = "";

		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();

		if (curl) 
		{
			// 7.0) select the python server that run the Machine Learning Algorithm (I use localhost in my case)
			curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/");
			curl_easy_setopt(curl, CURLOPT_POST, 1L); 
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 100L); // 7.1) The length of the post field

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); // 8.0) POST the chromosome row in string to the flask python server

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			if (CURLE_OK != res) {
				cerr << "CURL error: " << res << '\n';
			}
		}
		curl_global_cleanup();
		cout << result << "\n\n";
		double_result = stod(result); // 9.0) Get the result return from the python flask server

		fitness[i] = double_result; // 10.0) Save the result in fitness array
	}

}

/********************************************************************************************************************************************************/

// Initialize Population
void initializePopulation() {
	int randNum;
	int allowParam = 0;

	//initialize random seed
	srand(time(NULL));

	// 1.0) For each of the popualtion size
	for (int c = 0; c < POP_SIZE; c++) {

		// 2.0) Check if The chromosome contains params 1 less than allowParam (I want to limit number of activated attributes)
		for (int i = 0; i < GENE; i++) {

			//2.1) If the activated attributes less than maximum allow attributes, we allowit to be activated
			if (allowParam < MAX_ALLOW_PARAM)
			{
				randNum = rand() % 2;
				if (randNum == 1)
				{
					allowParam += 1;
				}
				chromosome[c][i] = randNum;
			}
			else // 2.2) else, it is not allowed to be activated
			{
				chromosome[c][i] = 0;
			}
		}
	}
}

// Parent Selection
void parentSelection() {
	// 1.0) Declare necessary variable
	int player1, player2; // select 2 parent to mate with each other
	int indexParents[2]; // index of selected players

	// Perform 2 times of the tournament to select 2 parents
	for (int i = 0; i < 2; i++) 
	{
		// selecting 2 parents
		player1 = rand() % POP_SIZE; // parent 1 chromosome index
		player2 = rand() % POP_SIZE; // parent 1 chromosome index

		// Smaller fitness is the winner (Change Operator if graph evaluation method change)
		if (fitness[player1] <= fitness[player2])
		{
			indexParents[i] = player1;
		}
		else
		{
			indexParents[i] = player2;
		}

		cout << "\n\t Players" << player1 << "vs" << player2;
		cout << "\n\t Fitness" << fitness[player1] << "vs" << fitness[player2];
		cout << "\n\t Winner" << indexParents[i];
	}

	// Copy the winner chromosome to parent chromosome
	for (int p = 0; p < 2; p++) // Here is 2 because our parent chromosome got only 2
	{
		cout << "\n\t parents " << p + 1 << ":";

		for (int g = 0; g < GENE; g++)
		{
			parents[p][g] = chromosome[indexParents[p]][g];
			cout << parents[p][g] << " ";
		}
	}
}

// One-Point CrossOver
void crossover() {

	float prob = 0.0;
	int co_point;

	// 1st Step: Copy Parents to Children 
	// For every parents p, copy it into childrens g
	for (int p = 0; p < 2; p++)
	{
		for (int g = 0; g < GENE; g++)
		{
			children[p][g] = parents[p][g];
		}
	}

	// 2nd: Generate random number probability from 0 to 1
	// Generate random number 0 to 1
	prob = ((rand() % 10) + 1) / 10;
	if (prob < CO_probability)
	{
		co_point = rand() % GENE;
		cout << "\n\t Children crossover at " << co_point;

		for (int g = co_point; g < GENE; g++)
		{
			children[0][g] = parents[1][g];
			children[1][g] = parents[0][g];
		}
	}
	else
	{
		cout << "\n\t CrossOver did not happen";
	}


	for (int c = 0; c < 2; c++)
	{
		cout << "\n\t Children " << c + 1 << ":";
		for (int g = 0; g < GENE; g++)
		{
			cout << children[c][g];
		}
	}


}

// Mutation
void mutation()
{
	float prob;
	int mut_point;

	for (int c = 0; c < 2; c++)
	{
		prob = (rand() % 11) / 10.0; // generate the probability value

		if (prob < MUT_PROBABILITY)
		{
			// mutate n number of times based on max-mutate parameter
			for (int i = 0; i < MAX_MUTATE; i++)
			{
				// bit-flip mutation
				mut_point = rand() % GENE;
				cout << "\n\tMutation Gene at " << mut_point;

				if (children[c][mut_point] == 0)
				{
					children[c][mut_point] = 1;
				}
				else
				{
					children[c][mut_point] = 0;
				}
			}
		}
		else
		{
			cout << "\n\tMutation do not occur";
		}
	}

}


// Function to print chromosome, just cout the value in the chromosome array
void printChromosome() {
	for (int c = 0; c < POP_SIZE; c++) {
		cout << "\tC" << c << "\t";
		for (int i = 0; i < GENE; i++) {
			cout << chromosome[c][i] << " ";
		}
		cout << endl;
	}
}

// Function to cout the value in the fitness function.
void printFitness() {
	for (int c = 0; c < POP_SIZE; c++) {
		cout << fitness[c] << endl;
	}
}

// All children replace parent
void survivalSelection()
{
	for (int c = 0; c < 2; c++)
	{
		// Copy All children to new chromsome
		for (int g = 0; g < GENE; g++)
		{
			newChromosome[newChromoCounter][g] = children[c][g];
		}

		newChromoCounter++;
	}

	for (int c = 0; c < newChromoCounter; c++)
	{
		cout << "\tNew chromosome " << c << ": ";
		for (int g = 0; g < GENE; g++)
		{
			cout << " " << newChromosome[c][g];
		}
		cout << "\n";
	}
}

void copyChromosome()
{
	for (int c = 0; c < POP_SIZE; c++)
	{
		for (int g = 0; g < GENE; g++)
		{
			chromosome[c][g] = newChromosome[c][g];
		}
	}
}

// Record the best fitness
void recordBestFitness()
{
	// for each of the population size
	for (int c = 0; c < POP_SIZE; c++)
	{
		// check if the best fitness smaller than current fitness in the array
		if (bestFitness > fitness[c])
		{
			// if yes, then it will be our best fitness
			bestFitness = fitness[c];

			// that will also be our best chromosome value
			for (int g = 0; g < GENE; g++)
			{
				bestChromosome[g] = chromosome[c][g];
			}
		}
	}

	cout << "\n Best Fitness = " << bestFitness;
	cout << "\n Best Chromosome = ";
	for (int g = 0; g < GENE; g++)
	{
		cout << bestChromosome[g] << " ";
	}

	cout << endl;

	// Output To Files
	bestFitnessFile << bestFitness << endl;
	for (int g = 0; g < GENE; g++)
	{
		bestChromosomeFile << bestChromosome[g] << " ";
	}
	bestChromosomeFile << endl;

}

// calculate the avaerage fitness among all the population
void calcAvgFitness()
{
	double sum = 0.0;
	for (int c = 0; c < POP_SIZE; c++)
	{
		sum += fitness[c]; // sum = sum + fitness
	}
	avgFitness = sum / POP_SIZE;
	cout << "\n Average Fitness = " << avgFitness;
	avgFitnessFile << avgFitness << endl;
}

int main()
{
	bestFitnessFile.open("bestFitness.txt");
	avgFitnessFile.open("avgFitness.txt");
	bestChromosomeFile.open("bestChromosome.txt");

	initializePopulation();

	for (int g = 0; g < MAX_GENERATION; g++)
	{
		// Get the Fitness Value of the chromosome
		evaluateChromosome();
		printFitness();

		// Check Whether we get any good fitness in this generation
		recordBestFitness();

		// Calculate the average fitness value among all the Population
		calcAvgFitness();

		newChromoCounter = 0;

		for (int i = 0; i < POP_SIZE / 2; i++)
		{
			// We need to perform Parent Selection
			cout << "\nPARENT SELECTION \n";
			parentSelection();

			cout << "\nCROSS-OVER SELECTION \n";
			crossover();

			cout << "\nMUTATION \n";
			mutation();

			cout << "\nSURVIVAL SELECTION \n";
			survivalSelection();

		}

		copyChromosome();

	}

	bestFitnessFile.close();
	avgFitnessFile.close();
	bestChromosomeFile.close();
	
}