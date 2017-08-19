import sys, os, subprocess, platform

class BotParameters(object):
    def __init__(self, weights, startMoves, step3, step4, stopFinal, toErase):
        self.__weights = weights
        self.__startMoves = startMoves
        self.__step3 = step3
        self.__step4 = step4
        self.__stopFinal = stopFinal
        self.__toErase = toErase

    def __str__(self):
        return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) \
               + ' -step3 ' + str(self.__step3) + ' -step4 ' + str(self.__step4)\
               + ' -stopFinal ' + str(self.__stopFinal) + ' -erase ' + str(self.__toErase)

def Compile(path, to):
    command = 'clang -Wall -o'+ to + ' ' + path + ' -O3 -std=c++11 -lstdc++'
    return os.system(command)

def GetManager(managerExecutable, serverExecutable, sourceExecutable, rounds, players):
    command = [];
    command.append(managerExecutable)
    command.append('-rounds')
    command.append(str(rounds))
    command.append('-server')
    command.append(serverExecutable)
    for player in players:
        command.append('-player')
        command.append(sourceExecutable + ' ' + str(player))
    return command

if __name__ == '__main__':
    if len(sys.argv) < (1 + 2*5):
        print('Not enough parameters')
        sys.exit(-1)
    sourcePath = sys.argv[sys.argv.index('-source') + 1]
    serverPath = sys.argv[sys.argv.index('-server') + 1]
    managerPath = sys.argv[sys.argv.index('-manager') + 1]
    bots = sys.argv[sys.argv.index('-bots') + 1]
    rounds = sys.argv[sys.argv.index('-rounds') + 1]
    sourceName = 'bot'
    sourceExecutable = './bot'
    serverName = 'server'
    serverExecutable = './server'
    managerName = 'manager'
    managerExecutable = './manager'
	
    if Compile(sourcePath, sourceName) != 0 or Compile(serverPath, serverName) != 0 or Compile(managerPath, managerName) != 0:
        sys.exit(-1)
    print ('Compile successful')

    bot1 = BotParameters(weights=[0.7, 0.85, 1], startMoves=4, step3=16, step4=15, stopFinal=9, toErase =-1)
    bot2 = BotParameters(weights=[0.7, 0.85, 1], startMoves=4, step3=14, step4=13, stopFinal=9, toErase=-1)
    bot3 = BotParameters(weights=[0.7, 0.85, 1], startMoves=4, step3=13, step4=12, stopFinal=9, toErase=-1)
    bots = []
    bots.append(bot1)
    bots.append(bot2)
    bots.append(bot3)
    manager = GetManager(managerExecutable, serverExecutable, sourceExecutable, rounds, bots)
    proc = subprocess.Popen(manager, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    for line in output.splitlines():
        print(line)

