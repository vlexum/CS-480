// Preprocessor directives
#ifndef DATATYPES_H
#define DATATYPES_H

// global constants

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


#endif  //DATATYPES_H
