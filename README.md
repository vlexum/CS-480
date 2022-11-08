# Simulator Project from CS480

#### THE PROGRAM CONFIGURATION DATA

All programs must be able to input and store the contents of the file shown next.

Note that any of the nine configuration lines may be in any order in the file.

However, the “Start Simulator...” and the “End Simulator...” lines will be located at

the beginning and end of the file as shown.

Start Simulator Configuration File

Version/Phase: 1.0

File Path: Test\_3.mdf

CPU Scheduling Code: NONE

Quantum Time (cycles): 55

Memory Available (KB): 12000

Processor Cycle Time (msec): 10

I/O Cycle Time (msec): 20

Log To: Monitor

Log File Path: logfile\_1.lgf

End Simulator Configuration File.

The following items specify the expected and allowed data that may be used in the

configuration file. Each has a specification of limits or conditions and if any

configuration item is outside the specified limits, the uploading process must throw

an error.

**Version/Phase:** This line will have a version number such as 1.25, 2.3, 3.44, etc.

Note that the version/phase will be different for each assignment and will be

floating point values; in many cases, student programs are likely to have evolving

fractional version numbers as the programs are developed. Specification: 0.0 ≤

V/P ≤ 10.0

**File Path:** This line must contain the file path where the meta-data will be found.

The assignment requirement is that the data must be in the same directory as the

program





**CPU Scheduling Code:** This line will hold any of the following: ***FCFS-N***, ***SJF-N***,

***SRTF-P***, ***FCFS-P***, ***RR-P***. No other code names are allowed, and if any are found,

the data access must be aborted, and the configuration function must signal

failure to the calling function. Note that the configuration input function should not

display any output – this will be discussed later.

**Quantum Time:** This line will hold an integer specifying the quantum time for the

Simulator. For the first couple of projects, this will be zero and/or will be ignored

by the program although it must still be stored in the data structure. Specification:

0 ≤ Q ≤ 100

**Memory Available:** This line will hold an integer specifying the system memory

that will be available. For the first couple of projects this may also be ignored

although it must still be stored in the data structure. Specification: 1024 ≤ MA ≤

102400 (1 MB to 100 MB in KB form)

**Processor Cycle Time (msec):** This line will hold an integer cycle time that will

specify the number of milliseconds each processor cycle will consume.

Specification: 1 ≤ PCT ≤ 1000

**I/O Cycle Time (msec):** This line will also hold an integer cycle time like the

processor cycle time. Specification: 1 ≤ IOCT ≤ 10,000

**Log To:** This line will hold one of three terms, being ***Monitor***, ***File***, or ***Both***. No

other code names are allowed, and if any are found, the data access must be

aborted, and the configuration input function must signal failure to the calling

function

**Log File Path:** This line will hold the file path of the log file, which is used if “Log

To:” has selected either ***File*** or ***Both***. It must still hold some string quantity even

if “Log To:” is set to ***Monitor*** (e.g., No\_Logfile, or None)

At the end of the configuration file, the last “End Simulator . . . “ must be found in

the configuration file exactly as shown above.





Most failure issues such as missing file, corrupted file data, or incomplete data

must stop the function and elegantly respond. This includes closing the input file if

it is open, releasing any acquired memory, halting any other processing, file I/O,

or file management, and providing an indication to the calling function as to what

went wrong. Remember that the function must communicate the error to the

calling function; error messages must all be printed from the main function.

#### THE PROGRAM META-DATA

**The program meta-data components are as follows:**

Commands: **sys**, **app**, **dev**, **cpu**, **mem**

In/Out arguments: **in**, **out**

First string argument (after In/Out where used): **start**, **end process**, **allocate**,

**access**, **ethernet**, **hard drive**, **keyboard**, **monitor**, **serial**, **sound signal**, **usb**,

**video signal**

If one integer after “start” and comma: This is the entry cycle time into the

system (optional in this project)

If one integer argument after comma: This is the cycle time for the operation

If two integer arguments after comma: These are the base and offset for memory

operations.

**Sample meta-data**

Start Program Meta-Data Code:

sys start

app start, 0

dev in, hard drive, 18

cpu process, 9

cpu process, 9

cpu process, 9

dev out, monitor, 60

app end

app start, 0





dev in, sound signal, 40

mem allocate, 2048, 4096

dev in, hard drive, 30

mem allocate, 2760, 2890

dev in, sound signal, 25

cpu process, 6

mem allocate, 3000, 4000

dev out, usb, 10

mem allocate, 3500, 3700

app end

app start, 0

dev in, video signal, 70

cpu process, 10

dev out, monitor, 70

dev in, hard drive, 18

cpu process, 9

app end

app start, 0

dev in, sound signal, 35

dev out, monitor, 100

dev in, keyboard, 50

cpu process, 9

dev out, video signal, 49

app end

app start, 0

dev in, keyboard, 90

dev out, sound signal, 40

dev out, serial, 32

cpu process, 10

dev in, hard drive, 15

app end

sys end

End Program Meta-Data Code.





#### GENERAL INFORMATION

**The cycle times are applied as specified here:**

The cycle time represents the number of milliseconds per cycle for the program.

For example, if a device has a 50 msec/cycle time (found in the configuration file),

and it is supposed to run for 10 cycles (found in the meta-data file, the device

operation (i.e., the timer for that device) must actually run for 500 mSec. An

onboard clock interface of some kind must be used to manage this, and the

precision must be to the microsecond level. To repeat, the simulator must

represent real time; if the operations take 10 seconds, the simulator must take 10

seconds.

#### SUPPORTING PROGRAM CODE

**Timing the simulator operations:**

A support file simtimer.c and its header file will be provided for student

consideration. It is not required for students to use this code, however timer

displays used for each of the assignments must correctly show the time at

microsecond precision (i.e., 0.000001 sec) as specified previously. The

microsecond display is demonstrated in the Sim01 demonstration program, which

will also be provided.

**Creating example test programs:**

The program proggen.chas been developed to support testing and work with this

assignment. It can generate test program meta-data with varying parameters,

although it does not generate memory access or allocation op codes as these need

to be uniquely created. It can also be modified as needed to use different

operations-generating algorithm(s). Besides using this program for its intended

purpose, students can also observe expected programming practices especially as

relates to readability. As noted previously in this document, comments are allowed

but not expected; program code should be eminently readable by the use of self-

documenting identifiers. That said, this code is significantly commented to support

learning.





#### RUNNING THE SIMULATOR

The simulator will input a configuration file that is accepted from the command

line, as follows:

./sim0x [-zz] config\_y.cnf

\***x** is the project number (1-4), **y** is the number of a given configuration file, and

**zz** is one or more of the three specified command line switches

***Note that the program MUST work in this form, and ONLY in this form.*** The

use of any console input actions for the configuration or meta data files will be

cause for significant credit reduction. The configuration file must be used as a

command-line argument, and the meta data file must be opened after acquiring

the meta data file name from the configuration file. Any deviation from this

requirement will cause a reduction of credit.

Also note that differing configuration files will be used for various testing purposes.





### Phase I (Sim01) – Input Data Management

#### DESCRIPTION

This phase – which is a review of data structures, implemented in C – will require

the creation of two data-acquisition operations that upload and store two sets of

data: the Simulator configuration file, and the Simulator meta-data file. It will also

provide a display representing the running simulator.

While this is a stand-alone project, students are wise to assess the next three

phases of the project so they can consider the requirements and develop their

code to be modular components of the larger system. The last project or two will

be pretty complicated but will not be difficult to develop as long as the base

components have been developed well.

IMPORTANT: As mentioned previously, no processing function should ever display

an output. The configuration and meta-data input operations are a good example.

If there is a failure in the operation/function, it should provide some form of

messaging back to the calling function so the calling function can manage the

issue, which may include displaying an error message and/or shutting down the

program. Any processing functions (i.e., functions not specifically focused on I/O

actions other than its specifications) that conduct any I/O will experience a

significant reduction of credit. As a note, the simulator function’s task is to display

simulated operations, so it is acceptable for that function, along with its

subordinate functions, to display or store output.

#### MAIN FILE/MAIN METHOD/DRIVER

One file will contain the main file for the simulator program. As mentioned

previously in this document, no other functions or operations should be in the

main file except the ***main*** function itself, and most of this function’s actions will be

to call other functions to conduct the necessary operations. The ***main*** function

should be developed to upload the configuration and meta-data files, and to

conduct the simulation process before any other code is written; this is

demonstrated in the tutorial related to this course. If implemented correctly, this

function will not change throughout this project.





#### CONFIGURATION FILE

The configuration file must be uploaded to the system as called by the main

function. Any issues with incorrect commands, in/out arguments, and string or

other arguments including specified limits for the configuration items must stop

the program and report the issue as specifically as possible.

#### META-DATA FILE

The meta-data file must also be uploaded to the system as called by the main

function and again, any incorrect or misspelled commands or string arguments, or

out of limit numerical arguments must stop the program, and report the issue as

specifically as possible.

As specified above, students will be provided a meta-data file generation program.

The meta-data acquisition component must upload ***any*** meta-data file of ***any*** size,

***any*** number of actions, ***any*** number of programs, etc., and all student programs

must work correctly on any correctly formed meta-data file.

#### ASSIGNMENT

As specified in the description, students are to develop modules that, when called,

input and store the Simulator configuration data and the Simulator meta-data. The

Sim01 program will also provide a call to the function that runs the simulator. For

purposes of this first assignment, the function will simply output “runSim called

here” to the monitor to demonstrate that the main driver program is fully

operational. Once this part of the assignment is completed, there should be no

reason to go back and modify the **main** program.

Once the modules are developed, they must be executed in a driver program and

tested with varying data to prove they are working correctly.

IMPORTANT: It will not be enough to hack together a program that seems to work.

All programs must be eminently readable since each program will be graded by

one of your peers in the class in a double-blind anonymous system. Even if your

program works – or seems to work – correctly, it will not receive full credit if it is

difficult to read and/or understand. Refer to the programming standards provided

in each project rubric as well as the example program code provided. While these

standards are not an absolute requirement, the intent (readability) of the

standards is a requirement. Also review each assignment rubric early in your





development process so you will know how your program will be graded. To

repeat: All code must be eminently readable. Use of single-letter variables, lack of

white space, lack of curly braces used for every selection or iteration statement,

etc. will be cause for loss of credit.

IMPORTANT (again): As mentioned previously in this document, the programming

quality of a 400-level course is expected here. While this Simulator project is much

easier than working with a real operating system, the programming is still non-

trivial. It is strongly recommended that students start on each of the Simulator

assignments as soon as they are posted; late starts and last-minute programming

attempts will not be successful.

SIM01 PROCESS/DISPLAY

The following command-line call using the switch **-dc** (display config) is provided

here:

\>OS\_SimDriver\_6 -dc config0.cnf

Simulator Program

\=================

Config File Display

\-------------------

Version

: 1.05

Program file name

: testfile.mdf

CPU schedule selection : SRTF-P

Quantum time

: 3

Memory Available

Process cycle rate

I/O cycle rate

Log to selection

Log file name

: 11100

: 10

: 20

: Both

: logfile\_1.lgf

Simulator Program End.





The following command-line call using the switch **-dm** (display meta-data) is

provided here:

\>OS\_SimDriver\_6 -dm config0.cnf

Simulator Program

\=================

Meta-Data File Display

\----------------------

Op Code: /pid: 0/cmd: sys/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: hard drive/arg 2: 18/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 9/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 9/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 9/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: monitor/arg 2: 60/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: sound signal/arg 2: 40/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: mem/io: NA

/arg1: allocate/arg 2: 2048/arg 3: 4096/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: hard drive/arg 2: 30/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: mem/io: NA

/arg1: allocate/arg 2: 2760/arg 3: 2890/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in





/arg1: sound signal/arg 2: 25/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 6/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: mem/io: NA

/arg1: allocate/arg 2: 3000/arg 3: 4000/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: usb/arg 2: 10/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: mem/io: NA

/arg1: allocate/arg 2: 3500/arg 3: 3700/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: video signal/arg 2: 70/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 10/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: monitor/arg 2: 70/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: hard drive/arg 2: 18/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 9/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: sound signal/arg 2: 35/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: monitor/arg 2: 100/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: keyboard/arg 2: 50/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 9/arg 3: 0/op end time: 0.000000





Op Code: /pid: 0/cmd: dev/io: out

/arg1: video signal/arg 2: 49/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: start/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: keyboard/arg 2: 90/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: sound signal/arg 2: 40/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: out

/arg1: serial/arg 2: 32/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: cpu/io: NA

/arg1: process/arg 2: 10/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: dev/io: in

/arg1: hard drive/arg 2: 15/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: app/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Op Code: /pid: 0/cmd: sys/io: NA

/arg1: end/arg 2: 0/arg 3: 0/op end time: 0.000000

Simulator Program End.

The following command-line call using the switch **-rs** (run simulator) is provided

here:

\>OS\_SimDriver\_6 -rs config0.cnf

Simulator Program

\=================

runSim called here

Simulator Program End.





Finally, note that the three command-line switches may be called in any

combination. For example, it might be called with both the **-dc** and **-dm** switches

so that it displays both the configuration file and the meta-data file. It could also

be called with all three switches (**-dc**, **-dm**, and **-rs**) and all three responses

should be correctly displayed.





### Phase 2 (Sim02) - Simple Multiple Program Batch

#### Simulator

##### DESCRIPTION

This phase will begin your real simulation work by processing several programs

(processes) in one simulator run. The simulator must conduct all of the required

operations of Sim01, and include the extensions of Sim02 specified here:

\- the simulator program must create a Process Control Block data structure for

each and every process. There are no specific requirements for PCB data

structures other than every process must have a unique process ID. Other data

quantities in the PCB are at the discretion of the programmer

\- the simulator must manage, process, and display the simulation of multiple

programs with multiple operation commands in a batch or sequential form. The

number of programs or operation commands will not be known in advance.

\- the simulator must output the simulation results a) to the monitor, b) to a file

(without displaying to the monitor) with the name specified in the configuration

file, or c) both; it is important to note again that the monitor display must occur as

the operation commands occur in real time with the appropriate time quantities

o the selection of monitor, file, or both must be made in the configuration file

o in addition, the output to file operation must all be conducted at one time,

after the simulation has been completed. This means that all the displayed

operation statements with the times, process numbers, operation descriptions,

etc. must be stored line by line until the simulation has finished, at which time

the data is output to a file. The given program may be run to observe the

structure of the logfile





o also, note that under the condition of outputting to the file only, the simulator

will not respond for a while during the time the simulation is running. You must

show some kind of indication that the simulation is running so the user won’t

think the program has failed. You may display something like, “Simulator

running for output to file only” but this display must only occur under that

circumstance

\- the simulator must be initially configured for First Come First Served – Non-

preemptive (FCFS-N). This means that if FCFS-N is shown in the configuration file,

the simulator will progress through the processes in the order they were found in

the meta-data file. At this point, any other scheduling algorithm must fail over to

FCFS-N

\- the simulator must show the time remaining when a process is placed into

RUNNING mode; this will only happen once in this batch mode but will happen

several times in Sim 04

\- the simulator must show one of four states that the process is in: new, ready,

running, or exiting

\- the simulator must now use a POSIX thread to manage the cycle time for all I/O

operations. This is not required for run operations; these may be run as normal

functions or threads at the student’s discretion. It also does not apply to the

memory management operations which may optionally be displayed as a place

holder. These operations will be handled differently in future simulation projects.

Note that students are required to create their own timer threads; as mentioned

previously in this document, no threads created in, or found in, available libraries,

such as ***sleep***, ***usleep***, ***nanosleep***, etc. may be used. For purposes of this

assignment, the simulator does not support a multi-tasking (multi-programming)

environment. For that reason, the simulator must still wait for each I/O operation

to complete before moving on to the next operation command

\- the system must report at least each of the following operation actions:

\- system start and end

\- any state change of any of the processes (e.g., ready, running, etc.)

\- any start or end of any operation command (e.g., hard drive input or

output, keyboard input, monitor output, run process actions, etc.)





An example config, metadata, and output file will be provided in BBLearn.

However, all programs should be tested with a variety of metadata files, at least

five to ten of them.

The program must also be tested with valgrind to identify any potential memory

leaks.

Finally, all functions related to the simulator operations must be placed in a file

named **simulator.c**, along with the header **simulator.h** as appropriate. Among

the prototypes and other data found in the header file, any data structure used

only in the **simulator.c** file must be defined in the **simulator.h** file. This file may

get a little long but this is an important requirement. Failure to place these

functions in this file will result in a significant loss of credit.





### Phase 3 (Sim03) – Batch Program Simulator with

##### Memory Management and CPU Scheduling

###### DESCRIPTION

This phase will offer you the opportunity to learn about memory management by

creating your own software Memory Management Unit (MMU). You will also be

extending the batch processing operations by implementing two different CPU

scheduling strategies. The simulator must conduct all of the required operations of

the previous (Sim02) simulator, with the addition of the following specifications:

\- the simulator must now be configurable for either First Come First Served – Non-

preemptive (FCFS-N) or Shortest Job First – Non-preemptive (SJF-N). This means

that if FCFS-N is shown in the configuration file, the simulator will progress

through the processes as they were found in the meta-data file. However, if SJF-N

is shown in the configuration file, the simulator will progress through the

processes in such a way that the jobs are run in order by their total operation

times from shortest operation times to longest operation times.

\- Note that this does not mean the shortest number of operations or cycles; the

actual running times for all operations must be calculated, compared, and

displayed when the process is placed into the RUNNING state. Also note that if two

or more processes have the exact same running times, they are to be scheduled

as FCFS. This is an unlikely scenario but must be considered and managed. Finally,

note that the metadata/processes must be loaded and numbered in the order they

are provided, and from there, they may be scheduled as specified. Do not reorder

the processes to satisfy the SJF scheduling.

\- Also note that it is not permitted to create separate simulation operations for the

separate scheduling. There must be only one simulation master loop that calls for

and uses the appropriate scheduling strategy





\- the simulator must continue to use a POSIX thread to manage the cycle time for

all I/O operations. This is an option but not a requirement for the CPU/run

operations which must also still be simulated using the clock times as in Sim02. It

also does not apply to the memory management operations which will be handled

as specified in the next paragraphs. Note that students are still required to create

their own timer threads; no previously created threads such as ***sleep***, ***usleep***,

***nanosleep***, etc. may be used. For purposes of this assignment, the simulator still

does not support a multi-tasking (multi-programming) environment. For that

reason, the simulator must still wait for each run and I/O operation to complete

before moving on to the next operation command.

\- the simulator must show one of four states that each current process is in: new,

ready, running, or exiting. Part of the management process for holding this data

includes a requirement to create a PCB for each process provided in the previous

phase. There is no specific requirement for what is to be stored in each PCB but

the data it holds must be specifically pertinent to the process it represents.

for the memory management, the following specifications must be followed:

\- the total memory permitted for a given simulator run will be placed in the

configuration file, as previously specified. Up to 100 MB (102400 KB) may be

specified in the configuration file

\- the process will allocate a segment of memory using the

mem allocate, 3000, 4000operation command provided previously and in the

Case displays. The first value is the memory base requested and the second

value is the offset, as described below. The difference between these values is

the memory allocation

\- The memory management processes are not that complicated in themselves

but they are too complicated to be presented in this document. See Cases 1

through 4 in the assignment information and check with the Instructor if you

have any questions. Your program running operation must display the memory

management exactly the same as is shown in the Case examples





\- finally, the system must report at least each of the following operation actions:

\- system start and end

\- any state change of any of the processes (e.g., ready, running, etc.)

\- any selection of a new process as a result of the scheduling requirement,

along with the display of the time remaining for that process

\- any start or end of any operation command (e.g., hard drive input or

output, keyboard input, monitor output, run process actions, etc.); note that

the ends of all of the I/O operations will occur at times between other

scheduled operations

\- the Memory operations as specified above and in the example Case

displays

\- if there are questions on any of the above conditions, the example Case

displays provided are the reference





Phase 4 (Sim04) – Multiprogramming Simulator

(tentative – to be updated at assignment time)

### DESCRIPTION

This phase will mark the culmination of how a multiprogramming operating system

works. The program will extend the previous programming assignments in such a

way that a user can view an operating system in action. The Sim04 system must

effectively demonstrate concurrency with reasonably correct times for running and

I/O operations. Threads may be used but are not required as long as the

concurrency requirement is met; the program must appear to run the I/O

operations in parallel with the run and housekeeping operations and as mentioned,

the times for the I/O operations returning from their work (as interrupts) must be

pretty close to the correct times. As before, all of the requirements of the previous

assignment phases must still be supported, which includes the ability to run one or

more programs with FCFS-N (i.e., first come, first served, non preemptive) and

SJF-N (i.e., shortest job first, non preemptive) scheduling strategies on any set of

given meta-data. In addition, all previous specifications still remain, such as no

use of various sleep functions, clean, readable programming code, correct

assignment file naming and management, etc. It would be a good idea to review

these before attempting this next project. New requirements are specified below.

\- threads may optionally be used for all I/O operations as needed; extra credit will

be earned for correct use of threads. For any of the new strategies, which are

FCFS-P, SRTF-P, and RR-P, the threads may be created and the program must

move on to the next available operation command. There is likely to be some

synchronization management to keep race conditions from occurring; since it is

likely the I/O threads will be updating the same data, which must be released to

the display when the thread has completed, this is a pretty clear reader/writer

problem, and it must be managed as such.

\- if threads are not used, simulated concurrency must still be represented and

displayed as if threads were used; also note that no matter what strategy is used,

an interrupt manager is required





\- the FCFS-P (i.e., first come, first served, preemptive) strategy must bring in

operation commands in order of the process entry. In addition, when a given

process is returned from being blocked, it must be placed back into the scheduling

queue in its original order. Example: In a program where there are 8 processes,

process operations 0, 1, 2, and 3 might all start with I/O, and are sent out as

threads. If process 3 returns first, it must be placed back in the scheduling queue

in order (e.g., 3, 4, 5, 6, 7) so that the next operation command of process 3 is

run next. Later when process 0 is freed, it must be placed in order (e.g., 0, 3, 5,

6, 7) so that the next operation command of process 0 is run next, and so on

\- the SRTF-P (i.e., shortest remaining time first, preemptive) strategy must find

the process with the shortest total remaining time before each operation command

is run, and run the operation command of that process next

\- the RR-P (round robin, preemptive) strategy starts the same as FCFS, however

when a process is returned either from running or from being blocked, it simply

goes back onto the end of the scheduling queue in the order it was returned

\- the P/run operation must stop after each complete individual cycle and check for

interrupts that have occurred while it was running that cycle. Example: three I/O

operation threads are running when a P/run operation requiring 7 cycles is placed

in the processing state, where the processing cycle time is 30 mSec. During the

first cycle, no interrupts occur, so the system checks for the interrupts, finds none,

and starts the second cycle. At a point 14 mSec into the next run cycle, one I/O

thread completes and sends an interrupt signal, and at 22 mSec into the same run

cycle, another I/O thread completes and sends an interrupt signal; note that these

are concurrent actions. The P/run action must complete its 30 mSec cycle but

when it checks for, and finds the two interrupt requests, the P/run process must

be placed back into the scheduling queue (appropriately, as specified previously in

this document, and with its 7-cycle requirement reduced to 5), and the two I/O

actions must be processed (e.g., each I/O completion transaction must be posted

with their correct return times, and the processes having these I/O operations

must be unblocked and appropriately placed back into the scheduling queue, etc.).

Also, if no I/O operation interrupts occur, the P/run operation must still be stopped

at the quantum time (e.g., even though it has a 7-cycle requirement, if the

quantum is 3, the P/run operation must be stopped after the third cycle, and it

must be placed back in the scheduling queue and its 7-cycle requirement must

now be reduced to 4





\- note that it is likely that two or more I/O operations may finish and drive an

interrupt while a P/run operation is being conducted; this will require some kind of

interrupt queuing management (mentioned above) for the waiting interrupt

requests

\- also note that for I/O-bound programs, many, and possibly all, of the processes

may be blocked for periods of time; the program must show the processor idling if

there are no Ready-state operation commands to be run

\- the system must report at least each of the following operation actions:

\- system start and end

\- any state change of any of the processes (e.g., ready, running, etc.)

\- any selection of a different process as a result of the scheduling

requirement; each selection of a process must include its time remaining at

that point

\- any start or end of any operation command (e.g., hard drive input or

output, keyboard input, monitor output, run process actions, etc.); note that

the ends of all of the I/O operations are likely to occur at times between

other scheduled operations

\- memory management is not required for this assignment; however extra credit

will be earned if memory management is correctly implemented. If memory

management is implemented, it must meet the requirements provided in the

specification for Sim 03


