import os
from math import floor
from random import random
from DNA import  DNA
from script import Battle
from Bots import  PythonBot
class Population(object):
    def __init__(self, count, mutationRate, arhitecture, managerName, serverName,toBattle, rounds):
        self.__count = count
        self.__mutationRate = mutationRate
        self.__population = [DNA(arhitecture) for i in range(count)]
        self.__scores = [0 for i in range(count)]

        self.__managerName = managerName
        self.__serverName = serverName
        self.__toBattle = toBattle
        self.__rounds = rounds
        os.makedirs('temp', exist_ok = True)
        self.__names = ['./temp/' + str(i) + '.json' for i in range (count)]


    def CalcFitness(self):
        bots = []
        for i in range(len(self.__population)):
            self.__population[i].WriteJson(self.__names[i])
            bots.append(PythonBot('python main.py', self.__names[i], 5, 15))
        bots.extend(self.__toBattle)
        scores = Battle(self.__managerName, self.__serverName, self.__rounds, bots, debug = True)

        self.__scores = scores[0:self.__count]
        return  self.__scores

    def NaturalSelection(self):
        maxScore = max(self.__scores)
        self.__matingPool = list()
        for i, dna in self.__population:
            score = self.__scores[i]/maxScore
            n = floor(score) * 10
            for j in range(n):
                self.__matingPool.append(dna)

    def Generate(self):
        self.__population = list()
        for i in range(self.__count):
            index1 = random() * len(self.__matingPool)
            index2 = random() * len(self.__matingPool)
            parent1 = self.__matingPool[index1]
            parent2 = self.__matingPool[index2]
            child = parent1.CrossOver(parent2)
            child.Mutate(self.__mutationRate)
            self.__population.append(child)

