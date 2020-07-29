import seaborn as sns
import matplotlib.pyplot as plt

sns.set()

best = open("C:/Users/user/source/repos/flask-post/flask-post/bestFitness.txt", "r")
bestFitnessList = best.read().split()
bestFitness = list(map(float, bestFitnessList))
print(bestFitness)

average = open("C:/Users/user/source/repos/flask-post/flask-post/avgFitness.txt", "r")
avgFitnessList = average.read().split()
avgFitness = list(map(float, avgFitnessList))
print(avgFitness)

plt.plot(bestFitness, label='Best Fitness')
plt.plot(avgFitness, label='Average Fitness')
# plt.axhline(y=15.00, color='g', label='Threshold Fitness')
plt.legend(loc="upper right")

plt.show()
