from io import TextIOWrapper
from typing import Dict, List
from graphics import *
import time

WIDTH = HEIGHT = 600
NUM_PROCESSORS = 4

win = GraphWin("Data Visualiser", WIDTH, HEIGHT, autoflush=False)

win.setBackground("white")

scale = WIDTH / 200
offset = WIDTH / 2


def applyTransforms(x, y):
    x *= scale
    y *= scale
    x += offset
    y += offset
    return [x, y]


class Agent:
    def __init__(this, id: str, x: float, y: float):
        this.shape = Oval(Point(0, 0), Point(5, 5))
        this.shape.move(x, y)
        this.shape.setFill("black")
        this.shape.draw(win)
        this.id = id

    def move(this, x, y):
        cPoint = this.shape.getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        this.shape.move(x - cPos[0], y - cPos[1])


startTime = 0

fileReaders = []

for i in range(NUM_PROCESSORS):
    fileReaders.append(open("../output/virus_pos_data_{}.dat".format(i), 'r'))


def mainLoop(fileReaders: List[TextIOWrapper], agents: Dict[str, Agent]):
    currentRead = 0
    cReader: TextIOWrapper = fileReaders[currentRead]
    while(True):
        s = cReader.readline()
        if len(s) == 0:
            break
        [command, payload] = s.split("\n")[0].split(":")
        if command == "tick":
            currentRead = (currentRead + 1) % NUM_PROCESSORS
            cReader = fileReaders[currentRead]
        elif command == "setpos":
            # print(list(agents))
            for locUpdate in payload.split(","):
                if len(locUpdate) == 0:
                    break
                [partID, startR, x, y] = locUpdate.split("|")
                [x, y] = applyTransforms(float(x), float(y))
                id = partID + "|" + startR
                agents[id].move(x, y)
        elif command == "created":
            [x, y] = applyTransforms(0, 0)
            agents[payload] = Agent(payload, x, y)
        else:
            print("ERROR: COMMAND NOT RECOGNISED")
        update()


mainLoop(fileReaders, {})
