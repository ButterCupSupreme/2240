/********************************************************|
|			Assignment 1: Love teh Pointer               |
|Name: Ryan DePrekel                                     |
|Due: 1-29-15                                            |
|Course: CS2240                                          |
|                                                        |
/********************************************************/

#include "apue.h" // Includes most standard libraries 
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>	
#include <string.h>

#define BUFF 512
#define FALSE 0
#define TRUE 1
/*
     Struct to define fields that are desired from file
     field 2: country code = code
     field 3: country name = name
     field 8: Population   = pop
     field 9: life expectancy = life_e
*/
typedef struct
{
	char    code[3];
	char*   name;
	int     pop;
	float   life_e;
}country;

// Function Prototypes 
//void openFile();
//void fillCountry(char** country);
//country binSearch(char[3]); // binary search that returns country searched for
//char[3]  userInput(); // Allow user to search for data structure
char * GimmeALine(int FileDescrip);
country parseLine(char* line);

//Insert file as argument on terminal? 
int main(int argc, char *argv[])
{
	int infd, outfd, num = 0, countrySlots = 50, i = 0, lCount = 0; // file descriptor, Country count, index
	char buff[BUFF];
	char *line;
	country** countries = malloc(50*sizeof(country*));

	infd = open("AllCountries.dat", O_RDONLY);
	if(infd < 0)
	{
		err_sys("failed open!");
	}

	while((line = GimmeALine(infd)) != NULL){
		fprintf(stderr, "Line %u \n %s \n", lCount, line);
		lCount++;
	}
	/* 
	   Reading File - [DONE]
	   Open file AllCountry.dat 
	   Read into 512 byte buffer [CHECK] and parse for desired fields 2,3,8,9. Back 
	   file pointer back to \n character and read new buffer. 

	   Parsing Data - [NOT DONE]
	   Parsing these fields into an array of the struct country. 
	   
	   Sort Data  & Compute user input  - [NOT DONE]
	   Sort
	   Allow user to search array of countrys to find based on Country Code. 
	   Using binary search. 

	   Write to Bin - [NOT DONE]
	   Write data to a binary file. (probably) write to a directory
	*/
}

// Based off Dr. Trenary's code 
char * GimmeALine(int fd)
{
	static char  buffer[BUFF+1];
	static int   ndxTok;
	static int   ndxBuff = BUFF + 1;
	static int   eof = FALSE;
	static int   lineFound;
	static int   readCount = 0;
	static char *returnLine;
	off_t offset;

	if(ndxBuff >= readCount)
	{
		readCount = read(fd, buffer, BUFF);
		ndxBuff = 0;
		eof = (readCount == 0);
	}

	if(readCount < 0)
		err_sys("Read less than 0\n");

	if(!eof)//There are lines availble to read
	{
		ndxTok = ndxBuff;
		lineFound = FALSE;

		do{ //Find Line
			while((ndxBuff < readCount) && (buffer[ndxBuff] != '\n'))
				ndxBuff++;
			lineFound = (ndxBuff < readCount);

			if(!lineFound){
				offset = (off_t)(ndxTok - ndxBuff);
				lseek(fd, offset, SEEK_CUR);
				readCount = read(fd, buffer, BUFF);
				ndxBuff = 0; ndxTok = 0; //reset buffer & token indexes 
				eof = (readCount == 0);
				if(eof)
					break;
			}
		} while(! lineFound);//find line in filled buffer 
		}
    if (eof)
        return returnLine = NULL;
    else
    {
        returnLine = &buffer[ndxTok];
        buffer[ndxBuff] = (char) 0; // create a cString
        ndxBuff++; //prep for next token
    }
    return returnLine;
}


/*
	


*/
country parseLine(char* line)
{
	printf("This function is empty GCC give me a break");

}

