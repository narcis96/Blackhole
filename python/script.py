import sys, os, subprocess, platform
from Bots import PythonBot, CBot

def Compile(path, to):
    command = 'clang -Wall -o'+ to + ' ' + path + ' -O3 -std=c++11 -lstdc++'
    return os.system(command)

def GetManager(managerExecutable, serverExecutable, rounds, players, debug = False):
    command = [];
    command.append(managerExecutable)
    command.append('-rounds')
    command.append(str(rounds))
    command.append('-server')
    command.append(serverExecutable)
    for player in players:
        command.append('-player')
        command.append(player.executable + ' ' + str(player))
    if debug == True :
        command.append('-debug')
    return command

def Battle(managerName, serverName, rounds, bots):
    manager = GetManager('./' + managerName, './' + serverName, rounds, bots)
    proc = subprocess.Popen(manager, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    for line in output.splitlines():
        print(line)

def Compile(managerPath, managerName, serverPath, serverName):
    return Compile(serverPath, serverName) != 0 or Compile(managerPath, managerName) != 0

if __name__ == '__main__':
    if len(sys.argv) < (1 + 2*5):
        print('Not enough parameters')
        sys.exit(-1)
    sourceName = 'botc++'
    serverName = 'server'
    managerName = 'manager'
    sourcePath = sys.argv[sys.argv.index('-source') + 1]
    serverPath = sys.argv[sys.argv.index('-server') + 1]
    managerPath = sys.argv[sys.argv.index('-manager') + 1]
    path = sys.argv[sys.argv.index('-path') + 1]
    rounds = sys.argv[sys.argv.index('-rounds') + 1]    
    if Compile(sourcePath, sourceName) != 0 or Compile(managerPath, managerName, serverPath, serverName):
       sys.exit(-1)
    print ('Compile successful')
#    with open(path) as data_file:
#        bots = json.load(data_file)
    bot1 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=16, step4=15, stopFinal=9, toErase =-1)
    bot3 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=13, step4=12, stopFinal=9, toErase = -1)
    Battle(managerName, serverName, rounds, bots, debug = True)

