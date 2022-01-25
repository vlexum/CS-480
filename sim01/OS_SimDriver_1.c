// header files
#include <stdbool.h>

int int main(int argc, char **argv)
   {
    // Initialize program
      // initialize variables
    bool programRunFlag = false;
    bool configDisplayFlag = false;
    bool configUploadFlag = false;
    bool mdDisplayFlag = false;
    bool runSimFlag = false;
    bool infoFlag = false;
    int argIndex = 1;
    int lastFourLetters = 4;
    int fileStrLen, fileStrSubLoc;
    char fileName[ STD_STR_LEN ];
    char errorMessage[ MAX_STR_LEN ];
    ConfigDataType *configDataPointer = NULL;
    OpCodeType *metaDataPtr = NULL;

      // show title
         // function: printf
      printf("\nSimulator Program\n");
      printf("=================\n\n");

    // check for only one arg
    if (argc < 2)
      {
       // show error, end program
         // function: showProgramFormat
       showProgramFormat();
       // unset program run flag
       programRunFlag = false;
       //set info flag
       infoFlag = true;
      }
    // initialize file name to empty
    fileName[0] = NULL_CHAR;

    // loop across args (starting at 1) and program run flag
    while ( programRunFlag == false && argIndex < argc )
       {
        // find lengths to verify file name
         // function: getStringLength, findSubString
          fileStrLen = getStringLength( argv[ argIndex ] );
          fileStrSubLoc = findSubString( argv[ argIndex ], ".cnf" );

         // check for -dc (display config flag)
            // function: compareString
         if ( compareString( argv[ argIndex ], "-dc" ) == STR_EQ )
          {
           // set config upload flag
           configUploadFlag = true;
           // set config display flag
           configDisplayFlag = true;
          }

      // otherwise, check for -dm (display metadata flag)
         // function: compareString
      else if ( compareString( argv[ argIndex ], "-dm" ) == STR_EQ )
         {
         // set config upload flag
          configUploadFlag = true;
         // set meta data display flag
          mdDisplayFlag = true;
         }
      //otherwise, check for -rs (run simulator)
         // function: compareString
      if ( compareString( argv[ argIndex ], "-rs" ) == STR_EQ )
         {
         // set config upload flag
          configUploadFlag = true;
         // set run simulator flag
          runSimFlag = true;
         }
      // otherwise, check for file name, ending in .config
      // must be last flag
      else if ( fileStrSubLoc != SUBSTRING_NOT_FOUND && fileStrSubLoc ==       fileStrLen - lastFourLetters )
         {
         // set file name to variable
            // function: copyString
          copyString( fileName, argv[ argIndex ] );

         // set program run flag
          programRunFlag = true;
         }
      // update arg index
       argIndex++;

      //end arg loop
      }
      // check for command line failure
      if ( programRunFlag == false && infoFlag == false )
         {
         // show error message, end program
            // fucntion: printf
          printf( "Incorrect argument line format, program aborted\n\n" );

         // provide command line info to user
            // function: showProgramFormat
          showProgramFormat():
         }
      // check for program run and config upload flag
      if ( programRunFlag == true && configUploadFlag == true )
         {
         //upload config file, check for success
            // function: getStringLength, getConfigData
          if ( getStringLength( fileName ) > 0 && getConfigData( fileName, &configDataPtr, errorMessage) == true )
            {
             // check config display flag
             if ( configDisplayFlag == true )
                {
                // display config data
                   // function: displayConfigData
                 displayConfigData( configDataPtr );
                }
            }
         // otherwise, assume congig file failure
         else
            {
            // show error message, end program
               // function: printf
             printf( "\nConfig Upload Error: %s, program aborted\n\n", errorMessage );
            // unset program run flag
             programRunFlag = false;
            }
         }
         // check for program run flag
         if ( programRunFlag == true && ( mdDisplayFlag == true || runSimFlag == true ) )
            {
            // upload meta data file, check for success
               // function: getMetaData
             if ( getMetaData( configDataPtr->metaDataFileName, &metaDataPtr, errorMessage ) == true )
               {
               //check meta data display flag
                if ( mdDisplayFlag == true )
                   {
                   //display meta data
                      // function: displayMetaData
                    displayMetaData( metaDataPtr );
                   }
                // check run simulator flag
                if ( runSimFlag == true )
                   {
                   // run simulator
                      // function: runSim
                    runSim( configDataPtr, metaDataPtr );

                   }
               // otherwise, assume meta data upload failure
               else
                  {
                  // show error message, end program
                     // function: printf
                   printf( "\nMetadata Upload Error: %s, program aborted\n", errorMessage );
                  }
            }
      // clean up config data as needed
         // function: clearConfigData
      configDataPtr = clearConfigData( configDataPtr );

      // clean up metadata as needed
         // function: clearMetaData
      metaDataPtr = clearMetaDataList( metaDataPtr );

      // show program end
         // fucntion: printf
      printf( "\nSimulator Program End.\n\n" );
      // return success
    return 0;
   }

/*
name: showProgramFormat
process : displays command line argument requirements for this showProgramFormat
method input parameter: none
method output parameter: none
method output returned: none
device input: none
device output: instructions provided as specified
dependencies: none
*/
void showProgramFormat()
   {
    //print out command linme argument instructions
      // function: printf
    print("\n\n\n")

   }
