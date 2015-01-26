/********************************************************************************************\
|			                Assignment 1: Love teh Pointer                                   |
|Name: Ryan DePrekel                                                                         |
|Due: 1-29-15                                                                                |
|Course: CS2240                                                                              |
|	   Reading File - [DONE]             													 |
|	   Open file AllCountry.dat                                                              |
|	   Read into 512 byte buffer [CHECK] and parse for desired fields 2,3,8,9. Back          |
|	   file pointer back to \n character and read new buffer.                                |
|																							 |
|	   Parsing Data - [DONE]                                                                 |
|	   Parsing these fields into an array of the struct country.                             |
|	                                                                                         |
|	   Sort Data  & Compute user input  - [NOT DONE]                                         |
|	   Sort                                                                                  |
|	   Allow user to search array of countrys to find based on Country Code.                 |
|	   Using binary search.               													 |
|                                                                                            |
|	   Write to Bin - [NOT DONE]                                                             |
|	   Write data to a binary file. (probably) write to a directory							 |
|	 																					     |
|                                                                                            |
|                                                                                            |
|                                                                                            |
|                                                                                            |
\********************************************************************************************/
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
	char    name[50]; 
	int     pop;
	float   life_e;
}country;

// Function Prototypes 
//country binSearch(char[3]); // binary search that returns country searched for
//char[3]  userInput(); // Allow user to search for data structure
char* GimmeALine(int FileDescrip);
country* parseLine(char* line);
void     printStruct(country** countryStruct, int numCountries);

//Insert file as argument on terminal? !!!Write Prompt for user input!!!
/*
	malloc(size_t ) returns a pointer to free dynamic memory of size given from the system.
	remember to free(void *) memory to return it back to the system.
	realloc(void* , size_t) changes size of pointer from malloc()
*/
int main(int argc, char *argv[])
{
	int infd, outfd, countrySlots = 50, i = 0, numCountries = 0; // file descriptor, Country count, index
	char buff[BUFF];                                                      // buffer var probably not needed
	char* line;                                                           // catches return value of GimmeALine
	country** countries = malloc(50*sizeof(country*)); 
	country  TempCountry;                   //Array of country structs with 50 spots 
	
	infd = open("AllCountries.dat", O_RDONLY);
	if(infd < 0)
	{
		err_sys("failed open!");
	}

	//printf("Outside of loop");
	while((line = GimmeALine(infd)) != NULL){
		//fprintf(stderr, "Line %u \n %s \n", lCount, line);
		//printf("Inside of loop");
		if(numCountries >= countrySlots)
		{
			countrySlots += 50;
			countries = realloc(countries, countrySlots*sizeof(country*));
		}
		//countries[numCountries] = malloc(sizeof(country));
		countries[numCountries] = parseLine(line);
		numCountries++;
	}

	printStruct(countries, numCountries);

	free(countries);
}

// Based off Dr. Trenary's code 
char* GimmeALine(int fd)
{
	static char  buffer[BUFF+1];
	static int   ndxTok;
	static int   ndxBuff = BUFF + 1;
	static int   eof = FALSE;
	static int   lineFound;
	static int   readCount = 0;
	static char* returnLine;
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
	 field 2: country code = code
     field 3: country name = name
     field 8: Population   = pop
     field 9: life expectancy = life_e

     parseLine(char* line) takes a line from the file and parses the fields 
     for tokens desired. This function will assign each field to each respective field in the 
     country. Then will return the country (or pointer to?).
*/
country* parseLine(char* line)
{
	char*   token;
	char*   tokPnt;
	//country returnCountry;
	int     tokenNum = 1;
	country* returnPointer = malloc(sizeof(country));
	token = strtok_r(line, ",", &tokPnt);

	//printf("in parseLine()");

	while(strlen(token)!=0 && tokenNum < 10)
	{
		token = strtok_r(NULL, ",", &tokPnt);
		switch(tokenNum){
			case 1: 
				/*returnCountry.code = */strncpy(returnPointer->code, token, sizeof(returnPointer->code) + 1);
				break;
			case 2: 
				/*returnCountry.name =*/ strncpy(returnPointer->name, token, sizeof(returnPointer->name) + 1);
				break;
			case 7:
				returnPointer->pop = atoi(token);
				break;
			case 8:
				returnPointer->life_e = atof(token);
				break;
		}
		tokenNum++;
	}
	//returnPointer = (*)returnCountry;

	return returnPointer;
}

//Prints Struct with format; Code, Name, Population, Life_e
void printStruct(country** countryStruct, int numCountries)
{
	int i;
	for(i = 0; i < numCountries; i++)
	{
		printf("%c%c%c, %s, %i, %f \n",countryStruct[i]->code[0], countryStruct[i]->code[1], countryStruct[i]->code[2],
			countryStruct[i]->name, countryStruct[i]->pop,countryStruct[i]->life_e);
	}
}

/*
	Compares a country object based on country code
	return -1 if less, 0 if equal, and 1 if greater
*/
void sort()
{

}