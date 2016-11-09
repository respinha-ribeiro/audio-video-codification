#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstdlib>

#include "BitStream.h"

BitStream::BitStream(string fname, int pos) {

		// read file	
		readPosition = 0;
		writePosition = pos;
		filename = new string(fname);
		surplusByte = 0;
		remainingByteSlots = 0;
		surplus = 0;
}

int BitStream::readBit () {

		//cout << "POS: " << readPosition << " " << writePosition << "\n";	
		if(readPosition >= writePosition){
			//cout << "FINISH: " << readPosition << "; " << writePosition << "\n";
			 return -1;
		}
		
		ifstream* stream = new ifstream(filename->c_str(), ifstream::binary);
		
		int bit = 0;
		if (stream->is_open()) {

				// get length of file
				stream->seekg(readPosition/8, stream->beg);

				char byteBuffer;

				// read data as a block
				stream->read (&byteBuffer,1);

				stream->close();

				// bitwise
				bit = (byteBuffer >> (7-readPosition%8)) & 0x1;
				readPosition++;	

		}

		//cout << bit;	
		return bit;
}

int* BitStream::readNBits(int nBits) {

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;


	ifstream* stream = new ifstream(filename->c_str());
	int* bitBuffer = new int[nBits];

	if (stream->is_open()) {
		// get length of file:
		stream->seekg (readPosition/8, stream->beg);

		char* bytesBuffer;		// read data as a block

		int readUntilNow = 0;

		if(readPosition%8 != 0) {
			bytesToRead++;
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

			int pos = readPosition%8;
			while(pos < 8) {
				int bit = (bytesBuffer[0] >> (7-pos++)) & 0x1;
				//cout << bit;

				bitBuffer[readUntilNow] = bit;			
				if(++readUntilNow == nBits) {	
					// exit
					readPosition += nBits;
					return bitBuffer;
				}
			}

			//cout << "\n";

	
		} 
		else {
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

		}


		// bytwise reading
		// if we already started reading bits, then skip to next byte
		for(int i = (readUntilNow != 0); i < bytesToRead; i++) {
			// bitwise reading

			for(int j = 0; j < 8; j++) {
				int bit = (bytesBuffer[i] >> (7-j)) & 0x1;
				// cout << bit;

				bitBuffer[readUntilNow] = bit;
				
				if(++readUntilNow == nBits) {	

				// exit
				i = bytesToRead;
				break;
	
				}
			}

			//cout << "\n";
		}

		delete[] bytesBuffer;
		
		readPosition += nBits;	// update bit position on file
	}

	return bitBuffer;
}

void BitStream::writeBit(int bit) {

	cout << "SOLO: Remaining bits: " << remainingByteSlots << "\n";

	if(remainingByteSlots) {
		int pos = 8 - remainingByteSlots;

		surplusByte = surplusByte | (bit << (7-pos));
		surplus = 1;
		remainingByteSlots--;

		if(!remainingByteSlots) {

			ofstream* stream = new ofstream();
			stream->open(filename->c_str(), ios::out | ios::binary | ios::app);		
			for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";

			stream->seekp(writePosition/8, stream->beg);
			stream->write(&surplusByte, sizeof(surplusByte));

			stream->close();

			writePosition += 8;
			cout << "solo update write pos " << writePosition << "\n";

			surplusByte = 0;
			surplus = 0;
		}
	} else {	
		surplusByte = surplusByte | (bit << 7);
		remainingByteSlots = 7;
	}
		/*for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";*/
}

void BitStream::writeNBits(int nBits, int bit_buff, int finalWrite) {


	ofstream* stream = new ofstream();
	const char* fname = filename->c_str();

	// tmp char to be filled with bits
	char buffer = 0;
	// tmp char bitwise position
	int bufferPos = 0;
	int j = 0;

	// flag to tell if writing surplus byte should be aborted
	int abort = 0;

	stream->open(fname, ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	if(surplus) {

		// position in surplus byte of file
		int pos = 8 - remainingByteSlots;


		while(pos < 8) {	
			cout << "remaining bits... " << remainingByteSlots << " and j = " << j << "\n";
			int bit = (bit_buff >> (nBits-(j+1))) & 0x1;
			surplusByte = surplusByte | (bit << (7-pos));


			remainingByteSlots--;
			if(j == nBits-1 && pos < 7) {
				cout << "Abortion\n";
				for(int k = 0; k < 8; k++) 
					cout << ((surplusByte >> (7-k)) & 0x1);
				cout << "\n";
				cout << "there arre still " << remainingByteSlots << " bits\n";
				j = nBits;
				abort = 1;
				break;
			}
			
			j++;
			pos++;
		}
		
		
		if(!abort) {

			for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";

			stream->write(&surplusByte,sizeof(surplusByte));		
			writePosition += 8;

			cout << "Update write pos " << writePosition << "\n";
			surplusByte = 0;
		}
	}

	for(int i = j; i < nBits; i++) {
		int bit = (bit_buff >> ((nBits-(i+1)))) & 0x1;
		
		buffer = buffer | (bit << (7-bufferPos));
			
		if(++bufferPos == 8) {
			stream->write(&buffer,sizeof(buffer));

			for(int k = 0; k < 8; k++) 
				cout << ((buffer >> (7-k)) & 0x1);
			cout << "\n";

			buffer = 0;
			bufferPos = 0;
			writePosition += 8;

			cout << "update write pos: " << writePosition << "\n";
		}
	}

	stream->close();
	if(finalWrite) {
		if(!abort)
			surplusByte = buffer;

		remainingByteSlots = (8-bufferPos);
		flush();

		return;
	}

	if(bufferPos != 0 || remainingByteSlots != 0) {
		if(!remainingByteSlots) 
			surplusByte = buffer;
		if(bufferPos != 0)
			remainingByteSlots = (8 - bufferPos);

		surplus = 1;
	} else {
		surplusByte = 0;
		remainingByteSlots = 0;
		surplus = 0;
	}

}

void BitStream::flush() {

	ofstream* stream = new ofstream(filename->c_str(), ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	cout << "flush\n";
		for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";

	stream->write(&surplusByte, sizeof(surplusByte));

	writePosition+=remainingByteSlots;

	cout << "update write pos " << writePosition << "\n";
	stream->close();

}

int BitStream::getFilePosition() {
	return writePosition;
}

