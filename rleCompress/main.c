#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
	ofstream myfile;
	myfile.open (argv[0]);
	cout << myfile;
	//myfile << "Writing this to a file.\n";
	myfile.close();
	return 0;
}
