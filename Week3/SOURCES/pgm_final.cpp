#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <math.h>

using namespace std;

#define N_samples 832*513+1

int main ()
{
    double a[N_samples], minVal, maxVal;
    string samplePoint = "";
    int count = 0;

    // Read all values.
    ifstream infile("Question 2a.txt");
    while(!infile.eof()) {
        getline(infile, samplePoint);
        istringstream iss(samplePoint);
        iss >> a[count++];
    }


    // Find the minimum and maximum values.
    minVal = maxVal = a[0];
    for(int i = 1; i < count; ++i) {
        maxVal = max(maxVal, a[i]);
        minVal = min(minVal, a[i]);
    }

    // Write the normalized values to the output pgm file.
    ofstream outfile("image.pgm");
    outfile << "P2\n";
    outfile << "832 513 255\n";

    for(int k = 512; k >= 0 ; k--)
    {
        for(int i = 0; i < count-1; i = i + 513)
        {
            int normalized = 255 - round((a[k+i] - minVal) * 255/ (maxVal - minVal));
            outfile << normalized<<"\t";
        }
        outfile << "\n";
    }


    infile.close();
    outfile.close();

    return 0;
}


