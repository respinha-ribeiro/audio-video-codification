#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>
#include <queue>
#include <list>

#include "Golomb.h"


Golomb::Golomb(int m, string encodedFilename, int pos) : M(m), B(log2(m)){
	stream = new BitStream(encodedFilename,pos);
}


void Golomb::encode(short n, short finalWrite) {

		
	int transformedN = n >= 0 ? 2*n : (2*abs(n))-1;

	int q = transformedN/M;
	int r = transformedN -(q*M);

/*	cout << "N = " << n << "\n";
	cout << "Transformed N: " << transformedN << "\n";
	cout << "M = " << M << "\n";
	cout << "Q = " << q << "\n";
	cout << "R = " << r << "\n";
	cout << "B = " << B << "\n";
	cout << "###########################\n";*/

	int i = 0;

	// unary
	while(i < q) {
		stream->writeBit(1);
		i++;
	}

	stream->writeBit(0);

	stream->writeNBits(B, r, finalWrite);
}

short Golomb::decode(int* end) {

	int nUnary = 0;
	int isUnary = 1;
	int q = 0;

	
	short sample;
	
	while(1) {
		
		if(isUnary) {
			int bit = stream->readBit();

			if(bit == -1) {
				*end = 1;
				cout << "The end as we know it\n";
			
				return -1;
			}
			
			if(bit) q++;			
			else isUnary = bit;		
		} else {
					int r = stream->readNBits(B);
			int n = r + (q*M);
			int original;

			// checking if even or odd to apply transformation
			if(n%2 == 0)
				original = n/2;
			else
				original = ((n+1)/2) * -1;
			
			cout << "decoded: " << n << "; Original: " << original << "\n";

			stringstream ss;
			ss << original;			
			ss >> sample;

			return sample;
		}
		
	}

	return sample;
	
}

int Golomb::getFilePosition() {
	return stream->getFilePosition();
}
