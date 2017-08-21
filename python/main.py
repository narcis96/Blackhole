#!/usr/bin/python
# -*- coding: utf-8 -*-
from random import random
from random import seed
from math import exp
import sys,  json
import multiprocessing
from array import *
class NeuralNetwork:

    def __init__(self, layers):
        self.__network = layers

    def __activate(self, weights, inputs):
        activation = weights[-1]  # bias
        assert(len(weights)-1 == len(inputs))
        for i in range(len(weights) - 1):
            activation += weights[i] * inputs[i]
        return activation

    # tansig function
    def __transfer(self, activation):
        return 2.0 / (1.0 + exp(-2.0*activation))-1

    def __forward_propagate(self, row):
        inputs = row
        for layer in self.__network:
            new_inputs = []
            for neuron in layer:
                activation = self.__activate(neuron['weights'], inputs)
                neuron['output'] = self.__transfer(activation)
                new_inputs.append(neuron['output'])
            inputs = new_inputs
        return inputs

    def __str__(self):
        string = ''
        for layer in self.__network:
            string += str(layer) + '\n'
        return string

    def predict(self, row):
        outputs = self.__forward_propagate(row)
        return outputs

class Player:
    def __init__(self, neuralNetwork, size, values):
        self.__neuralNetwork = neuralNetwork
        self.__positions = array('i',[0 for i in range(0, size)])#'i' -> Represents signed integer of size 2 bytes
        self.__values = array('i',[0 for i in range(0, values)])

    def SetBlocked(self, index):
        self.__positions[index] = -1

    def SetOpponet(self, index, value):
        self.__positions[index] = -2
        self.__values[index] = -value
    
    def SetMine(self, index, value):
        self.__positions[index] = 1
        self.__values[index] = value

    def GetTurn(self):
        outputs = neuralNetwork.predict(self.__positions)
        print(outputs)
        return outputs[0].index(max(outputs[0])), outputs[1].index(max(outputs[1]))
   
def ParseParameters(arguments):
    if len(arguments) == 1:
        layers = [
            [{'weights': [1, 0, 0]}],
            [{'weights': [1, 0]}, {'weights': [-1, 0]}, {'weights': [1, 0]}, {'weights': [-1, 0]}]
            ]
        blockedCells = 5
        moves = 15
    else:
        path = sys.argv[sys.argv.index('-path') + 1]
        blockedCells = sys.argv[sys.argv.index('-blockedCells') + 1]
        moves = sys.argv[sys.argv.index('-moves') + 1]
        with open(path) as data_file:
            layers = json.load(data_file)

    return layers, blockedCells, moves

def ConvertToIndex(line, pos)-> int:
    line = ord(line) - ord('A') + ord(pos) - ord('1')
    col = ord(pos) - ord('1')
    return int(line*(line+1)/2) + col

def ConvertToCell(indx) -> str:
    indx = indx + 1
    line = 0
    count = 0

    while True:
        if count + line + 1 >= indx:
            i = line
            j = indx - count - 1
            break
        line += 1
        count += line
    return chr(i + ord('A') - j) + chr(j + ord('1'))

def PlayGame(player1, blockedCells, moves):
    for i in range(blockedCells):
        input = sys.stdin.readline()
        index = ConvertToIndex(line = input[0], pos = input[1])
        player1.SetBlocked(index)

    input = sys.stdin.readline()
    if input == 'Start':
        turn = 1
    else:
        turn = 0
    for i in range(2*moves):
        if turn == 1:
            pos, val = player1.GetTurn()
            pos = ConvertToCell(indx = pos)
            print(pos + '=' + str(val))
            #print('Debug info', file=sys.stderr) 
            sys.stdout.flush()
            turn = 0
        else:
            if i > 0:
                input = sys.stdin.readline()
            pos = ConvertToIndex(line = input[0], pos = input[1])
            player1.SetOpponet(pos)
            turn = 1
if __name__ == '__main__':
#    for i in range(36):
#        cell = ConvertToCell(i)
#        print(ConvertToIndex(cell[0], cell[1]))
    layers, blockedCells, moves = ParseParameters(sys.argv)
    totalCells = 2*moves + blockedCells + 1
    neuralNetwork = NeuralNetwork(layers)
    player1 = Player(neuralNetwork, size = totalCells, values = moves)
    PlayGame(player1, blockedCells, moves)
