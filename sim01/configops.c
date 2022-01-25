// header files

/*
Function Name: clearConfigData
Algorithm : frees allocated memory for config metaDataPtr
Precondition: configData holds allocated data set
Postcondition: memory is freed, pointer is set to NULL
Exceptions: none
Notes: none
*/
ConfigDataType *clearConfigData( ConfigDataType *configData )
   {
    // check that config data pointer is not NULL

      //free data structure memory
         // fuction: free

      // set config data to NULL

    //set config data pointer to null ( returned as parameter )

    return NULL;
   }

/*
Function Name: displayConfigData
Algorithm: diagnostic function to shwo config data output
Precondition: configData holds allocated data set
Postcondition: data is printed to screen
Exceptions: none
Notes: none
*/
void displayConfigData( ConfigDataType *configData )
   {
    //initialize function/variables

    // print line of display
      // functions: printf, configCodeToString (translated coded items )

   }

/*
Function Name: getConfigData
Algorithm: opens file, aquires configuration data, returns pointer to data structure
Precondition: for correct operation, file is avaible, is formatted correctly, and has all the configuration lines and data although the configuration line are not required to be in a specific order
Postcondition: in correct operation, returns pointer to correct configuration data structure
Exceptions: correctly and appropriatley (without program failure) responds to and reports file access failure, incorrectly formatted lead or end descriptors, incorrectly formatted prompt, data out of range, and incomplete file conditions
Notes: none
*/
bool getConfigData( char *fileName, ConfigDataType **configData, char *endStateMsg )
   {
    // initialize function/varaibles
      // set constant number of data lines

      // set read only constant

      // create pointer for data input

      //declare other variables

    // set endStateMsg to success
      // function: copyString

    // initialize config data pointer in case of return error

    // open files
      // function: fopen

    // check for file open failure

      // set end state message to config file access error
         // function: copyString

      // return file access error

    // get first line, check for failure
      // function: getLineTo

      // close file access
         // fclose

      // set end state message to corrupt leader line error
         // function: copyString

      // return corrupt file data

    // create temporary pointer to congfiguration data structure

    // loop to end of congfig data items

      // get line leader, check for failure
         // function: getLineTo

         // free temp struct memory
            // function: free

         // close file access
            // function: fclose

         // set end state message to line capture failure
            // function: copyString

         // return incomplete file error

      // find correct data line by number
         // function: getDataLineCode

      // check for data line found

         // get data value

            // check for version number (double type)

               // get version number
                  // function: scanf

            // otherwise, check for metaData or logTo file names
            // or CPU scheduling names (strings)

               // get string input
                  // function: fscanf

            // otherwise, assume integer data

               // get integer input
                  // function: fscanf

            // check for data value in range
               // function: valueInRange

               // assign to data pointer depending on config items
                  // function : getCpuSchedCode, getLogToCode

            // otherwise, assume data value not in range

               // free temp struct memeory
                  // function: free

               // close file access
                  // function: fclose

               // set end state message to configuration data out of range
                  // function: copyString

               // return data out of range

            // otherwise, assume data line not found

               // free temp struct memeory
                  // function: free

               // close file access
                  // function: fclose

               // set end state message to configuration corrupt prompt error
                  // function: copyString

               // return corrupt config file code

            // increment line counter

    // end master loop

    // aquire end of sim config string
      // function: getLineTo

      // free temp struct memeory
         // function: free

      // close file access
         // function: fclose

      // set end state message to corrupt configuration end line
         // function: copyString

      // return corrupt file data

    // assign temporary pointer to parameter return pointer

    // close file access
      // function: fclose

    // return no error code

    return false;
   }

/*
Function Name: configCodeToString
Algorithm: utility function to support display of CPU scheduling or Log to code strings
Precondition: code variable holds constant value from ConfigDataCodes for item (e.g., CPU_SCHED_SRTF_P, LOGTO_FILE_CODE, etc. )
Postcondition: string parameter holds correct string associated with the given constant
Exceptions: none
Notes: none
*/
void configCodeToString( int code, char *outstring )
   {
    // return appropriate code depending on prompt string provided
      // function: compareString

   }

/*
Function Name: getDataLineCode
Algorithm: test string for one of known leader strings, returns line number if string is correct, returns CFG_CORRUPT_DATA_ERR if string not found
Precondition: dataBuffer is valid C-Style string
Postcondition: returns line number of data item in terms of a constant (e.g., CFG_VERSION_CODE, CFG_CPU_SCHED_CODE, etc.
Exceptions: returns CFG_CORRUPT_FILE_ERR if string is not identified
Notes: none
*/
int getDataLineCode( char *dataBuffer )
   {
    // return appropriate code depending on prompt string provided
      // function: compareString
    return 0;
   }

/*
Function Name: valueInRange
Algorithm: tests one of three values (int, double, string) for being in specified range, depending on data code
Precondition: one of the three data values is valid
Postcondition: returns true of data is within specified parameters, false otherwise
Exceptions: metadata or logfile names are ingnored and return true
Notes: none
*/
bool valueInRange( int lineCode, int intVal, double doubleVal, char *stringVal )
   {
    // initialize function/variables

    // use line code to identify prompt line

      // for version code

         // check if limits of version code are exceeded

            // set bool result to false

      // for cpu scheduling code
         // create temp string and set to lower case
            // function: getStringLength, malloc, setStrToLowerCase

         // check for not finding one of scheduling strings
            // function: compareString

            // set bool result to false

         // free temp string memory
            // function: free

      // for quantum cycles

         // check for quantum cycles limits exceeded

            // set bool result to false

      // for memory available

         // set bool result to false

      //check for process cycles

         // check for progress cycles limit exceeded

            // set bool result to false

      // check for I/O cycles

         // check for I/O cycles limits exceeded

            // set bool result to false

      // check for log to operations

         // create temp string and set to lower case
            // function: getStringLength, malloc, setStrToLowerCase

         // check for not finding one of the log to strings
            // function : compareString

            // set bool result to false

            // free temp string memory
               // function: free

    // return results of limits analysis

    return false;
   }

/*
Function Name: getCpuSchedCode
Algorithm: converts string data to constant number to be stored as an int
Precondition: codeStr is a C-Style string with one of the specified cpu scheduling operations
Postcondition: returns code representing scheduling actions
Exceptions: default to fcfs code
Notes: none
*/
ConfigDataCodes getCpuSchedCode( char *codeStr )
   {
    // initialize function/varaibles

      // set up temp string for testing
         // function: getStringLength, malloc

      // set default return ro FCFS-N

    // set string to lower case for testing
      // function: setStrToLowerCase

    // check for SJF-N
      // function: compareString

      // set return code to sjf

    // check for SRTF-P
      // function: compareString

      // set return code to SRTF

    // check for FCFS-P
      // function: compareString

      // set return code to fcfs

    // check for RR-P
      // function: compareString

      // set return code to rr

    // free temp string memory
      // function: free

    // return resulting value

    return 0;
   }

/*
Function Name: getLogToCode
Algorithm: converts string data to constant number to be stored as an int
Precondition: codeStr is a C-Style string with one of the specified LogTo operations
Postcondition: returns code representing log to actions
Exceptions: defaults to monitor code
Notes: none
*/
ConfigDataCodes getLogToCode( char *logToStr )
   {
    // initialize function/variables

      // create temp string
         // function: getStringLength, malloc

      // set default to log to monitor

    // set temp string to lower case
      // function: setStrToLowerCase

    // check for BOTH
      // function: compareString

      // set return value to both code

    // check for file
      // function: compareString

      // set return value to file code

    // free temp string memory
      // function: free

   // return found code

    return 0;
   }
