from graphics import *
import csv
import time

WIDTH = HEIGHT = 600

win = GraphWin("Data Visualiser", WIDTH, HEIGHT, autoflush=False)
rect = Rectangle(Point(0,0), Point(WIDTH, HEIGHT))
rect.setFill("white")
rect.draw(win)

win.setBackground("white")

csvData = open("../output/agent_pos_data.csv", mode='r', newline="")

reader = csv.reader(csvData, delimiter=',')

scale = WIDTH / 1000
offset = WIDTH / 2

def applyTransforms(x, y):
  x *= scale
  y *= scale
  x += offset
  y += offset
  return [x, y]


startTime = 0


header = next(reader)
agents = []


for agent in header:
    if(agent == "tick"):
        continue
    agents.append(Oval(Point(10,10), Point(5, 5)))
    agents[-1].setFill("black")
    agents[-1].draw(win)


for row in reader:
    startTime = time.time_ns()
    for i in range(1, len(row), 2):
        x = float(row[i])
        y = float(row[i+1])

        [x, y] = applyTransforms(x, y)
        
        cPoint = agents[i-1].getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        agents[i-1].move(x - cPos[0], y - cPos[1])

        #win.plot(x, y)
    update()
    timeDiff = 50000000 - (time.time_ns() - startTime)
    if(timeDiff > 0):
        time.sleep(timeDiff / 1000000000)
    #time.sleep()

