// todo
#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <stdio.h>

const string FCM::TEXT_SEPARATOR = "\n////////_________\n";
FCM::FCM(unsigned int order, string srcText) {

	

	if(order > srcText.size()) {
		cout << "No can do\n";
		return;
	}


	// build LUT
	string approximation;			
	unsigned int n = 0;

	map<string, float> counters;

	// iterate over source text
	for(int i = 0; i < srcText.size(); i++) 
	{
		// first operation: save the previous character

		if(i >= 1) 
			approximation += srcText[i-1];

		if(order == 0) 
		{	
			// zero-order -> no context                                                              		
			// only alphabetical unidimensional array is needed

			if(!isspace(srcText[i])) 
			{
				int alphabetPosition = srcText[i] - 'a' + 1;
				alphabetArray[alphabetPosition]++;				
			} else // whitespace
				alphabetArray[0]++;

		} else if(i >= order)  
		{

			bool found = false;
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

	



	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		float total = counters[it->first];

		float accumulated = 0;
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			accumulated += ((it2->second)/total);
			it2->second = accumulated;
		}

	}

	

	saveTable();
	//loadTable("saved_LUT");
}		

void FCM::saveTable() {


	fstream myfile("saved_LUT", ios::out | ios::binary);

	string output;
	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		string context = it->first;
		myfile.write(reinterpret_cast<const char*>( &context ), sizeof(string));
		char n = '\n';
		myfile.write(reinterpret_cast<const char*>(&n), sizeof(char));
		
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			char c = it2->first;
			myfile.write(reinterpret_cast<const char*>( &c ),sizeof(char));
			float f = it2->second;
			myfile.write(reinterpret_cast<const char*>(&f),sizeof(float));

			myfile.write(reinterpret_cast<const char*>( &FCM::TEXT_SEPARATOR ), sizeof(string));	
		}
	}

	myfile.close();
}

void FCM::loadTable(string fileName) {

	/* */
}