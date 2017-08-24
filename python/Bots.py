import json
from argparse import Namespace
class CBot(object):
    def __init__(self, executable, weights, startMoves, step3, step4, stopFinal, toErase):
        self.__weights = weights
        self.__startMoves = startMoves
        self.__step3 = step3
        self.__step4 = step4
        self.__stopFinal = stopFinal
        self.__toErase = toErase
        self.executable = executable

    def ToParam(self):
        return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) \
               + ' -step3 ' + str(self.__step3) + ' -step4 ' + str(self.__step4)\
               + ' -stopFinal ' + str(self.__stopFinal) + ' -erase ' + str(self.__toErase)
    def __str__(self):
        return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) \
               + ' -step3 ' + str(self.__step3) + ' -step4 ' + str(self.__step4)\
               + ' -stopFinal ' + str(self.__stopFinal) + ' -erase ' + str(self.__toErase)

    def _asdict(self):
        return self.__dict__

    def defto_json(self):
        return self.__dict__

class PythonBot():
    def __init__(self, executable, path, blockedCells, moves):
        self.executable = executable
        self.path = path
        self.blockedCells = int(blockedCells)
        self.moves = int(moves)
    def __str__(self):
        return self.executable + ' -path ' + self.path +  ' -blockedCells ' + str(self.blockedCells) + ' -moves ' + str(self.moves)

    def toJSON(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)

    def WriteToJson(self, path):
        with open(path, 'w') as outfile:
            json.dump(self.toJSON(), outfile)

    @staticmethod
    def json2obj(data):
        return json.loads(data, object_hook=lambda d: Namespace(**d))

    @staticmethod
    def ReadFromJson(path):
        def json2obj(data):
            return json.loads(data, object_hook=lambda d: Namespace(**d))
        with open(path) as data_file:
            param = json2obj(json.load(data_file))
        return PythonBot(param.executable, param.path, param.blockedCells, param.moves)