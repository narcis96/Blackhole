from random import random
from random import seed
from math import exp

class NeuralNetwork:
	def __init__(self, layers, n_outputs):
		network = list()
		#for i in range(len(layers)):
		#	hidden_layer = [{'weights':[random() for j in range(layers[i] + 1)]}]
		#	network.append(hidden_layer)
		#output_layer = [{'weights':[random() for i in range(n_hidden + 1)]} for i in range(n_outputs)]
		#network.append(output_layer)
		
		#hidden_layer = [{'weights':[random() for i in range(layers[i] + 1)]} for i in range(len(layers))]
		network = layers
		
		self.network = network
	
	def __activate(self, weights, inputs):
		activation = weights[-1] #bias
		for i in range(len(weights)-1):
			activation += weights[i] * inputs[i]
		return activation

	#sigmoid function
	def __transfer(self,activation):
		return 1.0 / (1.0 + exp(-activation))
	
	def __forward_propagate(self, row):
		inputs = row
		for layer in self.network:
			new_inputs = []
			for neuron in layer:
				activation = self.__activate(neuron['weights'], inputs)
				neuron['output'] = self.__transfer(activation)
				new_inputs.append(neuron['output'])
			inputs = new_inputs
		return inputs
	def __str__(self):
		string = ""
		for layer in self.network:
			string += str(layer) + "\n"
		return string
	
	def predict(self, row):
		outputs = self.__forward_propagate(row)
		return outputs.index(max(outputs))
		
if __name__ == '__main__':
	#sample layers
	layers = [
		[{'weights': [1, 1, 1]}, {'weights': [0.5, 0.5, 0.5]}],
		[{'weights': [1, 1, 1]}, {'weights': [0.2, 0.3, 0.4]}]
	]
	net = NeuralNetwork(layers,4)
	print(net)
	print (net.predict([2,3,4,5]))
