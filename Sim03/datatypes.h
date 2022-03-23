// Preprocessor directives
#ifndef DATATYPES_H
#define DATATYPES_H

// Data Types Used In Sim03

// config data structure
typedef struct ConfigDataType
   {
    double version;
    char metaDataFileName[ 100 ];
    int cpuSchedCode;
    int quantumCycles;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ 100 ];
} ConfigDataType;

// Memory Management Unit
typedef struct MemoryDataType
   {
    // pid for parent Process
    int pid;
    // memory offset
    int offset;
    // memory base
    int base;
    // Base address register
    int addressReg;
    // pointer to next block of memory
    struct MemoryDataType *nextBlock;
} MemoryDataType;

// Memory Management Unit
typedef struct MMU
   {
    // Linked list of memory "blocks"
    MemoryDataType *sysMemory;
    // Total Memory Left
    int memAvailable;
    // Number of processes using memory
    int numProc;
} MMU;

typedef struct OpCodeType
   {
    int pid;             // pid, added when PCB is created
    char command[ 5 ];   // three letter command quantity
    char inOutArg[ 5 ];  // for device in/out
    char strArg1[ 15 ];  // arg 1 descriptor, up to 12 chars
    int intArg2;         // cycles or memory, assume 4 byte int
    int intArg3;         // memory, assumes 5 byte int
                         // also non/premption indicator
    double opEndTime;    // size of time string returned from accessTimer
    struct OpCodeType *nextNode; // pointer to next node as needded
   } OpCodeType;

// Data Structure used for every process
// Every process has unique ID
typedef struct ProcessType
   {
    int pid;                          // pid, added when PCB is created
    int state;                        // process state, either NEW, READY,
    int prevState;                    // RUNNING or EXITING.

    int remainingTime;             // time remaining for process
    OpCodeType *programCounter;       // pointer to start of process in op codes
    struct ProcessType *nextProcess;  // pointer to next process
   } ProcessType;

typedef struct OperationDataNode
   {
    char lineData[ 100 ];

    struct OperationDataNode *nextNode;
   } OperationDataNode;

#endif  //DATATYPES_H
