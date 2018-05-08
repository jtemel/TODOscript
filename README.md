# TODOscript

## Abstract
Generate a TODO_t/ directory containing all of the TODOs, FIXMEs and OPTIMIZEs in the project folder. Each file in TODO_t/ contains
all the TODOs, FIXMEs and OPTIMIZEs for the subdirectory with the corresponding name. Invalid file types or those that do not contain
TODOs or FIXMEs will be ignored. Consecutive line comments and block comments containing a TODO, FIXME or OPTIMIZE will be parsed from the keyword to the end of the comment (or chain of comments for line comments).

## Supported Flags
  **-p "PATH"**: Use the path defined by "PATH" as the root folder for the project. PATH must be an absolute path to the project
             folder and does **not** contain a trailing "/".  
  **No Flags**: If not flag is specified, the TODOscript will assume the directory it is in as the projects root folder.

## Usage
Navigate to the directory containing the TODOscript, and invoke the script by typing:
![](images/run_todo.png)  
If the directory containing the TODOscript is not the project folder, either copy the TODOscript and TODOparser.cpp to the desired folder or type:  

![](images/run_todo_pflag.png)

where "/home/jonathon/Desktop/SomeProject" is the absolute path to the projects folder.

## How it Works
Once the TODOscript is invoked, it iterates through every file in the given folder, then recurses on each subdirectory.
When a valid file is encountered, it is piped to the TODOparser, which scans the file and locates all the comments. When a comment is encountered, the TODOparser locates the occurence of a keyword, writing appropriate lines to the corresponding TODO_t/*.txt.

## Valid File Types
All of the following file types are supported:  
  -C/C++  
  -Java  
  -BASH  
  -HTML  
  -CSS  
  -JS  
  -Python  
  -LaTeX  
