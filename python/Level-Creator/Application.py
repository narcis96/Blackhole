from tkinter import *
import json
#from script import Compile
#from script import Server
#from Bots import PythonBot, CBot
def Back():
    print('Back')
def Next():
    print('Next')
#class Level:

class Application(Frame):
    def say_hi(self):
        print ("hi there, everyone!")

    def CreateButtons(self):
        self.backButton = Button(text = 'Back', fg = 'blue', command = Back)
        self.backButton.pack(side = LEFT)


        self.nextButton = Button(text = 'Next', fg = 'blue', command = Next)
        self.nextButton.pack(side = LEFT)

    def createWidgets(self):
        B1 = Button(text ="circle", relief=RAISED)
        B1.pack(side = LEFT)
    def PlayerVsBot(self):
        x = 0
    def BotvsBot(self):
        x = 0
    def CreateLevel(self):
        print(0)
    def ImportLevel(self):
        print(1)
    def ExportLevel(self):
        print(2)
    def __init__(self, master = None, width = '800', height= '600'):
        Frame.__init__(self, master)
        self.pack()

        master.geometry(width + 'x' + height)
        master.resizable(width=False, height=False)

        menu = Menu(master)
        master.config(menu=menu)
        playMenu = Menu(menu)
        menu.add_cascade(label='Play', menu=playMenu)
        playMenu.add_command(label='Player vs Bot', command = self.PlayerVsBot)
        playMenu.add_command(label='Bot vs Bot', command = self.BotvsBot)
        playMenu.add_separator()
        playMenu.add_command(label='Exit', command=master.quit)

        createMenu = Menu(menu)
        menu.add_cascade(label='Create', menu=createMenu)
        createMenu.add_command(label='Create', command = self.CreateLevel)
        createMenu.add_command(label='Import', command = self.ImportLevel)
        createMenu.add_command(label='Export', command = self.ExportLevel)

        self.canvas = Canvas(self)
        self.canvas.create_rectangle(230, 10, 290, 60, 
            outline="#f11", fill="#1f1", width=2)

        self.canvas.pack(fill=BOTH, expand=1)
        self.createWidgets()

if __name__ == '__main__':
    #path = sys.argv[sys.argv.index('-path') + 1]
    with open('params.json') as data_file:
        params = json.load(data_file)
    root = Tk()

    '''
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
    '''
    app = Application(master = root, width = params['width'], height = params['height'])
    app.mainloop()
    root.destroy()
