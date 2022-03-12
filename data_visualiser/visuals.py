from io import TextIOWrapper
import tkinter
from typing import Dict, List
from graphics import *
import time
import cProfile
from PIL import Image

import tkinter as tk


WIDTH = HEIGHT = 600
NUM_PROCESSORS = 4
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
    def __init__(this, id: str, t: int, x: float, y: float):
        this.shape = Oval(Point(0, 0), Point(5, 5))
        this.shape.move(x, y)
        this.shape.setFill("black")
        this.shape.draw(win)
        this.id = id
        this.t = t  # type

    def move(this, x, y):
        cPoint = this.shape.getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        this.shape.move(x - cPos[0], y - cPos[1])


class Virus(Agent):
    def __init__(this, id: str, t: int, x: float, y: float):
        this.shape = Rectangle(Point(0, 0), Point(5, 5))
        this.shape.move(x, y)
        this.shape.setFill("black")
        this.shape.draw(win)
        this.id = id
        this.t = t  # type


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
                saveImage(tickCount)
                tickCount += 1
                print("tick ", tickCount)
            currentRead = (currentRead + 1) % NUM_PROCESSORS
            cReader = fileReaders[currentRead]
            
        elif command == "setpos":
            # print(list(agents))
            for locUpdate in payload.split(","):
                if len(locUpdate) == 0:
                    break
                [partID, startR, agentType, x, y] = locUpdate.split("|")
                [x, y] = applyTransforms(float(x), float(y))
                id = partID + "|" + startR + "|" + agentType
                agents[id].move(x, y)
        elif command == "created":
            [_, _, agentType] = payload.split("|")
            [x, y] = applyTransforms(0, 0)
            agents[payload] = Agent(payload, agentType, x, y)
        else:
            print("ERROR: COMMAND NOT RECOGNISED")
        


mainLoop(fileReaders, {})
#cProfile.run("mainLoop(fileReaders, {})")
