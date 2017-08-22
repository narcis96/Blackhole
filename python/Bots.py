
class CBot(object):
    def __init__(self, executable, weights, startMoves, step3, step4, stopFinal, toErase):
        self.__weights = weights
        self.__startMoves = startMoves
        self.__step3 = step3
        self.__step4 = step4
        self.__stopFinal = stopFinal
        self.__toErase = toErase
        self.executable = executable

    def __str__(self):
        return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) \
               + ' -step3 ' + str(self.__step3) + ' -step4 ' + str(self.__step4)\
               + ' -stopFinal ' + str(self.__stopFinal) + ' -erase ' + str(self.__toErase)

class PythonBot(object):
    def __init__(self, executable, path, blockedCells, moves):
        self.executable = executable
        self.__path = path
        self.__blockedCells = str(blockedCells)
        self.__moves = str(moves)

    def __str__(self):
        return '-path ' + self.__path +  ' -blockedCells ' + self.__blockedCells + ' -moves ' + self.__moves