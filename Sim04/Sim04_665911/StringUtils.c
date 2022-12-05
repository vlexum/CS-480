// header files
#include "StringUtils.h"

const int NO_ERR = 1;
const int STR_EQ = 0;
const int SUBSTRING_NOT_FOUND = -1;
const char NON_PRINTABLE_CHAR  = (char) 127;

/*
Name: compareString
Process: compares two strings with the following results:
   - if left string less than right, return < 0
   - if left string greater than right, return > 0
   - if left string equal to right, return 0
      - equal test includes length
Function Input/Parameters: c-syle left and right strings
Function Output/Parameters: none
Function Output/Returned: result as specified (int)
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringLength
Notes: none
*/
int compareString ( const char *oneStr, const char *otherStr )
   {
    //initialize function/variables
    int diff, index = 0;

    // loop to end of shortest string,
    // with overun protection
    while ( oneStr[ index ] != NULL_CHAR && otherStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
      {
       // get  difference in characters
       diff = oneStr[ index ] - otherStr[ index ];
       // check for difference between characters
       if ( diff != 0 )
          {
           // return difference
           return diff;
          }
       // increment index
       index++;
      }
    // end loop

    // return difference in lengths, if any
      // function: getStringLength
    return getStringLength( oneStr ) - getStringLength( otherStr );
   }

/*
Name: concatenateString
Process: appends one string onto another
Function Input/Parameters: c-style source string
Function Output/Parameters: c-style destination string
Function Output/Returned: none
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringLength
Notes: none
*/
void concatenateString ( char *destStr, const char *sourceStr )
   {
    // initalize funtion/variables

    // set destination index
       // function: getStringLength
    int destIndex = getStringLength( destStr );

    // get source string length
       // function: getStringLength
    int sourceStrLen = getStringLength( sourceStr );
    // create temp string pointer
    char *tempStr;

    // create other variables
    int sourceIndex = 0;

    // copy source string in case of aliasing
      // function: malloc, copyString
    tempStr = (char *)malloc( sizeof ( sourceStrLen + 1 ) );
    copyString( tempStr, sourceStr );

    // loop to end of source string
    while ( tempStr[ sourceIndex ] != NULL_CHAR && destIndex < MAX_STR_LEN )
      {
       // assign characters to end of destination string
       destStr[ destIndex ] = tempStr[ sourceIndex ];

       // update indices
       destIndex++;
       sourceIndex++;

       // set temp end of destination string
       destStr[ destIndex ] = NULL_CHAR;
      }
    // end loop

    // release memory used for temp string
      // function: free
    free( tempStr );
   }

/*
Name: copyString
Process: copies one string into another
Function Input/Parameters: c-style source string
Function Output/Parameters: c-style destination string
Function Output/Returned: none
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringLength
Notes: none
*/
void copyString ( char *destStr, const char *sourceStr )
   {
    // Initalize Function/Variables
    int index = 0;
    // check for source/destination not the same (aliasing)
    if ( destStr != sourceStr )
      {
       // loop to end of source string
       while ( sourceStr [ index ] != NULL_CHAR && index < MAX_STR_LEN )
         {
          // assign characters to end of destination string
          destStr[ index ] = sourceStr [ index ];
          // update index
          index++;
          // set temp end of destination string
          destStr[ index ] = NULL_CHAR;
         }
      }
      // end loop
   }

/*
Name: findSubString
Process: linear search for given substring within another string
Function Input/Parameters: c-style source test str, c-style source search str
Function Output/Parameters: none
Function Output/Returned: index of found substring, if not found then SUBSTRING_NOT_FOUND constant
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringLength
Notes: none
*/
int findSubString ( const char *testStr, const char *searchSubStr )
   {
    // Initalize Function/Variables

      // initialize test string length
         // function: getStringLength
      int testStrLen = getStringLength( testStr );

      // initialize master index - location of substring starting point
      int masterIndex = 0;

      // initialize other variables
      int searchIndex, internalIndex;

    // loop across test string
    while ( masterIndex < testStrLen )
      {
       // set internal loop index to current test string index
       internalIndex = masterIndex;

       // set set internal search index to zero
       searchIndex = 0;

       // loop to end of test string
       // while test string/sub string characters are the same
       while ( internalIndex <= testStrLen && testStr[ internalIndex ] == searchSubStr[ searchIndex ] )
          {
           // increment test string, substring indices
           internalIndex++;
           searchIndex++;

           // check for end of substring (search completed)
           if ( searchSubStr[ searchIndex ] == NULL_CHAR )
              {
               // return begininning location of substring]
               return masterIndex;
              }
          }
       // end internal comparison loop

       // increment current beginning location index
       masterIndex++;
      }
    // end loop across test string

    // assume test have failed at this point, return SUBSTRING_NOT_FOUND

    return SUBSTRING_NOT_FOUND;
   }

/*
Name: getStringConstrained
Process: captures a string from the input stream with various constraints
Function Input/Parameters: input stream (FILE *),
                           clears leading non printable (bool),
                           clear leading space (bool),
                           stops at non printable (bool),
                           stops at specified delimiter (char)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: fgetc
Notes: consumes delimeter
*/
bool getStringConstrained ( FILE *inStream, bool clearLeadingNonPrintable, bool clearLeadingSpace, bool stopAtNonPrintable, char delimeter, char *capturedString )
   {
    // Initalize Function/Varaibles
    int intChar = EOF, index = 0;

    // initialize output string
    capturedString[ index ] = NULL_CHAR;

    // capture first value in stream
      // function: fgetc
    intChar = fgetc( inStream );

    // loop to clear non printable or space (if indicated)
    while ( ( ( intChar != EOF ) && ( clearLeadingNonPrintable && intChar <= (int) SPACE ) ) || ( clearLeadingSpace && intChar == (int) SPACE ) )
      {
       // get next character
          // function: fgetc
       intChar = fgetc( inStream );
      }
    // end clear non printable/space loop

    // check for end of file found
    if ( intChar == EOF )
      {
       // return failed operation
       return false;
      }
    // loop to capture input
    while (
           // continues if not at end of file and max string length not reached
           ( intChar != EOF && index < MAX_STR_LEN - 1 )
           // AND
           // continues if not printable flag set and characters are printable
           // OR continues if not printable flag not set
           && ( ( stopAtNonPrintable && intChar >= (int) SPACE ) || ( !stopAtNonPrintable ) )
           // AND
           // continues if specified delimiter is not found
           && ( intChar != (char) delimeter )
         )
      {
       // place character in array element
       capturedString[ index ] = (char) intChar;

       // increment array index
       index++;

       // set next element to null character / end of c-string
       capturedString[ index ] = NULL_CHAR;

       // get next character as integer
          // function: fgetc
       intChar = fgetc( inStream );

      }
    // end loop

    // return succesful operation
    return true;
   }

/*
Name: getStringLength
Process: finds the length of a string by counting character up to the NULL_CHAR
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: none
Function Output/Returned: length of string
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies : none
Notes: none
*/
int getStringLength ( const char *testStr )
   {
    // initialize function/varaibles
    int index = 0;

    // loop to end of string, protect from overflow
    while ( index < STD_STR_LEN && testStr[ index ] != NULL_CHAR )
      {
       // update index
       index++;
      }
    // end loop

    // return index(length)
    return index;
   }

/*
Name: getStringToDelimiter
Process: captures a string from the input stream to a specified delimeter
Function Input/Parameters: input stream (FILE *), specified delimeter (char)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringConstrained
Notes: consumes delimeter
*/
bool getStringToDelimiter ( FILE *inStream, char delimeter, char *capturedString )
   {
    // initialize function/variables

    // call engine function with delimeter
      // function: getStringConstrained
    return getStringConstrained (
                                  inStream,
                                  true,
                                  true,
                                  true,
                                  delimeter,
                                  capturedString
                                );
   }

/*
Name: getStringToLineEnd
Process: captures a string from the input stream to end of line
Function Input/Parameters: input stream (FILE *)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringConstrained
Notes: none
*/
bool getStringToLineEnd( FILE *inStream, char *capturedString )
   {
    // initialize function/variables

    // call engine function with delimeter
      // function: getStringConstrained
    return getStringConstrained (
                                  inStream,
                                  true,
                                  true,
                                  true,
                                  NON_PRINTABLE_CHAR,
                                  capturedString
                                );
   }

/*
Name: getSubString
Process: captures sub string within larger string between two inclusive   indicies. Returns empty string if either index is out of range, assumes enough memory in destination string
Function Input/Parameters: c-style source string, start and end indices
Function Output/Parameters: c-style destination string
Function Output/Returned: none
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: getStringLength, malloc, copyString, free
Notes: none
*/
void getSubString ( char *destStr, const char *sourceStr, int startIndex, int endIndex )
   {
    // initialize function/variables

      // set length of source string
         // function: getStringLength
      int sourceStrLen = getStringLength( sourceStr );

      // initialize the destination index to 0
      int destIndex = 0;

      // initialize source index to start index (parameter)
      int sourceIndex = startIndex;

      // create pointer for temp string
      char *tempStr;

    // check for indices within limits
    if ( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
      {
       // create temp string
          // function: malloc, copyString
       tempStr = (char *)malloc( sizeof( sourceStrLen + 1 ) );
       copyString( tempStr, sourceStr);
       // loop across requested substring (indices)
       while ( sourceIndex <= endIndex )
         {
          // assign source character to destination element
          destStr[ destIndex ] = tempStr[ sourceIndex ];

          // increment indices
          destIndex++;
          sourceIndex++;

          // set temp end od destination string
          destStr[ destIndex ] = NULL_CHAR;
         }
       // end loop
      }
      // return memory from temp string
         // function: free
      free( tempStr );
   }

/*
Name: setStrToLowerCase
Process: iterates through string, sets any upper case letter to lower case; no effect on any other letters
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: toLowerCase
Notes: none
*/
void setStrToLowerCase ( char *destStr, const char *sourceStr )
   {
    // intitialize function/variables

      // get source string length
         // function: getStringLength
      int sourceStrLen = getStringLength( sourceStr );

      // create temporary string pointer
      char *tempStr;

      // create other varaiables
      int index = 0;
    // copy source string in case of aliasing
      // function: malloc, copyString
    tempStr = (char *)malloc( sizeof ( sourceStrLen + 1 ) );
    copyString( tempStr, sourceStr );

    // loop across source string
    while ( tempStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
      {
       // set individual character to lower case as needed,
       // assign to destination string
       destStr[ index ] = toLowerCase( tempStr[ index ] );
       // update index
       index++;

       // set temporary end of destination string
       destStr[ index ] = NULL_CHAR;
      }
    // end loop

    // release memory from temp string
      // function: free
    free( tempStr );
   }

/*
Name: toLowerCase
Process: if character is upper case, sets it to lower case
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: character set to lower case if appropriate
Device Input/Keyboard: none
Device Input/Monitor: none
Dependencies: none
Notes: none
*/
char toLowerCase ( char testChar )
   {
    // intialize function/varaibles

    // check for upper case letter
    if ( testChar >= 'A' && testChar <= 'Z')
      {
       // return the lower case letter
       return testChar - 'A' + 'a';
      }

    // otherwise, assume no upper case letter, return character unchanged
    return testChar;
   }
