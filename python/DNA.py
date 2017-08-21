from random import random
import json
class DNA(object):

    def __init__(self, networkArhitecture):
        assert (len(networkArhitecture) > 2)
        self.__network = list()
        self.__arhitecture = networkArhitecture

        for i in range(1, len(self.__arhitecture)):
            hiddenLayer = list()
            for j in range(len(self.__arhitecture[i])):
                neuron = [{'weights':[random() for j in range(len(self.__arhitecture[i-1]) + 1)]}] #+1 for bias, -1 for last layer
                hiddenLayer.append(neuron)
            self.__network.append(hiddenLayer)

    def CrossOver(self, other):
        child = DNA(self.__arhitecture)

        for i,layer in child.__network:
            for j,neuron in layer:
                if (random(1) <= 0.5):
                    child.__network[i][j] = self.__network[i][j]
                else:
                    child.__network[i][j] = other.__network[i][j]
        return child

    def Mutate(self, mutationRate):
        for i,layer in self.__network:
            for j,neuron in layer:
                if (random(1) < mutationRate):
                    self.__network[i][j]['weights'] = [random() for i in range(len(neuron['weights']))]

    def __str__(self):
        str(self.__network)

    def WriteJson(self, path):
        with open(path, 'w') as outfile:
            json.dump(str(self.__network), outfile)



