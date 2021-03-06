#include <iostream>
#include "Predictor.h"

int main(int argc, char** argv) {

	if(argc < 3) {
		fprintf(stderr, "Usage: <rgb file> <dst encoded file> <M>\n");
		return -1; 
	}
	Predictor* p = new Predictor(argv[2], atoi(argv[3]), 0);
	p->spatialPredict(argv[1]);
		
	return 0;
}
