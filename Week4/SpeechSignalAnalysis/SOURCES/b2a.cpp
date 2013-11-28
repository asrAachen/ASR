#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {

if (argc != 3) { 
	printf("./b2a <binary file> <output file> \n"); 
	return EXIT_FAILURE; 
}

ifstream inFile;
ofstream outFile;

inFile.open (argv[1], ios::binary);
outFile.open (argv[2]);

float value;
int count = 0;
while (!inFile.eof()){
	inFile.read((char*)(&value), sizeof(value));
	outFile << value << " ";
	count++;
	if (count == 12) {
		outFile << "\n";
		count = 0;
	}
}

inFile.close(); 
outFile.close();
return 0;
}