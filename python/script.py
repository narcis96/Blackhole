import sys, os, subprocess, platform
from Bots import PythonBot, CBot
import json
from DNA import  *
def Compile(path, to, compileOptions):
    command = 'clang -o'+ to + ' ' + path;
    command = command + ' -include ' + compileOptions['include']
    for flag in compileOptions['flags']:
        command += flag
    return os.system(command)

def Battle(params):
    command = []
    command.append(params['managerName'])
    command.append('-server')
    command.append(params['serverName'])
    for key,value in params['level'].items():
        command.append('-' + str(key))
        command.append(str(value))
    for player in params['players']:
        command.append('-player')
        command.append(str(player))

    proc = subprocess.Popen(command, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    scores = [int(line.decode("utf-8")) for line in output.splitlines()]
    return  scores

def Server(bot1, bot2, params):
    cmd = []
    cmd.append(params['serverName'])
    cmd.append('-player1')
    cmd.append(str(bot1))
    cmd.append('-player2')
    cmd.append(str(bot2))
    for key,value in params['level'].items():
        cmd.append('-' + str(key))
        cmd.append(str(value))
    for key, value in params['server'].items():
        cmd.append('-' + str(key))
        cmd.append(str(value))

    print(cmd)
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    output, error = proc.communicate()
    scores = [line.decode("utf-8") for line in output.splitlines()]
    return scores


if __name__ == '__main__':
    if len(sys.argv) < 1:
        print('Not enough parameters')
        sys.exit(-1)
    #path = sys.argv[sys.argv.index('-path') + 1]
    with open('params.json') as data_file:
        params = json.load(data_file)
    sourceName = params['sourceName']
    serverName = params['serverName']
    managerName = params['managerName']
    sourcePath = params['sourcePath']
    serverPath = params['serverPath']
    managerPath = params['managerPath']
    if Compile(sourcePath, sourceName, params['compileOptions']) != 0 or \
        Compile(serverPath, serverName, params['compileOptions']) != 0 or \
        Compile(managerPath, managerName, params['compileOptions']) != 0:
        sys.exit(-1)
    print ('Compile successful')
    bot1 = CBot(weights=[0.7, 0.85, 1],executable = sourceName, probabilities = [100, 0, 0, 0], func = 'log', startMoves=4, step3=16, step4=13, stopFinal=9, toErase = -1)
    bot2 = CBot(weights=[0.7, 0.85, 1],executable = sourceName, probabilities = [100, 0, 0, 0], func = 'log', startMoves=4, step3=15, step4=13, stopFinal=9, toErase = -1)
    bot3 = CBot(weights=[0.7, 0.85, 1],executable = sourceName, probabilities = [100, 0, 0, 0], func = 'x', startMoves=4, step3=16, step4=13, stopFinal=9, toErase = -1)
    bot4 = CBot(weights=[0.7, 0.85, 1],executable = sourceName, probabilities = [100, 0, 0, 0], func = 'x', startMoves=4, step3=15, step4=13, stopFinal=9, toErase = -1)

    dna = DNA.ReadFromJson('./bots/9.json')
    #print(dna.arhitecture)
    #print(dna.network)

    #dna.WriteNetworkJson('./script/pybot.json')
#    pyBot = PythonBot('python3 main.py', './script/pybot.json', params['blockedCells'], params['moves'])
    bot1.WriteJson('./import/bot1.json')
    bot2.WriteJson('./import/bot2.json')
    scores = Server(bot1,bot2, params)
    print(scores)
    '''
    bots = []
#    bots.append(bot4)
#    bots.append(bot3)
    bots.append(bot1)
    bots.append(bot2)
    params['players'] = bots
    scores = Battle(params)
    print (scores)
    '''

