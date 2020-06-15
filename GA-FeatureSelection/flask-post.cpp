#define CURL_STATICLIB
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "curl/curl.h"

using namespace std;

// declare constant - problem specification, population size
const int GENE = 561;
const int POP_SIZE = 2;
const float CO_probability = 0.1; // By right, the smaller the Co_probability will triggered less chance of cross-over
const float MUT_PROBABILITY = 0.5; // Mutation chances, how big or small the chances mutation will occur
const int MAX_GENERATION = 10;

// declare fitness data structure
double double_result;
double fitness[POP_SIZE];

// declare chromosomes data structure
int chromosome[POP_SIZE][GENE];
int newChromosome[POP_SIZE][GENE];

int chromosome_row[GENE];

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
static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
	string& text = *static_cast<string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

/*
void testGetRequest()
{
	string result;
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://tcno.co/hello.txt");
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
}
*/

void evaluateChromosome()
{
	string url, s_a;
	CURL* curl;
	CURLcode res;
	
	for (int i = 0; i < POP_SIZE; i++)
	{
		for (int j = 0; j < GENE; j++)
		{
			chromosome_row[j] = chromosome[i][j];
		}
		
		s_a = convertToString(chromosome_row, GENE);

		std::string str = s_a;
		const char* data = str.c_str();


		string result = "";

		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();

		if (curl) 
		{
			curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 1000L);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

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
		double_result = stod(result);

		fitness[i] = double_result;
	}

}

/********************************************************************************************************************************************************/

void initializePopulation() {
	int randNum;
	//initialize random seed
	srand(time(NULL));
	for (int c = 0; c < POP_SIZE; c++) {
		for (int i = 0; i < GENE; i++) {
			randNum = rand() % 2;
			chromosome[c][i] = randNum;
		}
	}
}

void printChromosome() {
	for (int c = 0; c < POP_SIZE; c++) {
		cout << "\tC" << c << "\t";
		for (int i = 0; i < GENE; i++) {
			cout << chromosome[c][i] << " ";
		}
		cout << endl;
	}
}

void printFitness() {
	for (int c = 0; c < POP_SIZE; c++) {
		cout << fitness[c] << endl;
	}
}

int main()
{
	initializePopulation();
	// printChromosome();
	evaluateChromosome();
	printFitness();
}