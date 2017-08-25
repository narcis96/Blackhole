import json
from argparse import Namespace
class CBot(object):
    def __init__(self, executable, weights, startMoves, step3, step4, stopFinal, toErase):
        self.weights = weights
        self.startMoves = startMoves
        self.step3 = step3
        self.step4 = step4
        self.stopFinal = stopFinal
        self.toErase = toErase
        self.executable = executable

    def __str__(self):
        return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) \
               + ' -step3 ' + str(self.__step3) + ' -step4 ' + str(self.__step4)\
               + ' -stopFinal ' + str(self.__stopFinal) + ' -erase ' + str(self.__toErase)

    @staticmethod
    def json2obj(data):
        return json.loads(data, object_hook=lambda d: Namespace(**d))

    @staticmethod
    def ReadFromJson(path):
        with open(path) as data_file:
            param = CBot.json2obj(json.load(data_file))
        return CBot(param.executable, param.weights, param.startMoves, param.step3, param.step4, param.stopFinal, param.toErase)

class PythonBot():
    def __init__(self, executable, path, blockedCells, moves):
        self.executable = executable
        self.path = path
        self.blockedCells = int(blockedCells)
        self.moves = int(moves)
    def __str__(self):
        return self.executable + ' -path ' + self.path +  ' -blockedCells ' + str(self.blockedCells) + ' -moves ' + str(self.moves)

    def __ToJson(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)

    def WriteJson(self, path):
        with open(path, 'w') as outfile:
            json.dump(self.__ToJson(), outfile)

    @staticmethod
    def json2obj(data):
        return json.loads(data, object_hook=lambda d: Namespace(**d))

    @staticmethod
    def ReadFromJson(path):
        with open(path) as data_file:
            param = PythonBot.json2obj(json.load(data_file))
        return PythonBot(param.executable, param.path, param.blockedCells, param.moves)