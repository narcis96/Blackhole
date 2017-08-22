import os
from math import floor
from random import random
from DNA import  DNA
from script import Battle, Compile

class Population(object):
    def __init__(self, count, arhitecture, mutationRate):
        self.__count = count
        self.__mutationRate = mutationRate
        self.__population = [DNA(arhitecture) for i in range(count)]
        self.__scores = [0 for i in range(count)]

        os.makedirs('temp')
        self.__names = ['./temp/' + str(i) + '.json' for i in range (count)]


    def CalcFitness(self):
        self.__scores = [0 for i in range(self.__count)]
        for i, dna in self.__population:
            dna.WriteJson(self.__names[i])
#to do: compute scores


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

