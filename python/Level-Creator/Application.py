from tkinter import *
import json
from math import cos, sin, sqrt, radians, pi
#from script import Compile
#from script import Server
#from Bots import PythonBot, CBot
def Back():
    print('Back')
def Next():
    print('Next')
#class Level:

def poly(canvas, n, xscale=None, yscale=None, posx=None, posy=None,
         linewidth=1, fill='', tags=''):
    '''
    Draws an n-gon with color set by fill, tagged with tags
    (space separated). Returns the objectID.
    '''
    if xscale is None: xscale = float(canvas['width']) / 2
    if yscale is None: yscale = float(canvas['height']) / 2
    if posx is None: posx = float(canvas['width']) / 2 + .5
    if posy is None: posy = float(canvas['height']) / 2 + .5

    xscale, yscale = float(xscale), float(yscale)
    posx, posy = float(posx), float(posy)
    polypoints = []
    for i in range(n + 1):
        theta = i * pi * 2 / n
        x = posx + xscale * cos(theta)
        y = posy - yscale * sin(theta)
        polypoints.append(x)
        polypoints.append(y)

    return canvas.create_polygon(polypoints,fill=fill, outline='black',
                                 width=linewidth, tags=tags)

def draw(canvas,startX, startY, length, color, tags):
    angle = 60
    coords = []

    for i in range(6):
        end_x = startX + length * cos(radians(angle * i))
        end_y = startY + length * sin(radians(angle * i))
        coords.append(startX)
        coords.append(startY)
        startX = end_x
        startY = end_y
    canvas.create_polygon(coords, fill = color, outline="gray", tags = tags)

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

    def click(self, event):
        print ('click!', self.canvas.winfo_containing(event.x, event.y))

    def addMenus(self, master):
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

    def __init__(self, master = None, width = '800', height= '600'):
        Frame.__init__(self, master)
        self.pack()

        master.geometry(width + 'x' + height)
        master.resizable(width=False, height=False)

        self.addMenus(master)

        self.canvas = Canvas(self, width = 800, height = 600)
        self.canvas.pack(fill=BOTH, expand=YES)
        self.canvas.bind('<Button-1>', self.click)

        cols = {
            10: 'red', 9: 'green', 8: 'blue', 7: 'yellow', 6: 'purple',
            5: 'red', 4: 'green', 3: 'blue', 2: 'yellow', 1: 'purple'
        }
        w = 800
        h = 600
        '''
        for size in range(10, 0, -1):
            xscale, yscale = (size / 10.0 * w / 2), (size / 10.0 * h / 2)
            id = poly(self.canvas,6,
                      xscale=xscale, yscale=yscale,
                      posx=w / 2, posy=h / 2,
                      linewidth=size / 2, fill=cols[size])
            print (id)
        '''
        cols = 2
        rows = 4
        size = 40
        for c in range(cols):
            if c % 2 == 0:
                offset = size * sqrt(3) / 2
            else:
                offset = 0
            for r in range(rows):
                draw(self.canvas,
                    c * (size * 1.5),
                    (r * (size * sqrt(3))) + offset,
                    size,
                    "#a1e2a1",
                    "{}.{}".format(r, c))
#            draw(self.canvas, 50, 50, 50, '#a1e2a1',  1)

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
