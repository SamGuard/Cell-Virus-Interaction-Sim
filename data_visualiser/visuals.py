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

isHeader = True

scale = WIDTH / 100
offset = 100

def applyTransforms(x, y):
  x *= scale
  y *= scale
  x += offset
  y += offset
  return [x, y]

for row in reader:
    if(isHeader):
        isHeader = False
        continue
    rect.undraw()
    rect.draw(win)
    for i in range(1, len(row), 2):
        x = float(row[i])
        y = float(row[i+1])

        [x, y] = applyTransforms(x, y)
        win.plot(x, y)
    update()
    #time.sleep()

