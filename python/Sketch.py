import sys, os, subprocess, platform, json
from Population import  Population
import statistics as stats
from script import Compile
from Bots import CBot
class Sketch(object):
    def __init__(self, name):
        self.__name = name
    def setup(self, population, mutationRate, arhitecture, managerName, serverName, toBattle, rounds):
        self.__population = Population(population, mutationRate, arhitecture, managerName, serverName, toBattle, rounds)

        self.__toBattle = toBattle
        self.__rounds = rounds

    def __Print(self, generation, scores):
        average = stats.mean(scores)
        os.makedirs(str(generation))

        for i, score in scores:
            print(score, file = './' + str(generation) + '/'+ str(i) + '.txt')

        for i, dna in self.__population.population:
            dna.WriteJson(file = './' + str(generation) + '/' + str(i) + '.json')

        print('generation:', str(generation), 'average score:', average)

    def run(self):
        generation = 1
        while True:
            scores = self.__population.CalcFitness()
            self.__Print(generation, scores)
            self.__population.NaturalSelection()
            self.__population.Generate()
            generation = generation + 1

def ParseParameters(argv):
    population = int(argv[argv.index('-population') + 1])
    mutationRate = float(argv[argv.index('-mutation') + 1])
    sourcePath = argv[argv.index('-source') + 1]
    serverPath = argv[argv.index('-server') + 1]
    managerPath = argv[argv.index('-manager') + 1]
    rounds = argv[argv.index('-rounds') + 1]
    return  population, mutationRate, sourcePath, serverPath, managerPath, rounds
#-source ../mainPlayer/mainPlayer/main.cpp -server ../Server/Server/main.cpp -manager ../Manager/Manager/main.cpp -rounds 1 -population 2 -debug 1 -mutation 0.01
if __name__ == '__main__':
    sourceName = 'botc++'
    serverName = 'server'
    managerName = 'manager'
    bot1 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=16, step4=15, stopFinal=9, toErase =-1)
    bot2 = CBot(weights=[0.6, 0.8,  1],executable = './' + sourceName, startMoves=5, step3=13, step4=12, stopFinal=9, toErase = -1)
    toBattle = []
    toBattle.append(bot1)
    toBattle.append(bot2)
    population, mutationRate, sourcePath, serverPath, managerPath, rounds = ParseParameters(sys.argv)


    if Compile(sourcePath, sourceName) != 0 or Compile(serverPath, serverName) != 0 or Compile(managerPath, managerName) != 0:
       sys.exit(-1)
    print ('Compile successful')

    arhitecture = [3, 30, 30]
    rounds = 1
    runner  = Sketch(population)
    runner.setup(population, mutationRate, arhitecture, managerName, serverName, toBattle, rounds)
    runner.run()