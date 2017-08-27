import sys, os, subprocess, platform
from Bots import PythonBot, CBot
import json
def Compile(path, to):
    command = 'clang -Wall -o'+ to + ' ' + path + ' -O3 -std=c++11 -lstdc++'
    return os.system(command)

def Battle(params):
    command = []
    command.append('./'+params['managerName'])
    command.append('-server')
    command.append('./' + params['serverName'])
    command.append('-graphPath')
    command.append(params['graphPath'])
    command.append('-rounds')
    command.append(str(params['rounds']))
    command.append('-blockedCells')
    command.append(str(params['blockedCells']))
    command.append('-moves')
    command.append(str(params['moves']))
    command.append('-debug')
    command.append(str(params['debug']))
    command.append('-debugServer')
    command.append(str(params['debugServer']))
    for player in params['players']:
        command.append('-player')
        command.append(str(player))

    proc = subprocess.Popen(command, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    scores = [int(line.decode("utf-8")) for line in output.splitlines()]
    return  scores

if __name__ == '__main__':
    if len(sys.argv) < (1 + 2*4):
        print('Not enough parameters')
        sys.exit(-1)
    #path = sys.argv[sys.argv.index('-path') + 1]
    with open('params.json') as data_file:
        params = json.load(data_file)
    sourceName = 'botc++'
    serverName = 'server'
    managerName = 'manager'
    sourcePath = params['sourcePath']
    serverPath = params['serverPath']
    managerPath = params['managerPath']
    rounds = params['rounds']
    if Compile(sourcePath, sourceName) != 0 or Compile(serverPath, serverName) != 0 or Compile(managerPath, managerName) != 0:
       sys.exit(-1)
    print ('Compile successful')

    bot1 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=16, step4=14, stopFinal=9, toErase = 20)
    bot2 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=14, step4=13, stopFinal=9, toErase = 20)
    bot3 = CBot(weights=[0.7, 0.85, 1],executable = './' + sourceName, startMoves=4, step3=13, step4=12, stopFinal=9, toErase = 20)
    '''
    cmd = []
    cmd.append('./' + serverName)
    cmd.append('-player1')
    cmd.append(str(bot1))
    cmd.append('-player2')
    cmd.append(str(bot3))
    cmd.append('-graphPath')
    cmd.append(params['graphPath'])
    cmd.append('-blockedCells')
    cmd.append(str(params['blockedCells']))
    cmd.append('-moves')
    cmd.append(str(params['moves']))
    cmd.append('-debug')
    cmd.append(str(params['debug']))
    print(cmd)
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    scores = [line.decode("utf-8") for line in output.splitlines()]
    print(scores)
    '''
    bots = []
    bots.append(bot1)
    bots.append(bot2)
    bots.append(bot3)
    params['players'] = bots
    scores = Battle(params)
    print (scores)

