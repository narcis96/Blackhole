from tkinter import *

def Boss():
    print('Boss')

class Application(Frame):
    def say_hi(self):
        print ("hi there, everyone!")

    def createWidgets(self):

        self.redbutton = Button(text = 'Boss', fg = 'blue', command = Boss)
        self.redbutton.pack(side = LEFT)
        
    def __init__(self, master = None, width = '800', height= '600'):
        Frame.__init__(self, master)
        self.pack()
        master.geometry(width + 'x' + height)
        master.resizable(width=False, height=False)
        self.createWidgets()

if __name__ == '__main__': 
    #width = sys.argv[sys.argv.index('-width') + 1]
    #height = sys.argv[sys.argv.index('-height') + 1]
    root = Tk()
    app = Application(master=root)
    app.mainloop()
    root.destroy()
