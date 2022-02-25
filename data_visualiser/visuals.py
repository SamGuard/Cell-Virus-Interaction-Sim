from graphics import *
import csv
import time

WIDTH = HEIGHT = 600
NUM_PROCESSORS = 4

win = GraphWin("Data Visualiser", WIDTH, HEIGHT, autoflush=False)
rect = Rectangle(Point(0, 0), Point(WIDTH, HEIGHT))
rect.setFill("white")
rect.draw(win)

win.setBackground("white")

scale = WIDTH / 200
offset = WIDTH / 2


def applyTransforms(x, y):
    x *= scale
    y *= scale
    x += offset
    y += offset
    return [x, y]


startTime = 0

readers = []

for i in range(NUM_PROCESSORS):
    csvData = open("../output/virus_pos_data_{}.csv".format(i),
                   mode='r', newline="")
    readers.append(list(csv.reader(csvData, delimiter=',')))


numReaders = len(readers)

fullData = []
# Create full data array
numIterations = 0
for row in readers[0][1:]:
    fullData.append([])
    for i in range(1, len(row) - 1, 2):
        fullData[numIterations].append([0, 0])

    numIterations += 1


# Put data into fullData
for reader in readers:
    currentRow = -1
    for row in reader:
        if(currentRow == -1):
            currentRow = 0
            continue
        for i in range(1, len(row) - 1, 2):
            x = row[i]
            y = row[i+1]
            if(x == "????" or y == "????"):
                continue
            fullData[currentRow][int((i-1) / 2)][0] = float(x)
            fullData[currentRow][int((i-1) / 2)][1] = float(y)
        currentRow += 1

# Init agent shapes
agents = []
for i in fullData[0]:
    agents.append(Oval(Point(10, 10), Point(5, 5)))
    agents[-1].setFill("black")
    agents[-1].draw(win)

targetTimeSeconds = 1 / 10
targetTime = targetTimeSeconds * 100000000
for row in range(len(fullData)):
    startTime = time.time_ns()
    for i in range(len(fullData[row])):
        [x, y] = fullData[row][i]
        print(x, y)
        [x, y] = applyTransforms(x, y)

        cPoint = agents[i].getP2()
        cPos = [cPoint.getX(), cPoint.getY()]
        agents[i].move(x - cPos[0], y - cPos[1])
    update()
    timeDiff = targetTime - (time.time_ns() - startTime)
    if(timeDiff > 0):
        pass
        #time.sleep(timeDiff / targetTime)
