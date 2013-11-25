#include <stdio.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <fstream>


int main()
{
	double y[513], c[513];
	int count = 0;
	std::ifstream infile ("370_spec.txt");

	// Read input spectra file.
	std::string value;
    while(!infile.eof()){
	    getline(infile, value);
        std::istringstream iss(value);
		iss >> y[count++];
    }

    // Calculate the cepstrum coefficients.
    for (int m = 0; m < 513; ++m) {
    	c[m] = 0;
    	for(int i = 0; i < 513; ++i) {
    		c[m] += cos(M_PI * m * (i + 0.5) / 513.0) * y[i];
    	}
    }
 
  	// Write to output file for plotting.
 	std::ofstream outfile("370_out.txt");
	for (int m = 0; m < 513; ++m) {
		outfile << c[m];
    	outfile << "\n";
	}

	infile.close();
	outfile.close();

	return 0;
}