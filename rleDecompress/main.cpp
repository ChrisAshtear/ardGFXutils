#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>

void exportCode(char **argv, RLE_data *outblock,int outsize);

using namespace std;
#include "types.h"
int main(int argc, char **argv)
{
	streampos size;
	unsigned char * memblock;
	RLE_data * outblock;
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
	
	int outBufferSize = (size);
	
	outblock = new RLE_data [outBufferSize];	
	outsize = RLE_Uncompress( memblock, outblock, size);
	cout << "\nfinished compression\n";
	cout << "array size = " << outsize << "\n";
	//cout << outblock[0];
	delete[] memblock;
	
	}
	
	else cout << "Unable to open file";

	exportCode(argv,outblock,outsize);
	
	delete[] outblock;
	
	
  return 0;
}

void exportCode(char **argv, RLE_data *outblock,int outsize)
{
	ofstream outfile (argv[2], ios::out|ios::trunc);
	if (outfile.is_open())
	{
		//outfile.write((char*)outblock,outsize);
		outfile << "PROGMEM " ;
		outfile << argv[2] << "[" << outsize << "] = {{";
		outfile << (int)outblock[0].colorIdx << ", " << outblock[0].rLength << "}";
		for(int i=1;i< outsize;i++)
		{
			outfile <<",{";
			outfile << (int)outblock[i].colorIdx << ", " << outblock[i].rLength << "}";
		}
		outfile << "};";
		outfile.close();
		delete[] outblock;
	}
}