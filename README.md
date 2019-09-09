
# Shell Implementation (myshell)

## Background
Any shell implementation largely involves in initializing the *environment*, *parsing* the command and *execution* of commands. A shell can support variety of features ranging from basic to advanced. 

## Features implemented for this shell implementation

- **1.** Regular commands (***ls -al, date, echo etc.***)
- **2.** Builtin commands (***cd, exit***)
- **3.** Multiple Pipes (***|***)
- **4.** Redirection (***>***)
- **5.** History (***history***)
- **6.** Prompt Change (***$*** for regular user and ***#*** for root)
- **7.** Environment variables
- **8.** Init config file (***.myrc***)
- **9.** Default apps mappings for xdg-open (eg: ***mp4***)

## Executed Testcases

### Comands
- *basic* :  ls -al , ps -a, pwd, date, cal, etc
- *builtin*: cd, exit, clear
- *others* : vi etc.

### Pipe related test cases
- *Single pipe* :  ls -al | wc -l
- *Multiple pipes* : ls -al | uniq | sort | wc -l

### Redirection
- Redirect ls -al > a.txt 
- ls -al | sort | uniq | wc -l > a.txt

### History
- *history*
- *HISTSIZE* configurable in  ***.myrc***

### Aliases
- alias command support
- aliases can be configured in .myrc

### Default App Provision
- xdg-open for *mp4* files
- This framework can be easily extended to other formats

### Prompt Changes
- Prompt display to differentiate between root user and normal user
- ***root#*** (root user)
- ***ratnakar$*** (normal user)

### References
- Advanced Programming in the Unix Environment by W. Richard Stevens & Stephen A. Rago
- Beginning Linux Programming by Nei Maththew & Richard Stones
- Musings on IPC by Momchil Velikov
- The Linux Programming Interface by Michael KerrisK


