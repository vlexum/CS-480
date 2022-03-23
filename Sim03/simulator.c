#include "simulator.h"

// int value used to keep track of the number of processes
int processCounter = 0;

int logMode;

char timeStr[ 10 ];

char *logFileName;

/*
Function Name: buildPCB
Algorithm: loops though metadata linked list and searches for "app start", if app start is seen then we add a node to the linked list
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
Function Name: clearPCB
Algorithm: loops though linked list and free each nodes memory.
Precondition: localPtr is linked list
Postcondition: all node memory, if any, is returned to OS, return pointer (head) is set to null
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
Function Name: findNextOpCode
Algorithm: sets process op code pointer to the next node
Precondition: process is a linked list
Postcondition: program counter points to the next op code
Exceptions: none
Notes: wont get to null since check in master loop
*/
OpCodeType *findNextOpCode( ProcessType *process )
   {
    // update programCounter
    process->programCounter = process->programCounter->nextNode;

    // return pointer to next opCode
    return process->programCounter;
   }

/*
Function Name: findNextProcess
Algorithm: uses cpuSchedMode to decide the next process only if flag set to true
Precondition: none
Postcondition: appropriate next process is chosen
Exceptions: none
Notes: will determine if function is necessary going forth
*/
ProcessType *findNextProcess( ProcessType *currentProcess, ProcessType *pcbHead, int cpuSchedMode, bool switchProcess )
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

               // loop to find shortest job
               while ( wkgHeadRef != NULL )
                  {
                   // determine the shortest job remaining
                   if ( wkgHeadRef->remainingTime < shortestJob->remainingTime && wkgHeadRef->state == READY )
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

        // case CPU_SCHED_SRTF_P_CODE:

        // case CPU_SCHED_RR_P_CODE:

        // case CPU_SCHED_FCFS_P_CODE:

        // defaults to FCFS_N
        default :
           // check switchProcess to see if we need to switch (process complete)
           if ( switchProcess )
              {
               // update time
               accessTimer( LAP_TIMER, timeStr );

               // display process change
                  // function: manageDisplay
               manageDisplay( currentProcess->nextProcess, PROCESS_CHANGE, NULL );

               // change the state to running
                  // function: setState
               setState(currentProcess->nextProcess , RUNNING, false );

               // return the next process
               return currentProcess->nextProcess;
              }

           // otherwise return the same process
           return currentProcess;
       }
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
    MemoryDataType *localPtr;
    MemoryDataType *nextPtr = NULL;
    MemoryDataType *prevPtr = NULL;

    localPtr = mmu->sysMemory;

    // loop through system memory
    while ( localPtr != NULL )
       {
        nextPtr = localPtr->nextBlock;

        // check if same process
        if ( localPtr->pid == process->pid )
           {

            if ( prevPtr == NULL )
               {
                mmu->sysMemory = nextPtr;
               }
            else
               {
                prevPtr->nextBlock = nextPtr;
               }

            mmu->numProc = mmu->numProc - 1;
            mmu->memAvailable = mmu->memAvailable + localPtr->offset;


            free( localPtr );
           }

        prevPtr = localPtr;
        localPtr = nextPtr;
       }
   }

/*
Function Name: manageDisplay
Algorithm: handles all printing operations for simulator, assigns aprropriate message to completeMessage using snprintf, appropriate message is decided based off the displayFlag, at the end it will be diplayed to the appropriate place using the logMode
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
                  "%s, Process: %d, failed mem %s request (%d, %d)\n%s, OS: Segmentation fault, Process %d ended\n\n",
                  timeStr,
                  process->pid,
                  process->programCounter->strArg1,
                  process->programCounter->intArg2,
                  process->programCounter->intArg3,
                  timeStr,
                  process->pid
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
        if ( ( logMode == LOGTO_BOTH_CODE || logMode == LOGTO_FILE_CODE ) &&
                                                           processCounter == 0 )
           {
            // write to the log file here
            // printf("Logging to File Here\n");
            // logging would happen here but did not get around to implementing
            // it in time
           }
       }

    // check if we need to print message and message is not null
    else if ( displayFlag == PRINT_MESSAGE && opMessage != NULL )
       {
        // assign completeMessage with appropriate message
           // function: snprintf
        snprintf( completeMessage, MAX_STR_LEN, "%s", opMessage );
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
        // did not have enough time to properly store and then write so its
        // just writes as the ops happen
        FILE *fileAccessPtr;

        fileAccessPtr = fopen( logFileName, "a" );

        fputs( completeMessage, fileAccessPtr );

        fclose(fileAccessPtr);
       }
   }

/*
Function Name: manageMemory
Algorithm: manages memory operations for the simulator
Input Parameter(s): pointer to process
Precondition(s): must have "mem" command
Postcondition(s): boolean result of mem process
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
       if( ( memBase >= wkgMemRef->base && memBase < wkgMemRef->addressReg ) || ( memAddress >= wkgMemRef->base && memAddress < wkgMemRef->addressReg ) )
          {
           // check if we are acccessing mem and inside memory block of same process
           if ( ( memAddress < wkgMemRef->addressReg &&
                  memBase >= wkgMemRef-> base ) &&
                  memFlag == ACCESS &&
                  process->pid == wkgMemRef->pid )
             {
              // Succesful mem access
              return true;
             }

           // if inside and not accessing then we have seg fault
           segFaultFlag = true;
          }

        // update mem ref and store pointer to last block
        lastBlock = wkgMemRef;
        wkgMemRef = wkgMemRef->nextBlock;
      }

    if ( memFlag == ALLOCATE && !segFaultFlag )
       {
        if ( !( memOffset > mmu->memAvailable ) )
           {
            //
            newBlock = (MemoryDataType *)malloc( sizeof( MemoryDataType ) );

            newBlock->pid = process->pid;
            newBlock->offset = memOffset;
            newBlock->base = memBase;
            newBlock->addressReg = memAddress;

            newBlock->nextBlock = NULL;

            mmu->memAvailable = mmu->memAvailable - memOffset;
            mmu->numProc = mmu->numProc + 1;

            if ( lastBlock == NULL )
               {
                mmu->sysMemory = newBlock;

                return true;
               }

            lastBlock->nextBlock = newBlock;

            return true;
          }
       }

    return false;
   }

/*
Function Name: runOpCode
Algorithm: "processes" the op code by calculating the process time and then runs timer as a thread
Precondition: none
Postcondition: timer is run for appropriate amount of time
Exceptions: none
Notes: none
*/
void runOpCode( OpCodeType *opCode, int cycleRate)
   {
    // initialize function/variables
    pthread_t inOutThread;

    int *timePtr;
    void *paramTimePtr;

    // calculate total time
    int totalTime = opCode->intArg2 * cycleRate;

    // set timePtr
    timePtr = &totalTime;

    // set paramTimePtr
    paramTimePtr = (void *) timePtr;

    // create thread to run timer
    pthread_create( &inOutThread, NULL, &runTimer, paramTimePtr );

    // join thread
    pthread_join( inOutThread, NULL );

    // set op end time and update time
       // function : accessTimer
    opCode->opEndTime = accessTimer( LAP_TIMER, timeStr );
   }

/*
Function Name: runSim
Algorithm: master driver for simulator operations;
           conducts OS simulation with varying scheduling strategies and varying numbers of processes
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
    OpCodeType *opCode;

    bool interruptFlag = false;
    bool idleFlag = false;
    bool procSwitchFlag = false;
    bool runFlag = true;
    int displayFlag;

    // int quantumCycles = configPtr->quantumCycles;=
    int procCycleRate = configPtr->procCycleRate;
    int ioCycleRate = configPtr->ioCycleRate;

    char dispMsg[ MAX_STR_LEN ];

    MMU *mmu = (MMU *)malloc( sizeof( MMU ) );

    mmu->sysMemory = NULL;
    mmu->memAvailable = configPtr->memAvailable;
    mmu->numProc = 0;

    // if not FCFS then allow process changed
    if ( cpuSchedulingMode == CPU_SCHED_SJF_N_CODE)
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

    // master loop
    while ( processCounter > 0 && pcbPtr != NULL && runFlag )
       {
        // find next process
           // function: findNextProcess
        currentProcess = findNextProcess( currentProcess, pcbPtr, cpuSchedulingMode, procSwitchFlag );

        // update procSwitchFlag
        procSwitchFlag = false;

        // reset DisplayFlag
        displayFlag = MEM_FAILURE;

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
            runOpCode( opCode, ioCycleRate );

            // set display string to appropriate message
               // functions: copyString, concatenateString
            copyString( dispMsg, opCode->strArg1);
            concatenateString( dispMsg, " input operation end" );

            // diplay operation info
               // function: manageDisplay
            manageDisplay( currentProcess, OP_COMMAND , dispMsg );
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
            runOpCode( opCode, ioCycleRate );

            // set display string to appropriate message
               // functions: copyString, concatenateString
            copyString( dispMsg, opCode->strArg1);
            concatenateString( dispMsg, " output operation end" );

            // diplay operation info
               // function: manageDisplay
            manageDisplay( currentProcess, OP_COMMAND, dispMsg );
           }

        // check for idle
        // place holder
        if ( idleFlag )
           {
            // Placeholder
            printf("In Idle\n");
           }

        // check for memory
        // place holder for now
        if ( compareString( opCode->command, "mem" ) == STR_EQ )
           {
             // update time
                // function: accessTimer
             accessTimer( LAP_TIMER, timeStr );

             // diplay operation info
               // function: manageDisplay
             manageDisplay( currentProcess, MEM_START, NULL );

             // check if allocation or access
             if ( compareString( opCode->strArg1, "allocate" ) == STR_EQ )
               {
                //
                if ( manageMemory( currentProcess, mmu, ALLOCATE ) )
                   {
                    displayFlag = MEM_SUCCESS;
                   }
               }

             else if ( compareString( opCode->strArg1, "access" ) == STR_EQ )
               {
                //
                if ( manageMemory( currentProcess, mmu, ACCESS ) )
                   {
                    //
                    displayFlag = MEM_SUCCESS;
                   }
               }

             // diplay operation info
               // function: manageDisplay
             manageDisplay( currentProcess, displayFlag, NULL );

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

           }

        // check for CPU
        if ( compareString( opCode->command, "cpu" ) == STR_EQ )
           {
             // set display string to appropriate message
               // function: copyString
            copyString( dispMsg, "cpu operation start" );

            // diplay operation info
               // function: manageDisplay
            manageDisplay( currentProcess, OP_COMMAND, dispMsg );

            // process the op code
               // function: runOpCode
            runOpCode( opCode, procCycleRate );

            // set display string to appropriate message
              // function: copyString
            copyString( dispMsg, "cpu operation end" );

            // diplay operation info
               // function: manageDisplay
            manageDisplay( currentProcess, OP_COMMAND, dispMsg );
           }

        // check for app end
        if ( compareString( opCode->command, "app" ) == STR_EQ &&
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

        // check for interrupts
        // place holder
        if ( interruptFlag )
           {
            printf("Interrupted Here\n");
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
   }

/*
Function Name: setState
Algorithm: sets state of process to state param, if allFlag is true, recursively sets the state of all the next processes
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
       }
   }
