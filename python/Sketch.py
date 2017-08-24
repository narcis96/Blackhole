import sys, os, subprocess, platform, json
from Population import  Population
import statistics as stats
from script import Compile
from Bots import CBot, PythonBot
from test import Object
from collections import namedtuple
from argparse import Namespace

class Sketch(object):
    def __init__(self, graphPath):
        self.__graphPath = graphPath

    def setup(self, params):
        self.__population = Population(params['population'], params['mutation'], params['arhitecture'])
        self.__managerName = params['managerName']
        self.__serverName = params['serverName']
        self.__rounds = params['rounds']

    def __Print(self, generation, scores):
        os.makedirs(str(generation), exist_ok = True)

        for i, score in scores:
            print(score, file = './' + str(generation) + '/'+ str(i) + '.txt')

        for i, dna in self.__population.population:
            dna.WriteJson(file = './' + str(generation) + '/' + str(i) + '.json')

        print('generation:', str(generation), 'average score:', stats.mean(scores))

    def run(self):
        generation = 1
        while True:
            scores = self.__population.CalcFitness(self.__managerName, self.__serverName, self.__rounds)
            self.__Print(generation, scores)
            self.__population.NaturalSelection()
            self.__population.Generate()
            generation = generation + 1

#-source ../mainPlayer/mainPlayer/main.cpp -server ../Server/Server/main.cpp -manager ../Manager/Manager/main.cpp -rounds 1 -population 2 -debug 1 -mutation 0.01

if __name__ == '__main__':
    bot1 = PythonBot ('python main.py', 'ala', 5, 15)
    bot1.WriteToJson('pybot.json')

    bot2 = PythonBot.ReadFromJson('pybot.json')

    print(bot1)
    print(bot2)

    sourceName = 'botc++'
    serverName = 'server'
    managerName = 'manager'
    dictParam = dict()
    dictParam['path'] = ''
    dictParam['blockedCells'] = 5
    dictParam['moves'] = 15;
    pybot = PythonBot('python main.py', 'ala', 5, 15)
    print(pybot.executable)
    with open('bot.json', 'w') as outfile:
        json.dump(pybot._asdict(), outfile)
    with open('bot.json') as data_file:
        pybot2Param = json.load(data_file)
    pybot2 = PythonBot(pybot2Param['executable'], pybot2Param['path'], pybot2Param['blockedCells'], pybot2Param['moves'])
    print(pybot2Param)
    exit(0)
    bot1 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=16, step4=15, stopFinal=9, toErase =-1)
    bot2 = CBot(weights=[0.6, 0.8,  1],executable = './' + sourceName, startMoves=5, step3=13, step4=12, stopFinal=9, toErase = -1)
    toBattle = []
    toBattle.append(bot1)
    toBattle.append(bot2)
    population, mutationRate, sourcePath, serverPath, managerPath, rounds, levelPath = ParseParameters(sys.argv)

    if Compile(sourcePath, sourceName) != 0 or Compile(serverPath, serverName) != 0 or Compile(managerPath, managerName) != 0:
       sys.exit(-1)
    print ('Compile successful')

    runner  = Sketch(levelPath)
    runner.setup(params)
    runner.run()