import argparse
import random
import sys

class AntPath:
    def __init__(self):
        self.length = 0
        self.last = -1
        self.path = []

    def next(self):
        # Visit new node
        options = [i for i in range(args.nodes) if not self.path.count(i)]
        chances = map(lambda i: pheromone[self.last][i] ** ALPHA / weights[self.last][i] ** BETA, options)
        choice = random.random() * sum(chances)
        for i, chance in enumerate(chances):
            choice -= chance
            if choice < 0:
                # Pick this path
                self.walk(options[i])
                break

    def walk(self, node):
        # print "Walk to %d" % node
        self.length += weights[self.last][node]
        pheromone[self.last][node] = (1 - LOCALDECAY) * pheromone[self.last][node] + LOCALDECAY * LOCALUPDATE
        pheromone[node][self.last] = pheromone[self.last][node]
        self.path.append(node)
        self.last = node


class dotdict(dict):
    def __getattr__(self, attr):
        return self.get(attr, None)
    __setattr__= dict.__setitem__
    __delattr__= dict.__delitem__

parameters = dotdict()

INITIAL_PHEROMONE = parameters.initial_pheromone or 0.1   # Parameter: Initial pheromone trail value
ALPHA             = parameters.alpha or 1                 # Parameter: Likelihood of ants to follow pheromone trails (larger value == more likely)
BETA              = parameters.beta or 6                  # Parameter: Likelihood of ants to choose closer nodes (larger value == more likely)
LOCALDECAY        = parameters.local_decay or 0.2         # Parameter: Governs local trail decay rate [0, 1]
LOCALUPDATE       = parameters.local_update or 0.4        # Parameter: Amount of pheromone to reinforce local trail update by
GLOBALDECAY       = parameters.global_decay or 0.2        # Parameter: Governs global trail decay rate [0, 1]
# If ALPHA = 0, the closest cities are more likely to be selected.
# If BETA = 0, only pheromone amplifications is at work.
# The number of ants will directly determine the speed of the algorithm proportionally. More ants will get more optimal results, but don't use more ants than the number of nodes.
# You need more ants when there are more nodes to have more chances to find a good path quickly. The usual default is numAnts = numNodes.

parser = argparse.ArgumentParser()
parser.add_argument('--nodes', default=20, type=int)
parser.add_argument('--iter', default=100, type=int)
parser.add_argument('--ants', default=20, type=int, help='Ants per iteration')
parser.add_argument('-v', action='count', help='Increase verbosity level')
args = parser.parse_args()

random.seed(5012012803)
weights = [[0 for i in range(args.nodes)] for j in range(args.nodes)]
for i in range(args.nodes):
    for j in range(i+1, args.nodes):
        weights[i][j] = random.random() * 100
        weights[j][i] = weights[i][j]
pheromone = [[0.1 for i in range(args.nodes)] for j in range(args.nodes)]
shortest = 1E6

for iteration in range(args.iter):
    ants = [AntPath() for i in range(args.ants)]
    # Set initial node
    for ant in ants:
        ant.last = random.randint(0, args.nodes-1)
        ant.path.append(ant.last)
    # Iterate n-1 times (all except connecting edge)
    for i in range(args.nodes-1):
        for ant in ants:
            ant.next()
    # Final edge and collect results
    for i, ant in enumerate(ants):
        ant.walk(ant.path[0])
        shortest = min(shortest, ant.length)
        # print "Iteration %d: ant %d finished with path %d" % (iteration, i, ant.length)
    print "Shortest path: %d" % shortest
for i in range(args.ants):
    for j in range(args.ants):
        print "%.2f\t" % pheromone[i][j],
    print
