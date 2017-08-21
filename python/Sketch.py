import sys, os, subprocess, platform, json
import Population
class Sketch(object):
    def __init__(self, name):
        self.__name = name
    def setup(self, count):
        self.__population = Population(count)
    def run(self):
        while True:
            x = x + 1

if __name__ == '__main__':
    fileName = 'data.json'
    command = list()
    command.append(sys.executable)
    command.append('main.py')
    layers = [
            [{'weights': [1, 0, 0]}],
            [{'weights': [1, 0]}, {'weights': [-1, 0]}, {'weights': [1, 0]}, {'weights': [-1, 0]}]
            ]
    with open(fileName, 'w') as outfile:
        json.dump(layers, outfile)

    command.append(fileName)
    proc = subprocess.Popen(command, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    for line in output.splitlines():
        print(line)
    population = sys.argv[sys.argv.index('-population') + 1]
    runner  = Sketch(population) 

