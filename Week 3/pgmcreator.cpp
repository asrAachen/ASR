#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <math.h>


#define N_samples 832*513+1
using namespace std;

int main ()
{
	double a[N_samples];
    string STRING = "";
	ifstream infile;
	int count =0;
	double MAX, MIN;


	infile.open ("Question 2a.txt");
    while(!infile.eof()) // To get you all the lines.
    {
	    getline(infile,STRING); // Saves the line in STRING.
	    istringstream iss(STRING);
		string sub;
		iss >> sub;
		a[count++] = atof(sub.c_str());
    }

	
	MAX = max(a[0],a[1]);
	MIN = min(a[0],a[1]);
	for(int i=2; i<count-1; i++)
	{
		if(a[i]>-2.5){
		MAX = max(MAX,a[i]);
		MIN = min(MIN,a[i]);
		}
	}

	cout<<MAX;
	cout<<MIN;
	cout<<count;
	
	ofstream myfile;
	myfile.open("image.pgm");
	myfile<<"P2\n";
	myfile<<"832 513 255\n";

	int b;
	for(int k=512; k>=0 ;k--)
	{
		for(int i=0;i<count-1;i=i+513)
		{
		b=255-round((a[k+i]-MIN)*255/(MAX-MIN));
		if(b>=0 && b<=255){
		myfile<<b<<"\t";
		}
		else
		{
			myfile<<"255"<<"\t";
		}
		}
		myfile<<"\n";
	}


	infile.close();
	myfile.close();
}


