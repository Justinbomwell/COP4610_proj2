# COP4610_project_2
README

Team Members and Responsibilities:
  
  Conrad Horn:
    • Part 3 - System Calls
    • Part 3 - Proc (open, read, release)
  
  
  Justin Bomwell:
    • I/O Redirection
    • Pipes
    • 

  
  Scott Forn:
  
  • Part 1
  
  • Part 2
  
  //////////////////////////////////////////////////////////////////////////////////////////

  
Tar Archive Contents:
  PART1:
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
    • To compile the project, move to the working directory that contains our opened .tar file
    • Type "make" into the command line 
    • To clean the directory the shell is running in, type "make clean" into the command line.
    • This will remove the .o files and the "shell" executable.
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  
  
  PART2:
    my_timer.c:
      • Prints prompt
      • Parses instructions
      • Shorcut Resolution function
      • Resolves Environmental Variables
      • Path Resolution function
      • Input/Output Redirection function
      • Piping functions
      • Function to implement the other functions and runs shell in main
  
    makefile:
      • Create my_timer.o in /lib/modules/

   Compilation Instructions:
    • To compile the project, move to the working directory that contains our opened .tar file
    • Type "make" into the command line 
    • To clean the directory the shell is running in, type "make clean" into the command line.
    • This will remove the .o files and the "shell" executable.
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  
  
  PART3:
    elevator.c:
      • 
      • 
      • 
      • 
      • 
      • 
      • 
      • 
  
    Makefile:
      • 
  
    Compilation Instructions:
      • To compile the project, move to the working directory that contains our opened .tar file
      • Type "make" into the command line 
      • To clean the directory the shell is running in, type "make clean" into the command line.
      • This will remove the .o files and the "shell" executable.

  
  ////////////////////////////////////////////////////////////////////////////////////////////
    
    
Known Bugs/Unfinished Portions: 
1.  Elevator waits 2*time on the top and bottom floors. (4 secs instead of 2)
   

Special Considerations: 
