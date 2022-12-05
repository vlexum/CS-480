#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "StandardConstants.h"

extern const int NO_ERR;
extern const int STR_EQ;
extern const int SUBSTRING_NOT_FOUND;
extern const char NON_PRINTABLE_CHAR;

// Function Prototypes
int compareString ( const char *oneStr, const char *otherStr );
void concatenateString ( char *destStr, const char *sourceStr );
void copyString ( char *destStr, const char *sourceStr );
int findSubString ( const char *testStr, const char *searchSubStr );
bool getStringConstrained ( FILE *inStream, bool clearLeadingNonPrintable, bool clearLeadingSpace, bool stopAtNonPrintable, char delimeter, char *capturedString );
int getStringLength ( const char *testStr );
bool getStringToDelimiter ( FILE *inStream, char delimeter, char *capturedString );
bool getStringToLineEnd( FILE *inStream, char *capturedString );
void getSubString ( char *destStr, const char *sourceStr, int startIndex, int endIndex );
void setStrToLowerCase ( char *destStr, const char *sourceStr );
char toLowerCase ( char testChar );

#endif  // STRINGUTILS_H
