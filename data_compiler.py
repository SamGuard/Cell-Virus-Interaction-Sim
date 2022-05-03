import csv
from typing import List

NUM_BATCHES = 2
VIRUS_THRESHOLD = 1000


def getData(batchNum):
    f = open(f"output/agent_totals_data_{batchNum}.csv", "r")
    reader = csv.reader(f)

    fields = next(reader)

    rows = []
    for r in reader:
      rows.append(list(map(lambda y: float(y), r)))
    
    maxVirus = max(rows, key=lambda x: x[2])
    maxInnate = max(rows, key=lambda x: x[7])
    timeToCure: List

    isAboveMin = True
    for r in rows:
      virusCount = r[2]
      if(virusCount > VIRUS_THRESHOLD):
        isAboveMin = True
      elif(isAboveMin == True):
        timeToCure = r
        isAboveMin = False     


    print(maxVirus)
    print(maxInnate)
    print(timeToCure)

    return [maxVirus[2], maxInnate[7], maxVirus[0], maxInnate[0], timeToCure[0]]


def main():
    out = [["Max_Virus_Count", "Max_Innate_Count", "Time_At_Max_Virus", "Time_At_Max_Innate", "Time_To_Cure"]]
    for i in range(NUM_BATCHES):
        # load csv
        # get fields
        # get max virus count and time
        # get max innate count
        # get time until virus is less than 100
        # write to csv
        out.append(getData(i))
    
    f = open("output/compiled_data.csv", "w")
    writer = csv.writer(f)
    writer.writerows(out)


main()
