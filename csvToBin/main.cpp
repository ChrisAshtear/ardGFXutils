#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>


using namespace std;

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

int main(int argc, char **argv)
{
	streampos size;
	vector<string> splitVals;
	char * memblock;
	unsigned char * outblock;
	int outsize = 0;
	//argv 0-executable name, argv1 first arg, etc.
	
	ifstream file (argv[1]);
	if (file.is_open())
	{
	size = file.tellg();
	memblock = new char [size];
	file.seekg (0, ios::beg);
	file.read ((char*)memblock, size);
	file.close();
	
	splitVals = split(memblock,',');
	cout << memblock;
	delete[] memblock;
	
	}
	
	else cout << "Unable to open file";
	
	ofstream outfile (argv[2], ios::out|ios::binary|ios::trunc);
	if (outfile.is_open())
	{
		for(int i =0; i< splitVals.size();i++)
		{
			int i_hex = atoi (splitVals[i].c_str());
			cout << i_hex;
			outfile.write((char*)i_hex,1);
		}
		outfile.close();
		delete[] outblock;
	}
	
	return 0;
}
