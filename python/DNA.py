from random import random
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
            hiddenLayer = list()
            for j in range(arhitecture[i]):
                    neuron = dict()
                    neuron['weights'] = [random()*5 for w in range(arhitecture[i - 1] + 1)] #+1 for bias
                    hiddenLayer.append(neuron)
            network.append(hiddenLayer)
        return cls(arhitecture, network)

    def CrossOver(self, other, fromFirst):
        network = self.network
        for i,layer in enumerate(network):
            for j,neuron in enumerate(layer):
                if (random() <= fromFirst):
                    network[i][j] = self.network[i][j]
                else:
                    network[i][j] = other.network[i][j]
        return DNA(self.arhitecture,network)

    def Mutate(self, mutationRate):
        for i,layer in enumerate(self.network):
            for j,neuron in enumerate(layer):
                if (random() < mutationRate):
                    self.network[i][j]['weights'] = [random()*5 for k in range(len(neuron['weights']))]

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
        return DNA(param.arhitecture, param.network)
