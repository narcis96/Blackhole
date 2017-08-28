#!/usr/bin/python
# -*- coding: utf-8 -*-
from random import random
from random import seed
from math import exp
import sys,  json
from array import *
class NeuralNetwork:

    def __init__(self, layers):
        self.__network = layers

    def __activate(self, weights, inputs):
        activation = weights[-1]  # bias
        assert(len(weights)-1 == len(inputs))
        for i in range(len(weights) - 1):
            activation += weights[i] * inputs[i]
        sys.stderr.flush()
        return activation

    # tansig function
    def __transfer(self, activation):
        return 2.0 / (1.0 + exp(-2.0*activation)) - 1

    def __forward_propagate(self, row):
        inputs = row
        for layer in self.__network:
            new_inputs = []
            new_inputs = [self.__transfer(self.__activate(neuron, inputs)) for neuron in layer]
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
        self.__positions = array('i',[0 for i in range(size)])#'i' -> Represents signed integer of size 2 bytes
        #self.__values = array('i',[0 for i in range(values)])
        self.__size = size
        self.__blocked = []
        self.__used = []

    def SetBlocked(self, index):
        self.__positions[index] = -1
        self.__blocked.append(index)

    def SetOpponet(self, index, value):
        self.__positions[index] = -(value+1)
#        self.__values[value-1] = -1
        self.__blocked.append(index)

    def SetMine(self, index, value):
        self.__positions[index] = value + 1
#        self.__values[value-1] = 1
        self.__blocked.append(index)
        self.__used.append(value - 1)

    def GetTurn(self):
        outputs = self.__neuralNetwork.predict(self.__positions)
        positions = outputs[:self.__size]
        values = outputs[self.__size:]
        for position in self.__blocked:
            positions[position] = -2
        for value in self.__used:
            values[value] = -2
        return positions.index(max(positions)), 1 + values.index(max(values))

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
        blockedCells = int(sys.argv[sys.argv.index('-blockedCells') + 1])
        moves = int(sys.argv[sys.argv.index('-moves') + 1])
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
        index = int(input)

        #index = ConvertToIndex(line = input[0], pos = input[1])

        player1.SetBlocked(index)

    input = sys.stdin.readline()
    if input == 'Start\n':
        turn = 1
    else:
        turn = 0
    for i in range(2*moves):
        if turn == 1:
            pos, val = player1.GetTurn()
            player1.SetMine(pos,val)
            #pos = ConvertToCell(indx = pos)
            sys.stdout.write(str(pos) + '=' + str(val) + '\n')
            sys.stdout.flush()
            turn = 0
        else:
            if i > 0:
                input = sys.stdin.readline()
            pos, val = map(int, input.split('='))
            #pos = ConvertToIndex(line = pos[0], pos = pos[1])
            player1.SetOpponet(int(pos), int(val))
            turn = 1
if __name__ == '__main__':
#    for i in range(36):
#        cell = ConvertToCell(i)
#        print(ConvertToIndex(cell[0], cell[1]))
    layers, blockedCells, moves = ParseParameters(sys.argv)
    totalCells = 2*moves + blockedCells + 1

    player1 = Player(NeuralNetwork(layers), size = totalCells, values = moves)
    PlayGame(player1, blockedCells, moves)
