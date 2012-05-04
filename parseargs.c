#include "aco.h"

//graph size, #ants, distribution method

void parseargs(int argc, char *argv[]){

	int i = 0;
	for(i = 0; i < argc; ++i){

		if(strcmp(argv[i], "--graph_size") == 0){
			graph_size = atoi(argv[i+1]);
			++i;
		}
		else if(strcmp(argv[i], "--ants") == 0){
			ant_count = atoi(argv[i+1]);
			++i;
		}
		else if(strcmp(argv[i], "--dist_method") == 0){
			
			++i;
		}
	}
}
