# Shell Program
## Features
The shell program runs a simple loop to *extract the current working directory* everytime. In each loop, the program prompts for user input and then executes the command given by the user through by creating a different process through fork for mkdir, rm, rmexcept, ls, touch, and <program_name> commands. It is done using the **fork()** function, to create a child process for the purpose.
The following commands are provided in the shell program:
> ls - Lists all files and folders in the current directory

> rm - removes specified files and folders. Multiple files/folders are allowed. **-r**, **-f**, **-v** functionalities are also supported.

> rmexcept - removes all files and folders except those which are given as arguments. Also supports **-r**, **-v**, and **-f** 
functionalities. Usage - *rmexcept* or *rmexcept <list_of_files>*

> touch - creates an empty file with given argument following it as the name. Usage - *touch <name>*

> mkdir - creates an empty directory with the specified name. Usage - *mkdir <name>*

> history - lists last n recent commmands executed. Usage - *history n* or *history*

> issue - issues nth recent command once again. Usage - *issue n*

> <program_name> - creates a child process that runs the given program (for a specific number of seconds if specified). Usage - *<program_name>* or *<program_name> n*.
