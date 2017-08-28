import sys, os, subprocess, platform, json
from Population import  Population
import statistics as stats
from script import Compile
from Bots import CBot, PythonBot
from DNA import DNA
class Sketch(object):
    def __init__(self, population, mutation):
        self.__population = Population(population, mutation)

    def __Print(self, generation, scores):
        saveFolder = './save/' + str(generation)
        os.makedirs(saveFolder, exist_ok = True)
        print(scores, file = open(saveFolder + '/scores.txt', 'w'))
        for i, dna in enumerate(self.__population.GetDNAs()):
            dna.WriteJson(path = saveFolder + '/' + str(i) + '.json')
        average = stats.mean(scores)
        print(average, file = open(saveFolder + '/average.txt', 'w'))
        print('generation:', str(generation), 'average score:', average)

    def run(self, params):
        generation = 1
        while True:
            scores = self.__population.CalcFitness(params)
            self.__Print(generation, scores)
            self.__population.NaturalSelection()
            self.__population.Generate()
            generation = generation + 1

#-source ../mainPlayer/mainPlayer/main.cpp -server ../Server/Server/main.cpp -manager ../Manager/Manager/main.cpp -rounds 1 -population 2 -debug 1 -mutation 0.01
def GetPopulation(params):
    population = []
    if 'population' in params:
        count = params['population']
        total = 2*params['moves'] + params['blockedCells'] + 1
        arhitecture =  params['arhitecture']
        arhitecture = [total]  + arhitecture
        arhitecture.append(total + params['moves'])
        population = [DNA.Random(arhitecture) for i in range (count)]
    else:
        files = os.listdir(params['importFrom'])
        for file in files:
           if file.endswith(".json"):
               with open(path + '/' + file) as jsonfile:
                   population.append(DNA.ReadFromJson(jsonfile))
    return population
def GetBots(path):
    bots = []
    files = os.listdir()
    for file in files:
        if file.endswith(".json"):
            with open(path + '/' + file) as jsonfile:
                 bots.append(CBot.ReadFromJson(jsonfile))
    return bots
if __name__ == '__main__':
    path = sys.argv[sys.argv.index('-params') + 1]
    with open(path) as paramFile:
        dictParam = json.load(paramFile)

    bot1 = PythonBot ('python3 main.py', 'ala', 5, 15)
    bot1.WriteJson('pybot.json')

    bot2 = PythonBot.ReadFromJson('pybot.json')

    print(bot1)
    print(bot2)
    print(dictParam)


#    dictParam = dict()
 #   dictParam['path'] = ''
  #  dictParam['blockedCells'] = 5
   # dictParam['moves'] = 15;
    #with open('bot.json', 'w') as outfile:
     #   json.dump(pybot._asdict(), outfile)
    #with open('bot.json') as data_file:
     #   pybot2Param = json.load(data_file)

 #   pybot2 = PythonBot(pybot2Param['executable'], pybot2Param['path'], pybot2Param['blockedCells'], pybot2Param['moves'])
 #   print(pybot2Param)
 #   exit(0)
 #   bot1 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=16, step4=15, stopFinal=9, toErase =-1)
#    bot2 = CBot(weights=[0.6, 0.8,  1],executable = './' + sourceName, startMoves=5, step3=13, step4=12, stopFinal=9, toErase = -1)
#    toBattle = []
#    toBattle.append(bot1)
#    toBattle.append(bot2)
#    population, mutationRate, sourcePath, serverPath, managerPath, rounds, levelPath = ParseParameters(sys.argv)

    if Compile(dictParam['sourcePath'], dictParam['sourceName']) != 0 or Compile(dictParam['serverPath'], dictParam['serverName']) != 0 or Compile(dictParam['managerPath'], dictParam['managerName']) != 0:
       sys.exit(-1)
    print ('Compile successful')
    dictParam['extraBots'] = []#GetBots(dictParam['botsPath'])
    population = GetPopulation(dictParam)
    runner = Sketch(population, dictParam['mutation'])
    runner.run(dictParam)