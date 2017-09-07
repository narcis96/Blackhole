from tkinter import *
import json
from script import Compile
from script import Server
from Bots import PythonBot, CBot
def Back():
    print('Back')
def Next():
    print('Next')

class Application(Frame):
    def say_hi(self):
        print ("hi there, everyone!")

    def createWidgets(self):

        self.backButton = Button(text = 'Back', fg = 'blue', command = Back)
        self.backButton.pack(side = LEFT)


        self.nextButton = Button(text = 'Next', fg = 'blue', command = Next)
        self.nextButton.pack(side = LEFT)

    def __init__(self, master = None, width = '800', height= '600'):
        Frame.__init__(self, master)
        self.pack()
        master.geometry(width + 'x' + height)
        master.resizable(width=False, height=False)
        self.createWidgets()

if __name__ == '__main__':
    #path = sys.argv[sys.argv.index('-path') + 1]
    with open('params.json') as data_file:
        params = json.load(data_file)
    root = Tk()
    if Compile(path = params['serverPath'], to = params['serverName'], compileOptions = params['compileOptions']) != 0:
        exit(-1)

    if params['sourcePath'].endswith(".cpp"):
        if Compile(path=params['sourcePath'], to=params['sourceName'], compileOptions = params['compileOptions']) != 0:
            exit(-1)
    print ('Compile successful')
    bot1 = CBot(weights=[0.7, 0.85, 1],executable = params['sourceName'], probabilities = [100, 0, 0, 0], func = 'log', startMoves=4, step3=16, step4=13, stopFinal=9, toErase = -1)
    bot2 = CBot(weights=[0.7, 0.85, 1],executable = params['sourceName'], probabilities = [100, 0, 0, 0], func = 'log', startMoves=4, step3=15, step4=13, stopFinal=9, toErase = -1)
    scores = Server(bot1, bot2, params)
    print (scores)
    app = Application(master = root, width = params['width'], height = params['height'])
    app.mainloop()
    #root.destroy()
