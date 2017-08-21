import sys, os, subprocess, platform, json
from Population import  Population
import statistics as stats
class Sketch(object):
    def __init__(self, name):
        self.__name = name
    def setup(self, population, arhitecture, mutationRate):
        self.__population = Population(population, arhitecture,mutationRate)

    def __Print(self, generation, scores):
        average = stats.mean(scores)
        os.makedirs(str(generation))

        for i, score in scores:
            print(score, file='./' + str(generation) + '/'+ str(i) + '.txt')

        for i, dna in population.population:
            dna.WriteJson('./' + str(generation) + '/' + str(i) + '.json')

        print('generation:', str(generation), 'average score:', average)

    def run(self,):
        generation = 1
        while True:
            scores = population.CalcFitness()
            self.__Print(generation, scores)
            population.NaturalSelection()
            population.Generate()
            generation = generation + 1


if __name__ == '__main__':
    population = sys.argv[sys.argv.index('-population') + 1]
    mutationRate = sys.argv[sys.argv.index('-mutation') + 1]
    sourcePath = sys.argv[sys.argv.index('-source') + 1]
    serverPath = sys.argv[sys.argv.index('-server') + 1]
    managerPath = sys.argv[sys.argv.index('-manager') + 1]
    arhitecture = [3, 30, 30]

    runner  = Sketch(population)
    runner.setup(population, arhitecture, mutationRate)



