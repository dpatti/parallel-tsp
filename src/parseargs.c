#include "aco.h"

//graph size, #ants, distribution method

void parseargs(int argc, char *argv[]){

	int i = 0;
	for(i = 0; i < argc; ++i){

		if(strcmp(argv[i], "--graph_size") == 0) {
			graph_size = atoi(argv[i+1]);
			++i;
		}
		else if(strcmp(argv[i], "--ants") == 0) {
			ant_count = atoi(argv[i+1]);
			++i;
		}
		else if(strcmp(argv[i], "--dist_method") == 0) {
      if (strcmp(argv[i+1], "round_robin") == 0)
        method = round_robin;
      else if (strcmp(argv[i+1], "distance") == 0)
        method = distance;
      else if (strcmp(argv[i+1], "clustering") == 0)
        method = clustering;
			++i;
		}
		else if(strcmp(argv[i], "--iterations") == 0) {
			iterations = atoi(argv[i+1]);	
			++i;
		}	
    else if(strcmp(argv[i], "-v") == 0) {
      verbose = TRUE;
    }
	}
}

