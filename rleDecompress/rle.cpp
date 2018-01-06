#include "types.h"
#include <stdio.h>
#include <iostream>
using namespace std;

int RLE_Uncompress( unsigned char *in, RLE_data *out,
    unsigned int insize )
{
    unsigned char marker, symbol;
    unsigned int  i, inpos, outpos, count;

    /* Do we have anything to uncompress? */
    if( insize < 1 )
    {
        return 0;
    }

    /* Get marker symbol from input stream */
    inpos = 0;
    marker = in[ inpos ++ ];

	cout <<"\nBeginning Decompression\n";
    /* Main decompression loop */
    outpos = 0;
	int curPos = 0;
    do
    {
        symbol = in[ inpos ++ ];
		//cout << "symbol: " << symbol;
        if( symbol == marker )
        {
            /* We had a marker byte */
            count = in[ inpos ++ ];
            if( count <= 2 )
            {
                /* Counts 0, 1 and 2 are used for marker byte repetition
                   only */
				curPos= outpos++;
				out[ curPos ].colorIdx = marker;
				out[ curPos ].rLength = count;
            }
            else
            {
                curPos = outpos++;
				
				symbol = in[inpos++];
				//cout<< "repeating symbol:" << symbol;
				out[ curPos ].colorIdx = symbol;
				out[ curPos ].rLength = count;
            }
        }
        else
        {
            /* No marker, plain copy */
			curPos = outpos++;
            out[ curPos ].colorIdx = symbol;
			out[ curPos ].rLength = 1;
        }
    }
    while( inpos < insize );
	return outpos;
}