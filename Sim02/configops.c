// header files
#include "configops.h"

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
    if ( configData != NULL )
      {
       //free data structure memory
          // fuction: free
       free( configData );
       // set config data to NULL
       configData = NULL;
      }
    //set config data pointer to null ( returned as parameter )
    return NULL;
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
    // define array with eight items, and short(10) lengths
    char displayStrings[ 8 ][ 10 ] = {"SJF-N", "SRTF-P", "FCFS-P", "RR-P", "FCFS-N", "Monitor", "File", "Both"};
    // copy string to return parameter
      // function: copyString
    copyString( outstring, displayStrings[ code ] );
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
    char displayString[ STD_STR_LEN ];
    // print line of display
      // functions: printf, configCodeToString (translated coded items )
    printf( "Config File Display\n" );
    printf( "-------------------\n" );
    printf( "Version                : %3.2f\n", configData->version );
    printf( "Program file name      : %s\n", configData->metaDataFileName );
    configCodeToString( configData->cpuSchedCode, displayString );
    printf( "CPU schedule selection : %s\n", displayString );
    printf( "Quantum time           : %d\n", configData->quantumCycles);
    printf( "Memory Available       : %d\n", configData->memAvailable );
    printf( "Process cycle rate     : %d\n", configData->procCycleRate );
    printf( "I/O cycle rate         : %d\n", configData->ioCycleRate);
    configCodeToString( configData->logToCode, displayString );
    printf( "Log to selection       : %s\n", displayString );
    printf( "Log file name          : %s\n", configData->logToFileName );
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
    const int NUM_DATA_LINES = 9;

      // set read only constant
    const char READ_ONLY_FLAG[] = "r";

      // create pointer for data input
    ConfigDataType *tempData;

      //declare other variables
    FILE *fileAccessPtr;
    char dataBuffer[ MAX_STR_LEN ];
    int intData, dataLineCode, lineCtr = 0;
    double doubleData;

    // set endStateMsg to success
      // function: copyString
    copyString( endStateMsg, "Configuration file upload successful" );

    // initialize config data pointer in case of return error
    *configData = NULL;

    // open files
      // function: fopen
   fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    // check for file open failure
    if ( fileAccessPtr == NULL )
      {
       // set end state message to config file access error
          // function: copyString
       copyString( endStateMsg, "Configuration file access error" );
       // return file access error
       return false;
      }
    // get first line, check for failure
      // function: getLineTo
    if ( getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) != NO_ERR || compareString( dataBuffer, "Start Simulator Configuration File") != STR_EQ  )
      {
       // close file access
          // fclose
       fclose ( fileAccessPtr );

       // set end state message to corrupt leader line error
          // function: copyString
       copyString( endStateMsg, "Corrupt configuration leader line error" );

       // return corrupt file data
       return false;
      }

    // create temporary pointer to congfiguration data structure
    tempData = (ConfigDataType *)malloc( sizeof( ConfigDataType ) );

    // loop to end of congfig data items
    while ( lineCtr < NUM_DATA_LINES )
      {
       // get line leader, check for failure
          // function: getLineTo
       if ( getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) != NO_ERR )
         {
          // free temp struct memory
             // function: free
          free ( tempData );

          // close file access
             // function: fclose
          fclose( fileAccessPtr );

          // set end state message to line capture failure
             // function: copyString
          copyString( endStateMsg, "Corrupt configuration start line capture error" );

          // return incomplete file error
          return false;
         }
       // find correct data line by number
          // function: getDataLineCode
        dataLineCode = getDataLineCode( dataBuffer );
       // check for data line found
       if ( dataLineCode != CFG_CORRUPT_PROMPT_ERR )
         {
          // get data value

             // check for version number (double type)
             if ( dataLineCode == CFG_VERSION_CODE )
               {
                // get version number
                   // function: scanf
                fscanf( fileAccessPtr, "%lf", &doubleData );
               }
             // otherwise, check for metaData or logTo file names
             // or CPU scheduling names (strings)
             else if ( dataLineCode == CFG_MD_FILE_NAME_CODE || dataLineCode == CFG_LOG_FILE_NAME_CODE || dataLineCode == CFG_CPU_SCHED_CODE || dataLineCode == CFG_LOG_TO_CODE )
               {
                // get string input
                   // function: fscanf
                fscanf( fileAccessPtr, "%s", dataBuffer );
               }
             // otherwise, assume integer data
             else
               {
                // get integer input
                   // function: fscanf
                fscanf( fileAccessPtr, "%d", &intData );
               }
             // check for data value in range
                // function: valueInRange
             if( valueInRange( dataLineCode, intData, doubleData, dataBuffer ) )
               {
                // assign to data pointer depending on config items
                   // function : getCpuSchedCode, getLogToCode
                switch ( dataLineCode )
                  {
                   case CFG_VERSION_CODE:
                     tempData->version = doubleData;
                     break;

                   case CFG_MD_FILE_NAME_CODE:
                     copyString ( tempData->metaDataFileName, dataBuffer );
                     break;

                   case CFG_CPU_SCHED_CODE:
                     tempData->cpuSchedCode = getCpuSchedCode( dataBuffer );
                     break;

                   case CFG_QUANT_CYCLES_CODE:
                     tempData->quantumCycles = intData;
                     break;

                   case CFG_MEM_AVAILABLE_CODE:
                     tempData->memAvailable = intData;
                     break;

                   case CFG_PROC_CYCLES_CODE:
                     tempData->procCycleRate = intData;
                     break;

                   case CFG_IO_CYCLES_CODE:
                     tempData->ioCycleRate = intData;
                     break;

                   case CFG_LOG_TO_CODE:
                     tempData->logToCode = getLogToCode( dataBuffer );
                     break;

                   case CFG_LOG_FILE_NAME_CODE:
                     copyString( tempData->logToFileName, dataBuffer );
                     break;
                  }
               }
             // otherwise, assume data value not in range
             else
               {
                // free temp struct memeory
                   // function: free
                free ( tempData );
                // close file access
                   // function: fclose
                fclose( fileAccessPtr );
                // set end state message to configuration data out of range
                   // function: copyString
                copyString( endStateMsg, "Configuration item out of range" );
                // return data out of range
                return false;
               }
          }
       // otherwise, assume data line not found
       else
         {
          // free temp struct memeory
             // function: free
          free ( tempData );
          // close file access
             // function: fclose
          fclose( fileAccessPtr );
          // set end state message to configuration corrupt prompt error
             // function: copyString
          copyString( endStateMsg, "Corrupted configuration prompt" );
          // return corrupt config file code
          return false;
         }

       // increment line counter
       lineCtr++;
      }
    // end master loop

    // aquire end of sim config string
      // function: getLineTo
    if ( getStringToDelimiter( fileAccessPtr, PERIOD, dataBuffer ) != NO_ERR || compareString( dataBuffer, "End Simulator Configuration File" ) != STR_EQ  )
      {
       // free temp struct memeory
          // function: free
       free ( tempData );

       // close file access
          // function: fclose
       fclose ( fileAccessPtr );

       // set end state message to corrupt configuration end line
          // function: copyString
       copyString( endStateMsg, "Configuration end line capture error");

       // return corrupt file data
       return false;
      }
    // assign temporary pointer to parameter return pointer
    *configData = tempData;

    // close file access
      // function: fclose
    fclose ( fileAccessPtr );

    // return no error code
    return true;
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
    int strLen = getStringLength ( codeStr );
    char *tempStr = (char *)malloc( strLen + 1 );

    // set default return to FCFS-N
    int returnVal = CPU_SCHED_FCFS_N_CODE;

    // set string to lower case for testing
      // function: setStrToLowerCase
    setStrToLowerCase( tempStr, codeStr );

    // check for SJF-N
      // function: compareString
    if ( compareString( tempStr, "sjf-n" ) == STR_EQ )
      {
       // set return code to sjf
       returnVal = CPU_SCHED_SJF_N_CODE;
      }

    // check for SRTF-P
      // function: compareString
    if ( compareString( tempStr, "srtf-p" ) == STR_EQ )
      {
       // set return code to SRTF
       returnVal = CPU_SCHED_SRTF_P_CODE;
      }

    // check for FCFS-P
      // function: compareString
    if ( compareString( tempStr, "fcfs-p" ) == STR_EQ )
      {
       // set return code to fcfs
       returnVal = CPU_SCHED_FCFS_P_CODE;
      }

    // check for RR-P
      // function: compareString
    if ( compareString( tempStr, "rr-p" ) == STR_EQ )
      {
       // set return code to rr
       returnVal = CPU_SCHED_RR_P_CODE;
      }

    // free temp string memory
      // function: free
    free ( tempStr );

    // return resulting value
    return returnVal;
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
    if ( compareString( dataBuffer, "Version/Phase" ) == STR_EQ )
      {
       return CFG_VERSION_CODE;
      }

    if ( compareString( dataBuffer, "File Path" ) == STR_EQ )
      {
       return CFG_MD_FILE_NAME_CODE;
      }

    if ( compareString( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
      {
       return CFG_CPU_SCHED_CODE;
      }

    if ( compareString( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
      {
       return CFG_QUANT_CYCLES_CODE;
      }

   if ( compareString( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
     {
      return CFG_MEM_AVAILABLE_CODE;
     }

    if ( compareString( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )
      {
       return CFG_PROC_CYCLES_CODE;
      }

    if ( compareString( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
      {
       return CFG_IO_CYCLES_CODE;
      }

    if ( compareString( dataBuffer, "Log To" ) == STR_EQ )
      {
       return CFG_LOG_TO_CODE;
      }

    if ( compareString( dataBuffer, "Log File Path" ) == STR_EQ )
      {
       return CFG_LOG_FILE_NAME_CODE;
      }

    return CFG_CORRUPT_PROMPT_ERR;
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
    int strLen = getStringLength ( logToStr );
    char *tempStr = (char *)malloc( strLen + 1 );

    // set default to log to monitor
    int returnVal = LOGTO_MONITOR_CODE;

    // set temp string to lower case
      // function: setStrToLowerCase
    setStrToLowerCase ( tempStr, logToStr );

    // check for BOTH
      // function: compareString
    if ( compareString ( tempStr, "both" ) == STR_EQ )
      {
       // set return value to both code
       returnVal = LOGTO_BOTH_CODE;
      }

    // check for file
      // function: compareString
    if ( compareString ( tempStr, "file" ) == STR_EQ )
      {
       // set return value to file code
       returnVal = LOGTO_FILE_CODE;
      }

    // free temp string memory
      // function: free
    free ( tempStr );

    // return found code
    return returnVal;
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
    bool result = true;
    char *tempStr;
    int strLen;

    // use line code to identify prompt line
    switch ( lineCode )
      {
       // for version code
       case CFG_VERSION_CODE:
          // check if limits of version code are exceeded
          if ( doubleVal < 0.00 || doubleVal > 10.00 )
            {
             // set bool result to false
             result = false;
            }
          break;

       // for cpu scheduling code
       case CFG_CPU_SCHED_CODE:
          // create temp string and set to lower case
             // function: getStringLength, malloc, setStrToLowerCase
          strLen = getStringLength ( stringVal );
          tempStr = (char *)malloc( strLen + 1 );
          setStrToLowerCase( tempStr, stringVal );
          // check for not finding one of scheduling strings
             // function: compareString

          if ( compareString( tempStr, "fcfs-n" ) != STR_EQ
               && compareString( tempStr, "sjf-n" ) != STR_EQ
               && compareString( tempStr, "srtf-p" ) != STR_EQ
               && compareString( tempStr, "fcfs-p" ) != STR_EQ
               && compareString( tempStr, "rr-p" ) != STR_EQ )
             {
              // set bool result to false
              result = false;
             }

          // free temp string memory
             // function: free
          free( tempStr );

          break;

       // for quantum cycles
       case CFG_QUANT_CYCLES_CODE:
          // check for quantum cycles limits exceeded
          if ( intVal < 0|| intVal > 100 )
            {
             // set bool result to false
             result = false;
            }
          break;

       // for memory available
       case CFG_MEM_AVAILABLE_CODE:
          // check for available memory
          if ( intVal < 1024 || intVal > 102400 )
            {
             // set result to false
             result = false;
            }
          break;

       //check for process cycles
       case CFG_PROC_CYCLES_CODE:
          // check for progress cycles limit exceeded
          if ( intVal < 1 || intVal > 100 )
            {
             // set bool result to false
             result = false;
            }
          break;

       // check for I/O cycles
       case CFG_IO_CYCLES_CODE:
          // check for I/O cycles limits exceeded
          if ( intVal < 1 || intVal > 1000 )
            {
             // set bool result to false
             result = false;
            }
          break;
       // check for log to operations
       case CFG_LOG_TO_CODE:
          // create temp string and set to lower case
             // function: getStringLength, malloc, setStrToLowerCase
          strLen = getStringLength ( stringVal );
          tempStr = (char *)malloc( strLen + 1 );
          setStrToLowerCase( tempStr, stringVal );
          // check for not finding one of the log to strings
             // function : compareString
          if ( compareString( tempStr, "both" ) != STR_EQ && compareString( tempStr, "monitor" ) != STR_EQ && compareString( tempStr, "file" ) != STR_EQ)
            {
             // set bool result to false
             result = false;
            }

          // free temp string memory
             // function: free
          free ( tempStr );

          break;
      }

    // return results of limits analysis
    return result;
   }
