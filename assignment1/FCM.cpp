#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <ctime>       

const string FCM::TEXT_SEPARATOR = "\n////////_________\n";
FCM::FCM(unsigned int order, string srcText) {

	initDict();

	if(order > srcText.size()) {
		cout << "No can do\n";
		return;
	}

	string approximation;			
	unsigned int n = 0;

	float total = 0;
	map<string, float> counters;

	

	// iterate over source text
	for(int i = 0; i < srcText.size(); i++) 
	{
		// first operation: save the previous character
		if(i >= 1  && order != 0) approximation += srcText[i-1];


		if(dict.find(srcText[i]) != dict.end()) 
			srcText[i] = dict[srcText[i]];	
		
		if(i >= order)  
		{
			total++;

			bool found = false;
			if(order == 0) {

				if(lut.size() == 0) {

					map<char, float> values;
					values.insert(make_pair((char)srcText[i], 1));
					firstWord = "key";
					counters.insert(make_pair(firstWord, 1));
					lut.insert(make_pair(firstWord, values));	
				
				}
				else {
					if(lut.begin()->second.find(srcText[i]) != lut.begin()->second.end()) {
						lut.begin()->second[srcText[i]]++;
					} else {
							lut.begin()->second.insert(make_pair(srcText[i], 1));
					}

					counters[firstWord]++;
				}

				

			}
			else {
				if(i == order) firstWord = approximation;

				
				typedef LUT::iterator it_lut;

				for(it_lut it = lut.begin(); it != lut.end(); it++) 
				{
					if(it->first.compare(approximation) == 0)
					{
						if(it->second.find(srcText[i]) != it->second.end()) {
							it->second[srcText[i]]++;
						} else {
							it->second.insert(make_pair(srcText[i], 1));
						}

						found = true;
						break;
					} 

				}

				if(!found) {
					map<char, float> values;
					values.insert(make_pair((char)srcText[i], 1));
					lut.insert(make_pair(approximation, values));
				}	


				counters[approximation]++;	
				// move on
				approximation.erase(0,1);	
			}
		}
	} 

	cout << calcEntropy(lut, counters, total, order) << "\n";


	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		float contextTotal = counters[it->first];

		float accumulated = 0;
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			accumulated += ((it2->second)/contextTotal);
			it2->second = accumulated;
		}

	}


	printLUT();

}		

void FCM::printLUT() {
	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << it->first << ": " << it2->first << ": " << it2->second << "\n";
		}
	}

}

void FCM::loadTable(string fileName) {

}

void FCM::genText(int len, int order) {


	string text((order != 0) ? firstWord : "");
	string approximation(text);
	int i = (order != 0) ? approximation.size()-1 : 0;

	srand(time(NULL));
	while(i < len) {

		float r = ((float) rand() / (RAND_MAX));

		if(order == 0) {
			for(it_map it = lut.begin()->second.begin(); it != lut.begin()->second.end(); it++) {
				if(r <= it->second) {				
					text.push_back(it->first);
					break;
				}

			}

			i++;
			continue;
		}
		else {
			map<char,float> contextMap = lut[approximation];

			
			//cout << r << "\n";
			for(it_map it = contextMap.begin(); it != contextMap.end(); it++) {

				if(r <= it->second) {				
					text.push_back(it->first);
					approximation.push_back(it->first);
					break;
				}
			}


			approximation.erase(0,1);
			i++;
		}
	}	
	
	cout << "$$$$$$$\n";
	cout << text << "\n";
}

float FCM::calcEntropy(LUT l, map<string, float> counters, float total, int order) {

    float totalEntropy = 0;

    if(order == 0) {
    	for(it_map it = l.begin()->second.begin(); it != l.begin()->second.end(); it++) {
    		float prob = it->second/total;
    		totalEntropy += -prob*log2(prob);
    	}

    	return totalEntropy	;
    }
	for(it_lut it = l.begin(); it != l.end(); it++) {
		// context

		float sumLine = counters[it->first];
		float localEntropy = 0;	

		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			// symbols in each context

			float prob = it2->second/sumLine;
			localEntropy += -(prob * log2(prob));
		}

		totalEntropy += (localEntropy * (sumLine/total));
	}


	return totalEntropy;
}