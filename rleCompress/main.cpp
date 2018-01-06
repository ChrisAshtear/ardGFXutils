#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
	streampos size;
	unsigned char * memblock;
	unsigned char * outblock;
	int outsize = 0;
	//argv 0-executable name, argv1 first arg, etc.
	
	ifstream file (argv[1], ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
	size = file.tellg();
	memblock = new unsigned char [size];
	file.seekg (0, ios::beg);
	file.read ((char*)memblock, size);
	file.close();

	cout << "the entire file content is in memory\n";
	cout << "size = " << size << "\n";
	
	int outBufferSize = (size * 1.04) + 1;
	
	outblock = new unsigned char [outBufferSize];	
	outsize = RLE_Compress( memblock, outblock, size);
	
	cout << "\nfinished compression\n";
	cout << "new size = " << outsize << "\n";
	
	delete[] memblock;
	
	}
	
	else cout << "Unable to open file";
	
	ofstream outfile (argv[2], ios::out|ios::binary|ios::trunc);
	if (outfile.is_open())
	{
		outfile.write((char*)outblock,outsize);
		outfile.close();
		delete[] outblock;
	}
	
	
  return 0;
}
