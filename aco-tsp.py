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
        # pheromone[self.last][node] = (1 - LOCALDECAY) * pheromone[self.last][node] + LOCALDECAY * LOCALUPDATE
        # pheromone[node][self.last] = pheromone[self.last][node]
        self.path.append(node)
        self.last = node


class dotdict(dict):
    def __getattr__(self, attr):
        return self.get(attr, None)
    __setattr__= dict.__setitem__
    __delattr__= dict.__delitem__

def print_phero():
    print "\t",
    for i in range(args.nodes):
        print "%d\t" % i,
    print
    for i in range(args.nodes):
        print "%d\t" % i,
        for j in range(args.nodes):
            print "%.2f\t" % (pheromone[i][j]*100),
        print

parameters = dotdict()

INITIAL_PHEROMONE = parameters.initial_pheromone or 0.1   # Parameter: Initial pheromone trail value
ALPHA             = parameters.alpha or 1                 # Parameter: Likelihood of ants to follow pheromone trails (larger value == more likely)
BETA              = parameters.beta or 5                  # Parameter: Likelihood of ants to choose closer nodes (larger value == more likely)
LOCALDECAY        = parameters.local_decay or 0.2         # Parameter: Governs local trail decay rate [0, 1]
LOCALUPDATE       = parameters.local_update or 0.4        # Parameter: Amount of pheromone to reinforce local trail update by
GLOBALDECAY       = parameters.global_decay or 0.2        # Parameter: Governs global trail decay rate [0, 1]
# If ALPHA = 0, the closest cities are more likely to be selected.
# If BETA = 0, only pheromone amplifications is at work.
# The number of ants will directly determine the speed of the algorithm proportionally. More ants will get more optimal results, but don't use more ants than the number of nodes.
# You need more ants when there are more nodes to have more chances to find a good path quickly. The usual default is numAnts = numNodes.

parser = argparse.ArgumentParser()
parser.add_argument('--nodes', default=20, type=int)
parser.add_argument('--iter', default=1000, type=int)
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

shortest = None
shortest_iter = 0
def shortest_len():
    return sum(weights[shortest[i]][shortest[i+1]] for i in range(len(shortest[:-1])))

# Hard code
# args.nodes = 6
# args.ants = 6
# weights = [
#     [0, 100, 101, 101, 101, 100],
#     [100, 0, 100, 101, 101, 101],
#     [101, 100, 0, 100, 101, 101],
#     [101, 101, 100, 0, 100, 101],
#     [101, 101, 101, 100, 0, 100],
#     [100, 101, 101, 101, 100, 0],
# ]

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
        if not shortest or ant.length < shortest_len():
            shortest = ant.path
            shortest_iter = iteration
        print "Iteration %d: ant %d finished with path %d" % (iteration, i, ant.length)
    # Global reduction
    size = shortest_len()
    for i in range(args.nodes):
        for j in range(i, args.nodes):
            update_amt = (1 - GLOBALDECAY) * pheromone[i][j]
            # print shortest
            diff = abs(shortest[:-1].index(i) - shortest[:-1].index(j))
            if diff == 1 or diff == len(shortest) - 2:
                update_amt += GLOBALDECAY / size
            pheromone[i][j] = update_amt
            pheromone[j][i] = update_amt
    # print shortest
    # print_phero()

    print "Shortest path: %d" % shortest_len()
print "Shortest path reached on iteration %d" % shortest_iter

