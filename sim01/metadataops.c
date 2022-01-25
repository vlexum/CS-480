// Header Files
#include <stdbool.h>

// Local Constants

/*
Function Name: clearMetaDataList
Algorithm: recursivley iterates through op code linked list, return memory to OS from the botttom of the list upward
Precondition: linked list, with or without data
Postcondition: all node memory, if any, is returned to OS, return pointer (head) is set to null
Exceptions: none
Notes: none
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr )
   {
    // check for localPtr not set to NULL

      // call recursive function with next pointer
         // function: clearMetaDataList

      // after recursive call, release memory to OS
         // funtion: free

      // set local pointer to null

    // return null to calling function

    return NULL;
   }

/*
Function Name: displayMetaData
Algorithm: iterates through op code linked list, displays op code data individually
Precondition: linked list, with or without data
Postcondition: displays all op codes in list
Exceptions: none
Notes: none
*/
void displayMetaData( OpCodeType *localPtr )
   {
    // display title, with underline
      // function: printf

    // loop to end of linked clearMetaDataList

      // print leader
         // function: printf

      // print op code pid
         // function: printf

      // print op code command
         // function: printf

      // check for dev op command

         // print in/out parameter
            // function: printf

      // otherwise assume other than dev

         // print NA
            // function: printf

         // print 1st string argument
            // function: printf

         // print first int argument
            // function: printf

         // print 2nd int argument
            // function: printf

         // print op end time
            // function: printf

         // end line
            // function: printf

         // assign local pointer to next node

    // end loop across nodes
   }

/*
Function Name: getMetaData
Algorithm: aquires the metadata items from the file, stores them in a linked list
Precondition: provided file name
Postcondition: return success, return linked list head via parameter, returns endstate/error message via parameter
Exceptions: function halted and error message returned if bad input data
Notes: none
*/
bool getMetaData( char *fileName, OpCodeType **opCodeDataHead, char *endStateMsg )
   {
    // initialize functions/variables

      // initialize read only constants

      // initialize variables

    // initialize op code data pointer in case of return error

    // initialize end state message
      // function: copyString

    // open file for reading
      // function: fopen

    // check for file open failure

      // set end state message
         // function: copyString

      // return file access error

    // check first line for correct leader
      // function: getLineTo, compareString

      // close file
         // function: fclose

      // set end state message
         // function: copyString

      // return corrupt descriptor error

    // allocate memory for the temp data structure
      // function: malloc

    // get the first op command
      // function: getOpCommand

    // get start and end counts for later comparison
      // function: updateStartCount, updateEndCount

    // check for failure of first complete op command

      // close file
         // function: fclose

      // clear data from structure list
         // function: clearMetaDataList

      // free temp structure memory
         // function: free

      // set end state message
         // function: copyString

      // return result of operation

    // Loop across all remaining op commands
    // (while complete op commands are found)

      // add the new op command to the linked list
         // function: addNode

      // get op command
         // function: getOpCommand

      // update start and end counts for later comparison
         // function: updateStartCount, updateEndCount

    // end loop across remaining op commands

    // after loop completion, check for last op command found

      // check for start and end op code counts equal

         // add the last node to the linked list
            // function: addNode

         // set access result to no error for later operation

         // check last line for incorrect end descriptor
            // function: getLineTo, compareString

            // set access result to corrupted descriptor error

            // set end state message
               // function: copyString

    // otherwise, assume didnt find end

      // set end state message
         // function: copyString

      // unset return state

    // check for any errors found (not no error)

      // clear op command list
         // fuction: clearMetaDataList

    // close access file
       // function: fclose

    // release temporary structure memory
       // function: free

    // assign temporary local head pointer to parameter return pointer

    // return access result

    return false;
   }

/*
Function Name: getOpCommand
Algorithm: acquires one op command, verifies all parts of it, returns as parameter
Precondition: file is open and file cursor is at beginning of an op code
Postcondition: in correct operation, finds, tests, and returns op command as parameter, and returns status as integer - either complete op command found, or last op  command found
Exceptions: correctly and appropriately, responds to and reports file access failure, incorrectly formatted op command letter, incorrectly formatted op command name, incorrect or out of range op command value
Notes: none
*/
int getOpCommand( FILE *filePtr, OpCodeType *inData )
   {
    // initialize function/variables

    // initialize local Constants

    // initialize other variables

    // get whole op command as string
      // function: getLineTo

    // check for successful access

      // get three-letter command
         // getCommand

      // assign op command to node
         // function: copyString

    // otherwise, assume unsuccessful access

      // set pointer to data structure to NULL

      // return op command access failure

    // verify op command
      // function: verifyValidCommand

      // return op command error

    // set all struct values that may not be initialized to defaults

    // check for device command
      // function: compareString

      // get in/out arguement
         // fucntion: getStringArg

      // set device in/out arguement
         // function: copyString

      // check for correct argument
         // function: compareString

         // return argument error

    // get first string arg
      // function: getStringArg

    // set device in/out argument
      // function: copyString

    // check for legitimate first string arg
      // function: verifyFirstStringArg

      // return argument error

    // check for last op command found
      // function: compareString

      // return last op command found

    // check for app start seconds argument
      // function: compareString

      // get number argument
         // function: getNumberArg

      // check for failed number access

         // set failure flag

      // set first int argument to number

    // check for cpu cycle time
      // function: compareString

      // get number argument
         // function: getNumberArg

      // check for failed number access

         // set failure flag

      // set first int argument to number

    // check for device cycle time
       // function: compareString

       // get number argument
          // function: getNumberArg

       // check for failed number access

          // set failure flag

       // set first int argument to number

    // check for memory base and offset
       // function: compareString

       // get number argument for base
          // function: getNumberArg

       // check for failed number access

          // set failure flag

       // set first int argument to number

       // get number argument for offset
          // function: getNumberArg

       // check for failed number access

          // set failure flag

       // set second int argument to number

    // check for int args for upload failure

      // return argument error

    // return complete op command found message

    return 0;
   }

/*
Function Name: updateStartCount
Algorithm: updates number of "start" op commands found in file
Precondition: count >= 0, op string has "start" or other op name
Postcondition: if op string has "start", input count + 1 is returned, otherwise is unchanged
Exceptions: none
Notes: none
*/
int updateStartCount( int count, char *opString )
   {
    // check for "start" in op string
      // function: compareString

      // return incremented start count

    // return unchanged start count

    return 0;
   }

/*
Function Name: updateEndCount
Algorithm: updates number of "end" op commands found in file
Precondition: count >= 0, op string has "end" or other op name
Postcondition: if op string has "end", input count + 1 is returned, otherwise is unchanged
Exceptions: none
Notes: none
*/
int updateEndCount( int count, char *opString )
   {
    // check for "end" in op string
      // function: compareString

      // return incremented end count

    // return unchanged end count

    return 0;
   }

/*
Function Name: addNode
Algorithm: adds op command structure with data to a linked list
Precondition: linked list pointer assinged to null or one op command link, struct pointer assigned to op command struct data
Postcondition: assigns new structure node to beginning of linked list or and of established linked list
Exceptions: none
Notes: assume memory access/availability
*/
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode )
   {
    // check for local pointer assigned to NULL

       // access memory for new link/node
          // function: malloc

       // assign all three values to newly created node
       // assign next pointer to null
          // function: copyString

       // return current local pointer

    // assume end of list not found yet
    // assign recursive function to current's next link
       // function: addNode

    return NULL;
   }

/*
Function Name: getCommand
Algorithm: gets first three letters of input string  to get command
Precondition: provided starting index
Postcondition: returns command (ref param) and updated index (return)
Exceptions: none
Notes: none
*/
int  getCommand( char *cmd, char *inputStr, int index )
   {
    // initialize function/variables

    // loop across command length

       // assign character from input string to buffer string

       // increment index

       // set next character to null character

    //end loop

    // return current index

    return 0;
   }

/*
Function Name: verifyValidCommand
Algorithm: check string argument for one of the allowed commands
Precondition: input string is provided
Postcondition: in correct operation, reports if given string is a command, false if not
Exceptions: none
Notes: none
*/
bool verifyValidCommand( char *testCmd )
   {
    // initialize function/variables

    // check for fice string command arguments

       // return true

    // return false
    return false;
   }

/*
Function Name: getStringArg
Algorithm: skips leading white space, acquires sub string from command input string ending at comma or end of string
Precondition: input string has some remaining string argument
Postcondition: in correct operation, captures next string argument
Exceptions: none
Notes: none
*/
int getStringArg( char *strArg, char *inputStr, int index )
   {
    // initialize function/variables

    // loop to skip white space and comma

      // increment index

    // end loop for white space and comma

    // loop across string length

       // assign character from input string to buffer string

       // increment index

       // set next character to null character

    //end loop across string length

    // return current index

    return 0;
   }

/*
Function Name: verifyFirstStringArg
Algorithm: check string argument for one of the allowed string arguments
Precondition: input string is provided
Postcondition: in correct operation, reports true if given string is in argument list, false if not
Exceptions: none
Notes: none
*/
bool verifyFirstStringArg( char *strArg )
   {
    // initialize function/variables

    // check for all possible string arg 1 possibilities
       // function: compareString

       // return true

    // return false
    return false;
   }

/*
Function Name: getNumberArg
Algorithm: skips leading white space, acquires next integer from op command input string ending at comma or end of string
Precondition: input string has some remaining string arguments
Postcondition: in correct operation, captures next integer argument, returns index location after process is finished
Exceptions: none
Notes: none
*/
int getNumberArg( int *number, char *inputStr, int index )
   {
    // initialize function/variables

    // loop to skip white space and comma

      // increment index

    // end loop for white space and comma

    // loop across string length
       // function: isDigit

       // set digit found flag

       // assign digit to output

       // increment index and multiplier

    // end loop across string length

    // check for digit not found

       // set number to BAD_ARG_VAL constant

    // return current index
    return 0;
   }

/*
Function Name: isDigit
Algorithm: checks for character digit, return result
Precondition: test value is character
Postcondition: if test value is '0' < value < '9' return true, otherwise false
Exceptions: none
Notes: none
*/
bool isDigit ( char testChar )
   {
    // initialize function/variables

    // check for test character between '0'-'9'

       // return true

    // otherwise, assume character is not digit, return false
    return false;
   }
