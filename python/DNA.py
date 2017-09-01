from random import random
from argparse import Namespace
import json
class DNA(object):

    def __init__(self, arhitecture, network):
        assert (len(arhitecture) > 2)
        self.arhitecture = arhitecture
        self.network = network

    @classmethod
    def Random(cls, arhitecture):
        network = []
        for i in range(1, len(arhitecture)):
            hiddenLayer = [[random()*5] * (arhitecture[i - 1] + 1)] *arhitecture[i]
            network.append(hiddenLayer)
        return cls(arhitecture, network)

    def CrossOver(self, other, fromFirst):
        network = self.network
        for i,layer in enumerate(network):
            for j in enumerate(layer):
                if (random() > fromFirst):
                    network[i][j] = other.network[i][j]
        return DNA(self.arhitecture,network)

    def Mutate(self, mutationRate):
        for i,layer in enumerate(self.network):
            for j,neuron in enumerate(layer):
                if (random() < mutationRate):
                    self.network[i][j] = [random()*5] * len(neuron)

    def __ToJson(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)

    def WriteJson(self, path):
        with open(path, 'w') as outfile:
            json.dump(self.__ToJson(), outfile)

    def WriteNetworkJson(self, path):
        with open(path, 'w') as outfile:
            json.dump(self.network, outfile)

    @staticmethod
    def json2obj(data):
        return json.loads(data, object_hook=lambda d: Namespace(**d))

    @staticmethod
    def ReadFromJson(path):
        with open(path) as data_file:
            param = DNA.json2obj(json.load(data_file))
        network = [[neuron for neuron in layer]for layer in param.network]
        return DNA(param.arhitecture, network)
