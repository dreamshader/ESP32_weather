#include <stdio.h>
#include <sys/types.h>
#include <time.h>

int decodeBits( unsigned char byte2decode, int width2decode )
{
    int shiftcount, shiftwidth;

    shiftwidth = width2decode >= 8 ? 8 : width2decode ;

    printf("\n");
    for( shiftcount = 0; shiftcount < shiftwidth; shiftcount++ )
    {
        if( (byte2decode >> shiftcount) & 0x01 )
        {
            printf("X");
        }
        else
        {
            printf("0");
        }
    }
    printf("\n");
    return( shiftwidth );
}


int main()
{
    FILE *dataFile;
    time_t timeStamp;
    short   pressure;
    unsigned char humidity;
    unsigned char temp;
    unsigned char recType;
    unsigned char sensId;

    unsigned char myByte = 0b10101010;
    printf("Value = %02x\n", myByte);

    printf("Value = %02x = 0b10101010\n", myByte);
    printf("decode = %d\n", decodeBits( myByte, 4 ));

    if( (dataFile = fopen("bin-data.bin", "w+")) != NULL )
    {
        timeStamp = time(NULL);

        recType = 'P';
        printf("Write %ld bytes\n", sizeof(recType) );
        fwrite(&recType, sizeof(recType), 1, dataFile);

	sensId = 1;
        printf("Write %ld bytes\n", sizeof(sensId) );
        fwrite(&sensId, sizeof(sensId), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(timeStamp) );
        fwrite(&timeStamp, sizeof(timeStamp), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(pressure) );
        fwrite(&pressure, sizeof(pressure), 1, dataFile);
	//
	//
	//
        recType = 'H';
        printf("Write %ld bytes\n", sizeof(recType) );
        fwrite(&recType, sizeof(recType), 1, dataFile);

	sensId = 2;
        printf("Write %ld bytes\n", sizeof(sensId) );
        fwrite(&sensId, sizeof(sensId), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(timeStamp) );
        fwrite(&timeStamp, sizeof(timeStamp), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(humidity) );
        fwrite(&humidity, sizeof(humidity), 1, dataFile);
	//
	//
	//
        recType = 'T';
        printf("Write %ld bytes\n", sizeof(recType) );
        fwrite(&recType, sizeof(recType), 1, dataFile);

	sensId = 3;
        printf("Write %ld bytes\n", sizeof(sensId) );
        fwrite(&sensId, sizeof(sensId), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(timeStamp) );
        fwrite(&timeStamp, sizeof(timeStamp), 1, dataFile);

        printf("Write %ld bytes\n", sizeof(temp) );
        fwrite(&temp, sizeof(temp), 1, dataFile);

        fclose(dataFile);
    }


    return(0);
}
