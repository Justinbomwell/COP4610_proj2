# COP4610_project_2
README

Team Members and Responsibilities:
    
    Conrad Horn:
      • Part 3 - System Calls
      • Part 3 - Proc (open, read, release)
  
    Justin Bomwell:
      • 
      • Scheduling Code
      • 

    Scott Forn:
      • Part 1
      • Part 2
      
      
      NOTE: As Part 3 was completed on a single computer, the processes and functional development/coding was a group effort of 3 students huddled around a computer, for three days.
  

  
Tar Archive Contents:

  Part1:
    
    empty.c:  
      • Empty C program, for strace analysis.
  
    part1.c:
      • C file that contains ONLY the <unistd.h> library
      • Accesses "/home" 5 times, so that strace can show the difference in the number of system calls made.
  
    executionProcessing.c:
      • Define Normal Execute function
      • Define Background Processesing Execution
      • Define Built Ins
  
    empty.log:
      • File containing a log of all of the system calls executed. (67 of them)
  
    part1.log:
      • File containing a log of all of the system calls executed. (72 of them)
      • Shows the 5 "access("/home", F_OK)                   = 0" calls at the end of the file.
    
    makefile:
      • Compiles empty.c and part1.c
      • Executes strace on empty.x and part1.x to create empty.log and part1.log
      
    Compilation Instructions:
      • To compile the project, move to the working directory that contains our Part1 folder.
      • Type "make" into the command line. This will compile the files and run strace, pushing the results to the .log files. 
      • To clean the directory, type "make clean" into the command line.
      • This will remove the *.o and *.log files.
  
  





  
  Part2:
    
    my_timer.c:
      • Prints prompt
      • Parses instructions
      • Shorcut Resolution function
      • Resolves Environmental Variables
      • Path Resolution function
      • Input/Output Redirection function
      • Piping functions
      • Function to implement the other functions and runs shell in main
  
    Makefile:
      • Creates my_timer.o in /lib/modules/

    Compilation and Running Instructions:
      • To compile the project, move to the working directory (usr/src) that contains the Part2 Folder.
      • Enter the folder
      • Type "sudo make" into the command line
      • Type "sudo insmod my_timer.ko" to insert the kernel module, followed by "lsmod|grep my_timer", to verify that the module was inserted correctly.
      • Run the "cat /proc/timed" command, to output the current time.
      • Running this command repeatedly will output the current time followed by the time since the last proc call.
      • Type "sudo rmmod my_timer.ko" to remove the kernel module, followed by "lsmod|grep my_timer", to verify that the module was removed correctly. NOTE: "cat /proc/timed" will now issue an error, because the module no longer exists.
      • To clean the directory the shell is running in, type "sudo make clean" into the command line.
      • This will remove all but the my_timer.c and Makefiles.
  
  





  
  Part3:
  
    elevatorModule // Directory placed in /usr/src/linux-4.19.98
      elevator.c:
        • Function defenitions for system calls start_elevator, stop_elevator, and issue_request
        • Creates proc/elevator 
        • Initializes module
        • Scheduling for elevator within kthread
  
      Makefile:
        • Makes the directory as a module
  
    SystemCalls // Directory placed in /usr/src/linux-4.19.98    
      issue_request.c:
        • Defines system call for issuing a request
        • links the system call to the kernel
        
      start_elevator.c:
        • Defines system call for starting elevator
        • links the system call to the kernel
        
      stop_elevator.c
        • Defines system call for stoping elevator
        • links the system call to the kernel
        
      Makefile: 
        • Compiles the system call declarations
        
    elevator // Directory placed in home
      producer.c:
        • issues random requests to the elevator
        
      consumer.c:
        • starts and stops the elevator
        
      wrappers.h:
        • links the issue, start and stop functions from producer/consumer to system calls
        • calls the specified system call
        
      Makefile:
        • make insert - inserts module/makes proc/elevator
        • make start - starts elevator
        • make stop - stops elevator
        • make remove - removes module
        
      Compilation Instructions:
      • sudo make insert // in the elevator directory, uses insmod to insert module
      • sudo make issue // adds people to floors waiting to be picked up, can call any time while module inserted
      • sudo make start // starts elevator scheduling
      • sudo make stop // stops elevator scheduling
      • sudo make remove // removes elevator module using rmmod
      
  
  ////////////////////////////////////////////////////////////////////////////////////////////
    
    
Known Bugs/Unfinished Portions: 
  1. Elevator waits 2*time on the top and bottom floors. (4 secs instead of 2)
    - Can be fixed by adding conditional statements on top and bottom floors for scheduling
  2. Does not go into idle state when elevator empty
    - Can add this with an extra if statement in scheduling 

Special Considerations: 
  1. In part 3 system calls need to be implemented which means the whole kernel must be recompiled before they will work on this section
