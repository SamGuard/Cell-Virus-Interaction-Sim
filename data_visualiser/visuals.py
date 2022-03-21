from io import TextIOWrapper
from typing import Dict, List
from warnings import catch_warnings
from graphics import *
import time
import cProfile
from PIL import Image

import tkinter as tk

#States
STATE_DEAD = 0
STATE_HEALTHY = 1
STATE_INFECTED = 2
STATE_EMPTY = 3




WIDTH = HEIGHT = 1281
NUM_PROCESSORS = 32
CELL_SIZE = WIDTH / 320
MAX_LAYERS = 2
global _root
win = GraphWin("Data Visualiser", WIDTH, HEIGHT, autoflush=False)

win.setBackground("white")

scale = WIDTH / 200
offset = 0


def saveImage(tick):
    tk.Canvas.postscript(win, file="tmp.eps")
    img = Image.open("tmp.eps")
    img.save("images/tick_" + str(tick) + ".png", "png")


def applyTransforms(x, y):
    x *= scale
    y *= scale
    x += offset
    y += offset
    return [x, y]


class Agent:
    def __init__(this, id: str):
        this.shape = None
        this.id = id
        this.state = STATE_HEALTHY
        this.layer = 0

    def move(this, x, y):
        cPoint = this.shape.getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        this.shape.move(x - cPos[0], y - cPos[1])

    def initGraphics(this, x: float, y: float):
        this.shape = Oval(Point(0, 0), Point(5, 5))
        this.shape.move(x, y)
        this.shape.setFill("black")
        this.shape.draw(win)
    
    def setState(this, state: int):
        this.state: int = state

    def update(this):
        pass


class Virus(Agent):
    
    def initGraphics(this, x: float, y: float):
        this.shape = Oval(Point(0, 0), Point(3, 3))
        this.shape.move(x, y)
        this.shape.setFill("red")
        this.shape.setOutline("red")
        this.shape.draw(win)
        this.layer = 1
    
    def update(this):
        if(this.state == STATE_HEALTHY):
            this.shape.setFill("red")
        else:
            print("Invalid virus state")


class Cell(Agent):    
    def move(this, x, y):
        x = x + CELL_SIZE / 2
        y = y + CELL_SIZE / 2
        cPoint = this.shape.getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        this.shape.move(x - cPos[0], y - cPos[1])

    def initGraphics(this, x: float, y: float):
        this.shape = Rectangle(Point(0, 0), Point(CELL_SIZE, CELL_SIZE))
        this.shape.move(x, y)
        this.shape.setOutline("white")
        this.shape.draw(win)
        this.layer = 0

    
    def update(this):
        if(this.state == STATE_EMPTY):
            this.shape.setFill("black")
            this.shape.setOutline("black")
        elif(this.state == STATE_HEALTHY):
            this.shape.setFill("white")
            this.shape.setOutline("white")
        elif(this.state == STATE_INFECTED):
            this.shape.setFill("purple")
            this.shape.setOutline("purple")
        elif(this.state == STATE_DEAD):
            this.shape.setFill("grey")
            this.shape.setOutline("grey")
        else:
            print("Invalid virus state", this.state)
    


startTime = 0

fileReaders = []

for i in range(NUM_PROCESSORS):
    fileReaders.append(open("../output/sim_{}.dat".format(i), 'r'))


def getNextLine(fileReader: TextIOWrapper, buff: List[str]) -> str:
    if(len(buff) == 0):
        # read in a set of lines
        for i in range(1000):
            line = fileReader.readline()
            if len(line) == 0:
                break
            buff.append(line)

        if len(buff) == 0:
            return ""
        return buff.pop(0)
    return buff.pop(0)


def mainLoop(fileReaders: List[TextIOWrapper], agents: Dict[str, Agent]):
    currentRead = 0
    cReader: TextIOWrapper = fileReaders[currentRead]
    buff: List[List[str]] = []

    for i in range(NUM_PROCESSORS):
        buff.append([])

    tickCount: int = 0
    while(True):
        s = getNextLine(cReader, buff[currentRead])
        if len(s) == 0:
            break
        [command, payload] = s.split("\n")[0].split(":")
        if command == "tick":
            if(currentRead == NUM_PROCESSORS - 1):
                update()
                #time.sleep(1)
                saveImage(tickCount)
                tickCount += 1
                print("tick ", tickCount)
            currentRead = (currentRead + 1) % NUM_PROCESSORS
            cReader = fileReaders[currentRead]
            
        elif command == "setpos":
            # print(list(agents))
            for entry in payload.split(","):
                if len(entry) == 0:
                    break
                [partID, startR, agentType, x, y] = entry.split("|")
                [x, y] = applyTransforms(float(x), float(y))
                id = partID + "|" + startR + "|" + agentType
                agents[id].move(x, y)
        elif command == "setstate":
            for entry in payload.split(","):
                if len(entry) == 0:
                    break
                [partID, startR, agentType, s] = entry.split("|")
                id = partID + "|" + startR + "|" + agentType
                agents[id].setState(int(s))
                agents[id].update()

        elif command == "create":
            [_, _, agentType] = payload.split("|")
            [x, y] = applyTransforms(0, 0)
            agentType = int(agentType)
            agent = None
            if agentType == 1:
                agent = Virus(payload)
            elif agentType == 2:
                agent = Cell(payload)
            else:
                print("INVALID AGENT TYPE", agentType)
            
            agent.initGraphics(x, y)
            agent.update()
            agents[payload] = agent
        elif command == "kill":
            try:
                agents[payload].shape.undraw()
                agents.pop(payload)
            except:
                pass

        elif command == "sortlayers":
            keyList = list(agents.keys())
            for l in range(MAX_LAYERS):
                for key in keyList:
                    a: Agent = agents[key]
                    if(a.layer > l):
                        a.shape.undraw()
                    elif(a.layer == l):
                        try:
                            a.shape.draw(win)
                        except:
                            pass
        else:
            print(command)
            print("ERROR: COMMAND NOT RECOGNISED")
        


mainLoop(fileReaders, {})
#cProfile.run("mainLoop(fileReaders, {})")
