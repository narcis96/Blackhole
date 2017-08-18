import sys, os, subprocess, platform

class BotParameters(object):
	def __init__(self, weights, startMoves, stopFinal):
		self.__weights = weights
		self.__startMoves = startMoves
		self.__stopFinal = stopFinal
	def __str__(self):
		return '-weights ' + str(self.__weights) + ' -startMoves ' + str(self.__startMoves) + ' -stopFinal ' + str(self.__stopFinal)

def Compile(path, to):
	command = 'clang -Wall -o'+ to + ' ' + path + ' -O3 -std=c++1z'
	return subprocess.call(command)

def GetManagerCommand(managerExecutable, serverExecutable, rounds, players):
	manager = managerExecutable + ' -rounds ' + str(rounds) + ' -server ' + serverExecutable
	for player in players:
		manager = manager + ' -player \"' + sourceExecutable + ' ' + str(player) + '\"'
	return manager

if __name__ == '__main__':
	if len(sys.argv) < (1 + 2*5):
		print('Not enough parameters')
		sys.exit(-1)
	sourcePath = sys.argv[sys.argv.index('-source') + 1]
	serverPath = sys.argv[sys.argv.index('-server') + 1];
	managerPath = sys.argv[sys.argv.index('-manager') + 1]
	bots = sys.argv[sys.argv.index('-bots') + 1]
	rounds = sys.argv[sys.argv.index('-rounds') + 1]
	if 'Windows' in platform.platform():
		currentPlatoform = 'Windows'
		sourceName  = sourceExecutable = 'bot.exe'
		serverName  = serverExecutable = 'server.exe'
		managerName = managerExecutable ='manager.exe'
	else:
		currentPlatoform = 'POSIX'
		sourceName = 'bot'
		sourceExecutable = './bot'
		serverName = 'server'
		serverExecutable = './bot'
		managerName = 'manager'
		managerExecutable = './manager'
	
	if Compile(sourcePath, sourceName) != 0 or Compile(serverPath, serverExecutable) != 0 or Compile(managerPath, 'manager.exe') != 0:
		sys.exit(-1)
	print ('Compile successful')

	bot1 = BotParameters([0.7,0.85,1], 4, 9)
	bot2 = BotParameters([0.7,0.8, 1], 4, 9)
	manager = GetManagerCommand(managerExecutable, serverExecutable, rounds, [bot1, bot2])
	subprocess.call('server.exe bot.exe bot.exe')
#	subprocess.call('server.exe "bot.exe -weights [0.7, 0.85, 1] -startMoves 4 -stopFinal 9" "bot.exe -weights [0.7, 0.8, 1] -startMoves 4 -stopFinal 9"');
#	print(manager)
#	subprocess.call(manager)
#	os.system(manager)
	print(bot1)
