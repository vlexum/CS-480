#include "simulator.h"

// int value used to keep track of the number of processes
int processCounter = 0;

int logMode;

bool preemption = false;

int quantumCycles;

char timeStr[ 10 ];

char *logFileName;

OperationDataNode *logHead = NULL;
OperationDataNode *wkgLogRef = NULL;

InterruptData *intHeadRef = NULL;
InterruptData *wkgIntRef = NULL;

/*
Function Name: buildPCB
Algorithm: loops though metadata linked list and searches for "app start",
if app start is seen then we add a node to the linked list
Precondition: configPtr is not null
Postcondition: PCB LL is created
Exceptions: none
Notes: none
*/
ProcessType *buildPCB( OpCodeType *metaDataMstrPtr, ConfigDataType *configPtr )
   {
    // initialize function/variables
    ProcessType *localHeadPtr = NULL;
    ProcessType *temp;
    ProcessType *localPtr;

    int procCycleRate = configPtr->procCycleRate;
    int ioCycleRate = configPtr->ioCycleRate;

    int time = 0;

    // loop while there are op codes
    while ( metaDataMstrPtr != NULL )
       {
        // check for app start
        if ( compareString( metaDataMstrPtr->command, "app" ) == STR_EQ &&
             compareString( metaDataMstrPtr->strArg1, "start" ) == STR_EQ )
           {
            // We can now create the PCB
            localPtr = (ProcessType *)malloc( sizeof( ProcessType ) );

            // update process counter before because it starts at 0.
            processCounter++;

            // assing the data to PCB
            localPtr->pid = processCounter;
            localPtr->state = NEW;
            localPtr->prevState = NEW;
            localPtr->programCounter = metaDataMstrPtr;

            // if this is the first process assign the headPtr
            if ( processCounter == 1 )
               {
                localHeadPtr = temp = localPtr;
               }

            // otherwise set temp and its next to localPtr to build the LL
            else
               {
                temp->nextProcess = localPtr;
                temp = localPtr;
               }
           }

        // check for app end
        else if ( compareString( metaDataMstrPtr->command, "app" ) == STR_EQ &&
                  compareString( metaDataMstrPtr->strArg1, "end" ) == STR_EQ )
           {
            // set remaining time to the calculated total
            localPtr->remainingTime = time;

            // reset time to 0 for the next process ( if there is one )
            time = 0;
           }

        // check to make sure its not a memory operations so we can do the math
        // do not need to do math with mem op
        else if ( compareString( metaDataMstrPtr->command, "mem" ) != STR_EQ )
           {
            // check if an I/O op
            if ( compareString( metaDataMstrPtr->command, "dev" ) == STR_EQ )
               {
                // add to time using I/O cycle rate
                time += ( ( metaDataMstrPtr->intArg2 ) * ioCycleRate );
               }

            // check if NOT sys op
            else if ( compareString( metaDataMstrPtr->command, "sys" ) !=
                                                                        STR_EQ )
               {
                // add to time using cpu cycle rate
                time += ( ( metaDataMstrPtr->intArg2 ) * procCycleRate );
               }
           }

        // update pointer so it points to next op code
        metaDataMstrPtr = metaDataMstrPtr->nextNode;
       }

    // last node next ref will always be NULL
    temp->nextProcess = NULL;

    // return pointer to PCB head
    return localHeadPtr;
   }

/*
Function Name: clearLog
Algorithm: loops though linked list and free each nodes memory.
Precondition: localPtr is linked list
Postcondition: all node memory, if any, is returned to OS, return pointer (head)
is set to null
Exceptions: none
Notes: none
*/
OperationDataNode *clearLog( OperationDataNode *localPtr )
   {
    // check for localPtr not set to NULL
    if ( localPtr != NULL )
      {
       // call recursive function with next pointer
          // function: clearLog
       clearLog( localPtr->nextNode);

       // after recursive call, release memory to OS
          // funtion: free
       free( localPtr );

       // set local pointer to null
       localPtr = NULL;
      }

    // return null to calling function
    return NULL;
   }

/*
Function Name: clearPCB
Algorithm: loops though linked list and free each nodes memory.
Precondition: localPtr is linked list
Postcondition: all node memory, if any, is returned to OS, return pointer (head)
is set to null
Exceptions: none
Notes: none
*/
ProcessType *clearPCB( ProcessType *localPtr )
   {
    // check for localPtr not set to NULL
    if ( localPtr != NULL )
      {
       // call recursive function with next pointer
          // function: clearPCB
       clearPCB( localPtr->nextProcess );

       // after recursive call, release memory to OS
          // funtion: free
       free( localPtr );

       // set local pointer to null
       localPtr = NULL;
      }

    // return null to calling function
    return NULL;
   }

/*
Function Name: clearInterrupts
Algorithm: loops though linked list and free each nodes memory.
Precondition: localPtr is linked list
Postcondition: all node memory, if any, is returned to OS, return pointer (head)
is set to null
Exceptions: none
Notes: none
*/
InterruptData *clearInterrupts( InterruptData *localPtr )
   {
    // check for localPtr not set to NULL
    if ( localPtr != NULL )
      {
       // call recursive function with next pointer
          // function: clearInterrupts
       clearInterrupts( localPtr->nextInterrupt );

       // after recursive call, release memory to OS
          // funtion: free
       free( localPtr );

       // set local pointer to null
       localPtr = NULL;
      }

    // return null to calling function
    return NULL;
   }

/*
Function Name: findNextOpCode
Algorithm: sets process op code pointer to the next node
Precondition: process is a linked list
Postcondition: program counter points to the next op code needed
Exceptions: none
Notes: wont get to null since check in master loop
*/
OpCodeType *findNextOpCode( ProcessType *process )
   {
    // Determine if the opCode has finished processing -> no cycles left or mem
    // Needed to keep track for preemption
    if ( process->programCounter->intArg2 <= 0 ||
           compareString( process->programCounter->command, "mem" ) == STR_EQ )
      {
       // update programCounter
       process->programCounter = process->programCounter->nextNode;
      }

    // return the opCode
    return process->programCounter;
   }

/*
Function Name: findNextProcess
Algorithm: uses cpuSchedMode to decide the next process only if flag set to true
Precondition: none
Postcondition: appropriate next process is chosen
Exceptions: none
Notes: FCFSP and RRP use the same strategies
*/
ProcessType *findNextProcess( ProcessType *currentProcess, ProcessType *pcbHead,
                                          int cpuSchedMode, bool switchProcess )
   {
    // initialize function/variables
    switch ( cpuSchedMode )
       {
        case CPU_SCHED_SJF_N_CODE:
           // check switchProcess to see if we need to switch (process complete)
           if ( switchProcess )
              {
               ProcessType *shortestJob;
               ProcessType *wkgHeadRef;

               // update time
               accessTimer( LAP_TIMER, timeStr );

               shortestJob = pcbHead;
               wkgHeadRef = pcbHead;

               // fix shortest job if not ready
               while ( shortestJob->state != READY && shortestJob != NULL)
                  {
                   shortestJob = shortestJob->nextProcess;
                  }

               // loop to find shortest job
               while ( wkgHeadRef != NULL )
                  {
                   // determine the shortest job remaining
                   if ( ( wkgHeadRef->remainingTime < shortestJob->remainingTime )
                                              && wkgHeadRef->state == READY )
                      {
                       // update shortest job pointer to new shortest job
                       shortestJob = wkgHeadRef;
                      }
                   // update working ref
                    wkgHeadRef = wkgHeadRef->nextProcess;
                  }

               // display process change
                  // function: manageDisplay
               manageDisplay( shortestJob, PROCESS_CHANGE, NULL );

               // change the state to running
                  // function: setState
               setState( shortestJob, RUNNING, false );

               // return the next process
               return shortestJob;
              }

           return currentProcess;

        break;

        case CPU_SCHED_SRTF_P_CODE:
           // check switchProcess to see if we need to switch (process complete)
           if ( switchProcess )
              {
               ProcessType *shortestJob;
               ProcessType *wkgHeadRef;

               // update time
               accessTimer( LAP_TIMER, timeStr );

               shortestJob = pcbHead;
               wkgHeadRef = pcbHead;

               // fix shortest job if not ready
               while ( shortestJob != NULL && shortestJob->state != READY )
                  {
                   shortestJob = shortestJob->nextProcess;
                  }

                // essentially the same as SJF but with preemption we need to
                // return NULL if no processes avaialable
                // if shortestJob is NULL then we know all operating processes
                // are blocked
                if ( shortestJob == NULL )
                  {
                   return NULL;
                  }

               // loop to find shortest job
               while ( wkgHeadRef != NULL )
                  {
                   // determine the shortest job remaining
                   if ( ( wkgHeadRef->remainingTime < shortestJob->remainingTime )
                                              && wkgHeadRef->state == READY )
                      {
                       // update shortest job pointer to new shortest job
                       shortestJob = wkgHeadRef;
                      }
                   // update working ref
                    wkgHeadRef = wkgHeadRef->nextProcess;
                  }

               // display process change
                  // function: manageDisplay
               manageDisplay( shortestJob, PROCESS_CHANGE, NULL );

               // change the state to running
                  // function: setState
               setState( shortestJob, RUNNING, false );

               // return the next process
               return shortestJob;
              }

             // check if state is read because process can come in ready with preemption
             if ( currentProcess != NULL && currentProcess->state == READY   )
              {
               // set running
                  // function: setState
               setState( currentProcess, RUNNING, false );
              }

             return currentProcess;
        break;

        case CPU_SCHED_FCFS_P_CODE:
        case CPU_SCHED_RR_P_CODE:
           // check switchProcess ( which is set true by intterupt in runSim )
           if ( switchProcess && currentProcess != NULL )
              {
               // used to count how many processes are blocked
               int counter = 0;

               ProcessType *nextProcessPtr;

               // set ptr for next to head
               nextProcessPtr = pcbHead;

               // update time
               accessTimer( LAP_TIMER, timeStr );

               // update nextProcess if it has a next node
               // if its the last node then the next ptr will be at the head since
               // its circular
               if ( currentProcess->nextProcess != NULL )
                  {
                   // update the ptr
                   nextProcessPtr = currentProcess->nextProcess;
                  }

               // Loop to find the next process that is ready in our queue
               while ( nextProcessPtr->state != READY )
                  {
                   // keep track of the number of blocked proccess
                   if ( nextProcessPtr->state == BLOCKED )
                      {
                       // update counter
                       counter++;
                      }

                   // update the next process
                   nextProcessPtr = nextProcessPtr->nextProcess;

                   // if nextProcess is null go back to the head
                   if ( nextProcessPtr == NULL )
                      {
                        nextProcessPtr = pcbHead;
                      }

                   // if the # blocked = # process then we are in idle and return
                   // NULL so the master loop know we are idling
                   if ( counter == processCounter )
                      {
                      return NULL;
                      }
                 }

               // change the state to RUNNING
                 // function: setState
              setState( nextProcessPtr, RUNNING, false );

               // display process change
                  // function: manageDisplay
               manageDisplay( nextProcessPtr , PROCESS_CHANGE, NULL);

              // return the next process
              return nextProcessPtr;
              }

           // check if state is read because process can come in ready with preemption
          if ( currentProcess != NULL && currentProcess->state == READY )
           {
            // set running
               // function: setState
            setState( currentProcess, RUNNING, false );
           }

           // else we return the current
           return currentProcess;

        break;

        // defaults to FCFS_N
        case CPU_SCHED_FCFS_N_CODE:
           // check switchProcess to see if we need to switch (process complete)
           if ( switchProcess )
              {
               // update time
               accessTimer( LAP_TIMER, timeStr );

               // display process change
                  // function: manageDisplay
               manageDisplay(currentProcess->nextProcess, PROCESS_CHANGE, NULL);

               // change the state to running
                  // function: setState
               setState(currentProcess->nextProcess , RUNNING, false );

               // return the next process
               return currentProcess->nextProcess;
              }

           // otherwise return the same process
           return currentProcess;
       }

    return NULL;
   }

/*
Function Name: freeMemBlock
Algorithm: loops through sys mem LL and free memory if the process id matches
Precondition: process is EXITING
Postcondition: exiting process memory is freed
Exceptions: none
Notes:
*/
void freeMemBlock( MMU *mmu, ProcessType *process )
   {
    // initialize vars
    MemoryDataType *localPtr;
    MemoryDataType *nextPtr = NULL;
    MemoryDataType *prevPtr = NULL;

    // set ptr to head of system memory LL
    localPtr = mmu->sysMemory;

    // loop through system memory
    while ( localPtr != NULL )
       {
        // saves the nextBlock so we can link the list once memory is freed
        nextPtr = localPtr->nextBlock;

        // check if same process
        if ( localPtr->pid == process->pid )
           {
            // check if head
            if ( prevPtr == NULL )
               {
                // set head to the next block
                mmu->sysMemory = nextPtr;
               }
            else
               {
                // set the previous block next block to the ptr next
                prevPtr->nextBlock = nextPtr;
               }

            // update MMU values as necessary
            mmu->numProc = mmu->numProc - 1;
            mmu->memAvailable = mmu->memAvailable + localPtr->offset;

            // free memory from the node as it is now unlinked
            free( localPtr );
           }

        // update local vars as needed
        prevPtr = localPtr;
        localPtr = nextPtr;
       }
   }

/*
Function Name:interruptManager
Algorithm: loops through interrupt queue and determines if an intterupt is necessary
Precondition: none
Postcondition: first available interrupt is handled
Exceptions: none
Notes: intHeadRef is global var
*/
ProcessType *interruptManager()
   {
    // update time
    double time = accessTimer( LAP_TIMER, timeStr );

    InterruptData *wkgRef;

    // set the wkg ref to the head of the interrupt queue
    wkgRef = intHeadRef;

    // loop while wkgRef not null
    while ( wkgRef != NULL )
       {
        // Determine if process is ready to interrupt
        if ( time >= wkgRef->blockedEndTime && !wkgRef->serviced )
           {
             // process is being serviced -> set true
             wkgRef->serviced = true;

             // return the process thats is interrupting
             return wkgRef->process;
           }

        // update wkg ref
        wkgRef = wkgRef->nextInterrupt;
       }

    // if no interrupts return NULL
    return NULL;
   }

/*
Function Name: manageDisplay
Algorithm: handles all printing operations for simulator, assigns aprropriate
message to completeMessage using snprintf, appropriate message is decided based
off the displayFlag, at the end it will be diplayed to the appropriate place
using the logMode
Precondition: displayFlag is valid flag in DisplayFlags ( header )
Postcondition: completeMessage is diplayed to appropriate place
Exceptions: none
Notes: none
*/
void manageDisplay( ProcessType *process, int displayFlag, char *opMessage )
   {
    // initialize function/variables
    char completeMessage[ MAX_STR_LEN ];

    // check if system start
    if ( displayFlag == SYS_START )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage, MAX_STR_LEN,
                  " 0.000000, OS: Simulator Start\n"
                );
       }

    // check if state change
    else if ( displayFlag == STATE_CHANGE )
       {
         // initialize strings for the prevState and currState
        char prevState[7];
        char currState[7];

        // assign state strings
          // fuinction: stateToString
        stateToString( process->prevState, prevState );
        stateToString( process->state, currState );

        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Process %d set to %s state from %s state\n",
                  timeStr,
                  process->pid,
                  currState,
                  prevState
                );
       }

    // check if process change
    else if ( displayFlag == PROCESS_CHANGE )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Process %d selected with %d ms remaining\n",
                  timeStr,
                  process->pid,
                  process->remainingTime
                );
       }

    // check if op command and the op message is not null
    else if ( displayFlag == OP_COMMAND && opMessage != NULL )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, Process: %d, %s\n",
                  timeStr,
                  process->pid,
                  opMessage
                );
       }

    // check if mem op was tried
    else if ( displayFlag == MEM_START )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, Process: %d, mem %s request (%d, %d)\n",
                  timeStr,
                  process->pid,
                  process->programCounter->strArg1,
                  process->programCounter->intArg2,
                  process->programCounter->intArg3
                );
       }

    // check if mem operation was Successful
    else if ( displayFlag == MEM_SUCCESS )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, Process: %d, successful mem %s request (%d, %d)\n",
                  timeStr,
                  process->pid,
                  process->programCounter->strArg1,
                  process->programCounter->intArg2,
                  process->programCounter->intArg3
                );
       }

    // check if mem op was a failed attempt
    else if ( displayFlag == MEM_FAILURE )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, Process: %d, failed mem %s request (%d, %d)\n",
                  timeStr,
                  process->pid,
                  process->programCounter->strArg1,
                  process->programCounter->intArg2,
                  process->programCounter->intArg3
                );

        manageDisplay( process, SEGFAULT, NULL);
       }

    // check if mem op was a failed attempt
    else if ( displayFlag == SEGFAULT )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Segmentation Fault -> Process: %d ended\n\n",
                  timeStr,
                  process->pid
                );
       }

    // check if process was blocked for I/O
    else if ( displayFlag == GOTBLOCKED )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "\n%s, OS: Process: %d blocked for %s\n\n",
                  timeStr,
                  process->pid,
                  opMessage
                );
       }

    // check if process end
    else if ( displayFlag == PROC_END )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Process %d ended\n\n",
                  timeStr,
                  process->pid
                );
       }

    // check if system end
    else if ( displayFlag == SYS_END )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: System Stop\n",
                   timeStr
                );
       }

    // check for intterupt
    else if ( displayFlag == INTERRUPT )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Interrupted by process %d\n",
                  timeStr,
                  process->pid
                );
       }

    // check for intterupt
    else if ( displayFlag == IDLE )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, CPU IDLE, all active processes blocked\n",
                   timeStr
                );
       }

    // check for intterupt
    else if ( displayFlag == ENDIDLE )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: CPU interrupt, end idle\n",
                  timeStr
                );
       }
    // check if simulation end
    else if ( displayFlag == SIM_END )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage,
                  MAX_STR_LEN,
                  "%s, OS: Simulation End\n\n",
                   timeStr
                );

        // since its end of sim we can write to the log file
        // check for logMode and if processCounter is 0 ( sim end )
        if ( ( logMode == LOGTO_BOTH_CODE || logMode == LOGTO_FILE_CODE ) )
           {
            // create file ptr
            FILE *fileAccessPtr;

            // open file
            fileAccessPtr = fopen( logFileName, "w" );

            // loop through log LL
            while ( logHead != NULL )
               {
                // write to file
                fputs( logHead->lineData, fileAccessPtr );

                // update logptr
                logHead = logHead->nextNode;
               }

            fclose(fileAccessPtr);
           }
       }

    // check if we need to print message and message is not null
    else if ( displayFlag == PRINT_MESSAGE && opMessage != NULL )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage, MAX_STR_LEN, "%s, %s", timeStr, opMessage );
       }

    // print the message if logMode is both or monitor
    if ( logMode == LOGTO_BOTH_CODE || logMode == LOGTO_MONITOR_CODE )
       {
        // print the complete message to monitor
        printf("%s", completeMessage );
       }

    // store the message so we can write to file if logMode is both or file
    if ( logMode == LOGTO_BOTH_CODE || logMode == LOGTO_FILE_CODE )
       {
        // store the message
        OperationDataNode *newNode;

        // allocate memory for new node
         // function : malloc
        newNode = (OperationDataNode *)malloc( sizeof( OperationDataNode ) );

        // set data in new node
        copyString( newNode->lineData, completeMessage );
        newNode->nextNode = NULL;

        // if head is null than new node becomes head and the wkgRef
        if ( logHead == NULL )
           {
            logHead = wkgLogRef = newNode;
           }
        // else we append to end of the log
        else
           {
            // append node
            wkgLogRef->nextNode = newNode;

            // update wkgRef
            wkgLogRef = wkgLogRef->nextNode;
           }
       }
   }

/*
Function Name: manageMemory
Algorithm: It first loops through existing memory if it exists and then it will
try to find the memory no matter which op is being done( ALLOCATE or ACCESS ).
If it is found and we are accessing then it returns success, if it is an
allocation then it sets the segFaultFlag. If not found and accessing then it
returns failure. Now if it is not found and we are allocating then we can step
into the allocation and allocate the memory as necessary.
Input Parameter(s): pointer to process, pointer to the MMU and a flag which
tells us which operation is being done
Precondition(s): must have "mem" command
Postcondition(s): boolean result of mem operation
Exceptions: none
Notes: none
*/
bool manageMemory( ProcessType *process, MMU *mmu, MemOpType memFlag )
   {
    // initialize variables
    bool segFaultFlag = false;

    OpCodeType *opCode;
    MemoryDataType *wkgMemRef;

    MemoryDataType *lastBlock = NULL;
    MemoryDataType *newBlock;

    wkgMemRef = mmu->sysMemory;
    opCode = process->programCounter;

    int memBase = opCode->intArg2;
    int memOffset = opCode->intArg3;

    int memAddress = memBase + memOffset;

    // update time
      // function: accessTimer
    accessTimer( LAP_TIMER, timeStr );

    // loop through memory to find the memory wanted
    while ( wkgMemRef != NULL && mmu->numProc > 0 )
      {
       // check if any part of block inside another block
       if( ( memBase >= wkgMemRef->base && memBase < wkgMemRef->addressReg ) ||
       ( memAddress >= wkgMemRef->base && memAddress < wkgMemRef->addressReg ) )
          {
           // check if we are acccessing mem and inside memory block of same process
           if ( ( memFlag == ACCESS &&
                  memBase >= wkgMemRef->base ) &&
                  memAddress < wkgMemRef->addressReg &&
                  process->pid == wkgMemRef->pid )
             {
              // Succesful mem access
              return true;
             }

           // if inside and not accessing then we have seg fault for allocating
           segFaultFlag = true;
          }

        // update mem ref and store pointer to last block so it can be used if
        // allocating
        lastBlock = wkgMemRef;
        wkgMemRef = wkgMemRef->nextBlock;
      }

   // check if allocating and check if segFaultFlag was triggered
    if ( memFlag == ALLOCATE && !segFaultFlag )
       {
        // check if there is enough memory
        if ( !( memOffset > mmu->memAvailable ) )
           {
            // create new memory block
               // function: malloc
            newBlock = (MemoryDataType *)malloc( sizeof( MemoryDataType ) );

            // assign all vars for new mem block
            newBlock->pid = process->pid;
            newBlock->offset = memOffset;
            newBlock->base = memBase;
            newBlock->addressReg = memAddress;

            // make sure next is NULL so looping is simple
            newBlock->nextBlock = NULL;

            // update necessary mmu vars
            mmu->memAvailable = mmu->memAvailable - memOffset;
            mmu->numProc = mmu->numProc + 1;

            // check if head is null
            if ( lastBlock == NULL )
               {
                // set head to the newBlock
                mmu->sysMemory = newBlock;

                return true;
               }

            // otherwise link the new node to LL
            lastBlock->nextBlock = newBlock;

            // return success
            return true;
          }
       }

    // return failure aka seg fault
    return false;
   }

/*
Function Name: printMemMap
Algorithm: goes through memory LL and prints out the current memory blocks
Precondition:  none
Postcondition: mem map is printed
Exceptions: none
Notes:
*/
void printMemMap( MMU *mmu )
   {
    // initialize function/variables
    MemoryDataType *wkgMemRef;
    int physMem = 0;

    wkgMemRef = mmu->sysMemory;

    // print barrier
       //function: printf
    printf("------------------------------\n");

    // loop through memory blocks
    while ( wkgMemRef != NULL )
      {
        //print the diagnostic
        printf( "%d [ Used by Process: %d, %d-%d ] %d\n",
                 physMem,
                 wkgMemRef->pid,
                 wkgMemRef->base,
                 wkgMemRef->addressReg - 1,
                 physMem + wkgMemRef->offset - 1
                 );

        // update local variables
        physMem += wkgMemRef->offset;
        wkgMemRef = wkgMemRef->nextBlock;
      }

    // check if still have memory left and display as empty block
    if ( physMem < mmu->memAvailable )
       {
        // print the empty block
           // function: printf
        printf( "%d [ Open , 0-0 ] %d\n",
                 physMem,
                 mmu->memTotal - 1
                 );
       }

    // print barrier
       //function: printf
    printf("------------------------------\n");
   }

/*
Function Name: runOpCode
Algorithm: "processes" the op code by calculating the process time and then runs
 timer as a thread
Precondition: none
Postcondition: timer is run for appropriate amount of time
Exceptions: preemption and command is dev -> puts into intterupt queue
Notes: none
*/
void runOpCode( ProcessType *process, int cycleRate)
   {
    // initialize function/variables
    OpCodeType *opCode;
    int cycles;

    // get opCode
    opCode = process->programCounter;

    // set the cycles to the num remaining for opCode
    cycles = opCode->intArg2;

    // if we have preemption and not a dev command then we check if the cycles
    // exceed the quantum amount
    if ( preemption && cycles > quantumCycles &&
                             compareString( opCode->command, "dev" ) != STR_EQ )
      {
       // update cycles to quantum cycles
       cycles = quantumCycles;
      }

    // update the amount of cycles left for opCode
    opCode->intArg2 = opCode->intArg2 - cycles;

    // calculate total time
    int totalTime = (cycles * cycleRate);

    // update and grab time
    double time = accessTimer( LAP_TIMER, timeStr );

    // update the remaining time for process
    process->remainingTime = process->remainingTime - totalTime;

    // if no preemption or if there is make sure no dev command
      // function : compareString
    if ( compareString( opCode->command, "dev" ) != STR_EQ || ! preemption )
       {
        // run the timer
         // function: runTimer
        runTimer( totalTime );

        // set op end time and update time
           // function : accessTimer
        opCode->opEndTime = accessTimer( LAP_TIMER, timeStr );
       }

    else
       {
        // initialize vars
        InterruptData *newNode;
        double totalSec = (double) totalTime / 1000;

        // allocate space for new node
           // function : malloc
        newNode = (InterruptData *)malloc( sizeof( InterruptData ) );

        // assign data to new node
        newNode->blockedEndTime = time + totalSec;

        newNode->process = process;

        newNode->serviced = false;

        newNode->nextInterrupt = NULL;

        // if first interrupt assign to head
        if ( intHeadRef == NULL )
           {
             intHeadRef = wkgIntRef = newNode;
           }
        // othewise append to end
        else
           {
            wkgIntRef->nextInterrupt = newNode;

            wkgIntRef = wkgIntRef->nextInterrupt;
           }
       }

   }

/*
Function Name: runSim
Algorithm: master driver for simulator operations;
           conducts OS simulation with varying scheduling strategies and varying
           numbers of processes
Precondition: given head pointer to config data and meta metaDataPtr
Postcondition: simulation is provided, file output as configured
Exceptions: none
Notes: none
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
   {
    // Initialize Function/Variables
    int cpuSchedulingMode= configPtr->cpuSchedCode;

    logMode = configPtr->logToCode;
    logFileName = configPtr->logToFileName;

    ProcessType *pcbPtr = NULL;
    ProcessType *currentProcess;
    ProcessType *interruptResult;
    OpCodeType *opCode;

    bool procSwitchFlag = false;
    bool runFlag = true;

    // int displayFlag;

    // flag so program knows if there is preemption
    preemption = ( cpuSchedulingMode == CPU_SCHED_FCFS_P_CODE ) ||
                   ( cpuSchedulingMode == CPU_SCHED_RR_P_CODE ) ||
                 ( cpuSchedulingMode == CPU_SCHED_SRTF_P_CODE );

    quantumCycles = configPtr->quantumCycles;
    int procCycleRate = configPtr->procCycleRate;
    int ioCycleRate = configPtr->ioCycleRate;

    char dispMsg[ MAX_STR_LEN ];

    MMU *mmu = (MMU *)malloc( sizeof( MMU ) );

    int idleCycle = 0;

    mmu->sysMemory = NULL;
    mmu->memAvailable = mmu->memTotal = configPtr->memAvailable;

    mmu->numProc = 0;

    // if not SJF or SRTF then allow process changed
    if ( cpuSchedulingMode == CPU_SCHED_SJF_N_CODE ||
                                    cpuSchedulingMode == CPU_SCHED_SRTF_P_CODE )
       {
        procSwitchFlag = true;
       }

    //build pcb linked list from Op Code LL
    pcbPtr = buildPCB( metaDataMstrPtr , configPtr );

    // start the timer
    accessTimer( ZERO_TIMER, timeStr );

    // diplay start of system
       // function: manageDisplay
    manageDisplay( pcbPtr , SYS_START, NULL );

    // set all process to ready
       // function: setState
    setState( pcbPtr, READY, true );

    // set current process the the pcb head
    currentProcess = pcbPtr;

    // check if logging to file only
    if ( logMode == LOGTO_FILE_CODE )
       {
        // print using printf since manageDisplay wont print to screen if log
        // mode is FILE only
        printf("LOGGING TO FILE ONLY THIS WILL TAKE SOME SECONDS\n\n");
        // printf("Memory Map will be printed to monitor and not to log file\n\n");
       }

    // master loop
    while ( processCounter > 0 && pcbPtr != NULL && runFlag )
       {
        // find next process
           // function: findNextProcess
        currentProcess = findNextProcess( currentProcess, pcbPtr,
                                            cpuSchedulingMode, procSwitchFlag );

        // update procSwitchFlag
        procSwitchFlag = false;

        // Check if idling
        // findNextProcess will return null if idling
        if ( currentProcess != NULL )
           {
            // reset DisplayFlag
            // displayFlag = MEM_FAILURE;

            // find next op code
               // function: findNextOpCode
            opCode = findNextOpCode( currentProcess );

            // update time
               // function: accessTimer
            accessTimer( LAP_TIMER, timeStr );

            // check for input
            if ( compareString( opCode->inOutArg, "in" ) == STR_EQ )
               {
                // set display string to appropriate message
                   // functions: copyString, concatenateString
                copyString( dispMsg, opCode->strArg1);
                concatenateString( dispMsg, " input operation start" );

                // diplay operation info
                   // function: manageDisplay
                manageDisplay( currentProcess, OP_COMMAND, dispMsg );

                // process the op code
                   // function: runOpCode
                runOpCode( currentProcess, ioCycleRate );

                if ( preemption )
                  {
                   // set state to BLOCKED
                    // function: setState
                   setState( currentProcess, BLOCKED, false );

                   // set display string to appropriate message
                      // functions: copyString, concatenateString
                   copyString( dispMsg, "input operation");

                   // diplay operation info
                      // function: manageDisplay
                   manageDisplay( currentProcess, GOTBLOCKED, dispMsg );

                   procSwitchFlag = true;
                  }
                else
                  {
                   // set display string to appropriate message
                     // functions: copyString, concatenateString
                  copyString( dispMsg, opCode->strArg1);
                  concatenateString( dispMsg, " input operation end" );

                  // diplay operation info
                     // function: manageDisplay
                  manageDisplay( currentProcess, OP_COMMAND , dispMsg );
                  }
               }

            // check for output
            else if ( compareString( opCode->inOutArg, "out" ) == STR_EQ )
               {
                // set display string to appropriate message
                   // functions: copyString, concatenateString
                copyString( dispMsg, opCode->strArg1);
                concatenateString( dispMsg, " output operation start" );

                // diplay operation info
                   // function: manageDisplay
                manageDisplay( currentProcess, OP_COMMAND, dispMsg );

                // process the op code
                   // function: runOpCode
                runOpCode( currentProcess, ioCycleRate );

                if ( preemption )
                  {
                   // set state to BLOCKED
                    // function: setState
                   setState( currentProcess, BLOCKED, false );

                   // set display string to appropriate message
                      // functions: copyString, concatenateString
                   copyString( dispMsg, "output operation");

                   // diplay operation info
                      // function: manageDisplay
                   manageDisplay( currentProcess, GOTBLOCKED, dispMsg );

                   procSwitchFlag = true;
                  }
                else
                  {
                   // set display string to appropriate message
                     // functions: copyString, concatenateString
                   copyString( dispMsg, opCode->strArg1);
                   concatenateString( dispMsg, " output operation end" );

                   // diplay operation info
                     // function: manageDisplay
                   manageDisplay( currentProcess, OP_COMMAND , dispMsg );
                  }
               }

            // check for memory
            if ( compareString( opCode->command, "mem" ) == STR_EQ )
               {
                /*
                 // update time
                    // function: accessTimer
                 accessTimer( LAP_TIMER, timeStr );

                 // diplay operation info
                   // function: manageDisplay
                 manageDisplay( currentProcess, MEM_START, NULL );

                 // check if allocation or access
                 if ( compareString( opCode->strArg1, "allocate" ) == STR_EQ )
                   {
                    // set flag to success if operation goes well
                    if ( manageMemory( currentProcess, mmu, ALLOCATE ) )
                       {
                        displayFlag = MEM_SUCCESS;
                       }
                   }

                 else if ( compareString( opCode->strArg1, "access" ) == STR_EQ )
                   {
                    // set flag to success
                    if ( manageMemory( currentProcess, mmu, ACCESS ) )
                       {
                        displayFlag = MEM_SUCCESS;
                       }
                   }

                 // diplay operation info
                   // function: manageDisplay
                 manageDisplay( currentProcess, displayFlag, NULL );

                 // if flag wasnt updated we know it was a failed mem op
                 if ( displayFlag == MEM_FAILURE )
                   {
                    // free mem from taken by process
                    freeMemBlock( mmu, currentProcess );

                    // set switch flag to true
                    procSwitchFlag = true;

                    // set state to EXITING
                      // function: setState
                    setState( currentProcess, EXITING, false );
                   }
                // show diagnostic Map
                  // function: printMemMap
                printMemMap( mmu );
                */
                // decided to not include memory operations
                manageDisplay( currentProcess, PRINT_MESSAGE,
                                                 "Memory operation happens\n");
               }

            // check for CPU
            else if ( compareString( opCode->command, "cpu" ) == STR_EQ )
               {
                 // set display string to appropriate message
                   // function: copyString
                copyString( dispMsg, "cpu operation start" );

                // diplay operation info
                   // function: manageDisplay
                manageDisplay( currentProcess, OP_COMMAND, dispMsg );

                // process the op code
                   // function: runOpCode
                runOpCode( currentProcess, procCycleRate );

               if ( opCode->intArg2 <= 0 )
                  {
                   // set display string to appropriate message
                   // function: copyString
                   copyString( dispMsg, "cpu operation end" );
                  }
               else
                  {
                   // set display string to appropriate message
                   // function: copyString
                   copyString( dispMsg, "cpu quantum time out\n" );
                  }

                // diplay operation info
                   // function: manageDisplay
                manageDisplay( currentProcess, OP_COMMAND, dispMsg );

               }

            // check for app end
            else if ( compareString( opCode->command, "app" ) == STR_EQ &&
                 compareString( opCode->strArg1, "end" ) == STR_EQ )
               {
                // set switch flag to true
                procSwitchFlag = true;

                // diplay process end
                   // function: manageDisplay
                manageDisplay( currentProcess, PROC_END, NULL );

                // set state to EXITING
                   // function: setState
                setState( currentProcess, EXITING, false );

                // free mem that process was using if any
                   // function: freeMemBlock
                freeMemBlock( mmu, currentProcess );
               }
           }

        // only need to step through if preemptive scheduling
        if ( preemption )
         {
          // print that the cpu is idling if first idleCycle and we dont have a
          // process (NULL)
          if ( idleCycle == 0 && currentProcess == NULL )
             {
              // diplay idle
                 // function: manageDisplay
              manageDisplay( interruptResult, IDLE, NULL);

              // increment so idleCycle no longer 0
              idleCycle++;
             }

          // check for an interrupt
            // function: interruptManager
          interruptResult = interruptManager();

          // if interrupt is not null then we have an intterupt
          if ( interruptResult != NULL && interruptResult->state != EXITING )
             {
              // update time
                // function: accessTimer
              accessTimer( LAP_TIMER, timeStr );

              // check if we are idling
              if ( currentProcess == NULL )
                 {
                  // diplay end idle
                    // function: manageDisplay
                  manageDisplay( interruptResult, ENDIDLE, NULL);

                  // set current process to the intterupt since we are idling
                  // and need a process
                  currentProcess = interruptResult;

                  // reset idleCycle since we are no longer idling
                  idleCycle = 0;
                 }
                 
              // diplay interrupt
                 // function: manageDisplay
              manageDisplay( interruptResult, INTERRUPT, NULL);

              // extra check to make only process that are blocked are set ready
              if ( interruptResult->state == BLOCKED )
               {
                // set the blocked process to ready
                  // function: setState
                setState( interruptResult, READY, false );
               }

              // set procSwitchFlag
              procSwitchFlag = false;
             }
         }
       }

    // update time
      // function: accessTimer
    opCode->opEndTime = accessTimer( LAP_TIMER, timeStr );

    // diplay system end
      // function: manageDisplay
    manageDisplay( currentProcess, SYS_END, NULL);

    // clear pcb LL and free memory
       // function: clearPCB
    pcbPtr = clearPCB( pcbPtr );

    // free memory for MMU
    free(mmu);

    // update time but not to op code and STOP IT
      // function: accessTimer
    accessTimer( STOP_TIMER, timeStr );

    // diplay simulation end
      // function: manageDisplay
    manageDisplay( currentProcess, SIM_END, NULL);

    // clear log
       // function: clearLog
    logHead = wkgLogRef = clearLog( logHead );

    // clear interrupt queue
      // function: clearInterrupts
    intHeadRef = wkgIntRef = clearInterrupts( intHeadRef );
   }

/*
Function Name: setState
Algorithm: sets state of process to state param, if allFlag is true, recursively
 sets the state of all the next processes
Precondition: process is a linked list
Postcondition: state is changed
Exceptions: none
Notes: none
*/
void setState( ProcessType *process, int state, bool allFlag )
   {
    // set previous state to current state
    process->prevState = process->state;

    // set state to state parameter
    process->state = state;

    if ( state == EXITING )
       {
        processCounter--;
       }

    // update time
       // function: accessTimer
    accessTimer( LAP_TIMER, timeStr );

    // diplay state change
      // function: manageDisplay
    manageDisplay( process, STATE_CHANGE, NULL );

    // check for allFlag and  if there is a next process
    if ( allFlag && ( process->nextProcess != NULL ) )
       {
        // recursively sets the state of all the next processes
        setState( process->nextProcess, state, allFlag );
       }
   }

/*
Function Name: stateToString
Algorithm: sets stateStr to appropriate string value depending on state param
Precondition: state is a valid state declared in ProcessStates (header)
Postcondition: stateStr contains appropriate state str
Exceptions: none
Notes: none
*/
void stateToString( int state , char *stateStr)
   {
    // assign stateStr based of state
       // function : copyString, use switch
    switch ( state )
       {
        case NEW:
           copyString( stateStr, "NEW");
           break;

        case READY:
           copyString( stateStr, "READY");
           break;

        case RUNNING:
           copyString( stateStr, "RUNNING");
           break;

        case EXITING:
           copyString( stateStr, "EXITING");
           break;

        case BLOCKED:
           copyString( stateStr, "BLOCKED");
           break;
       }
   }
