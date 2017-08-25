import os
from math import floor
from random import random
from DNA import  DNA
from script import Battle
from Bots import  PythonBot
class Population(object):
    def __init__(self, population, mutationRate):
        self.__count = len(population)
        self.__mutationRate = mutationRate
        self.__population = population
        self.__scores = [0 for i in range(count)]

        os.makedirs('temp', exist_ok = True)
        self.__names = ['./temp/' + str(i) + '.json' for i in range (count)]


    def CalcFitness(self, managerName, serverName, extraBots, blockedCells, moves, rounds):
        bots = []
        for i, dna in self.__population:
            dna.WriteNetworkJson(self.__names[i])
            bots.append(PythonBot('python main.py', self.__names[i], blockedCells, moves))
        bots.extend(extraBots)
        scores = Battle(managerName, serverName, rounds, bots, debug = True)

        self.__scores = scores[0:self.__count]
        return self.__scores

    def NaturalSelection(self):
        maxScore = max(self.__scores)
        self.__matingPool = []
        for i, dna in self.__population:
            score = self.__scores[i]/maxScore
            n = floor(score) * 10
            for j in range(n):
                self.__matingPool.append(dna)

    def Generate(self):
        self.__population = []
        for i in range(self.__count):
            index1 = random() * len(self.__matingPool)
            index2 = random() * len(self.__matingPool)
            parent1 = self.__matingPool[index1]
            parent2 = self.__matingPool[index2]
            child = parent1.CrossOver(parent2)
            child.Mutate(self.__mutationRate)
            self.__population.append(child)

