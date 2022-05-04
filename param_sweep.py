import sys
import subprocess
from os.path import exists
from functools import reduce
from tracemalloc import start
from typing import Dict
import time
import itertools

PROCS_DIM_X = 2
PROCS_DIM_Y = 3
NUM_PROCS = PROCS_DIM_X * PROCS_DIM_Y
MAX_BATCHES = 1

# Parameters
paramsBase = {
    # System parameters
    "VIS_DATA_OUTPUT": 0,
    "BATCH_NUM": 0,
    "LIFETIME": 0,
    "TIME_PER_TICK": 0,
    "VIRUS_COUNT": 0,
    "SIM_SIZE": 0,
    "DENSITY_OF_CELL": 0,

    # Cell
    "CELL_DEATH_LENGTH": 0,
    "CELL_DIVIDE_PROB": 0,
    "CELL_BYSTANDER_DEATH_TIME": 0,
    "CELL_REVERT_BYSTANDER_PROB": 0,
    "CELL_BYSTANDER_INFECT_SKIP_PROB": 0,
    "CELL_TO_BYSTANDER_PROB": 0,
    "CELL_VIRUS_SPAWN_NUMBER": 0,
    "CELL_IFN_SPAWN_NUMBER": 0,
    "CELL_TIME_TO_SPAWN_VIRUS": 0,
    "CELL_INFECT_NEIGH_CELL_PROB": 0,

    # Particles
    "VIRUS_INFECT_PROB": 0,
    "VIRUS_LIFETIME": 0,
    "VIRUS_SPEED": 0,
    "INNATE_LIFETIME": 0,
    "INNATE_SPEED": 0,
    "INNATE_RANGE": 0,
    "INNATE_KILL_VIRUS_PROB": 0,
    "INNATE_KILL_CELL_PROB": 0,
    "INNATE_RECRUIT_PROB": 0,
    "ANTIBODY_LIFETIME": 0,
    "ANTIBODY_SPEED": 0,
    "ANTIBODY_RANGE": 0,
    "ANTIBODY_KILL_PROB": 0,
    "IFN_SPEED": 0,

    # Human Response
    "AVG_TIME_TO_FIND_ANTIBODY": 0,
    "DEV_TIME_TO_FIND_ANTIBODY": 0,
    "PROB_PER_UNIT_TO_SPAWN_INNATE": 0,
    "PROB_PER_UNIT_TO_SPAWN_ANTIBODY": 0,
    "THREAT_LEVEL_SENSITIVITY": 0,
    "THREAT_LEVEL_SMOOTHING": 0
}


def loadBaseValues():
    fileHandle = open("model.props")
    text = filter(
        lambda x: x != "", map(
            lambda x: x.split("#")[0].replace("\n", "").replace(" ", ""),
            fileHandle.readlines()
        )
    )

    variables = map(
        lambda x: [x[0], float(x[1])],
        map(
            lambda x: x.split("="), text
        )
    )

    params = paramsBase.copy()
    for v in variables:
        params[v[0]] = v[1]

    params["VIS_DATA_OUTPUT"] = 0

    return params


def run(params: Dict):
    if(not exists("bin/main")):
        print("Executable file does not exist, please compile")

    proc = subprocess.Popen(
        ["mpirun", "-n", str(NUM_PROCS), "./bin/main", "config.props", "model.props",
         "procDimsX="+str(PROCS_DIM_X), "procDimsY="+str(PROCS_DIM_Y)] +
        list(map(lambda x:  f"{x}={params[x]:.8f}", params.keys())),
        start_new_session=False, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

    # proc.wait()
    #subprocess.run(["kill", str(proc.pid)])
    return proc


def getArgs():
    pSweeps = []
    args = sys.argv
    for i in range(1, len(sys.argv), 4):
        pSweeps.append(
            {
                "var": args[i],
                "start": float(args[i+1]),
                "end": float(args[i+2]),
                "step": float(args[i+3])
            }
        )

    return pSweeps


def main():
    params = loadBaseValues()
    pSweeps = getArgs()
    valueRanges = []
    subprocess.run(["make", "del_data"])

    for x in pSweeps:
        valueRanges.append([])
        for i in range(round(x["start"] / x["step"]), round(x["end"] / x["step"]) + 1, 1):
            valueRanges[-1].append(i * x["step"])

    allIters = list(itertools.product(*valueRanges))
    procs = []
    print(allIters)
    num = 0
    for it in allIters:
        p = params.copy()

        for i in range(len(it)):
            p[pSweeps[i]["var"]] = it[i]
        p["BATCH_NUM"] = num
        print("Stared proc", num)
        procs.append(run(p))
        del p

        while(len(procs) >= MAX_BATCHES):
            for i in range(len(procs)):
                if(procs[i].poll() != None):
                    procs[i] = None
                    print("Ended proc")

            procs = list(filter(lambda x: x != None, procs))
        num += 1

    while(len(procs) > 0):
        for i in range(len(procs)):
            if(procs[i].poll() != None):
                procs[i] = None
                print("Ended proc")

        procs = list(filter(lambda x: x != None, procs))


main()
