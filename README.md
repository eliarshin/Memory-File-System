# Memory-File-System
Memory File System Implementation C++ 
About the code :

The program represent a simulation of paging structrue in memory management of opearating system.
We virtualize how the operation system memory works, we got the main memory, swap file and fd program and also
the page table.
In our project we show what we do in several cases of memory paging, such update information from program 
to the ram and update it in the page table, and also using swap when the ram is full.
We got function of Load and Store, load from data and store into it, each one got itself permissions.
More than it, every time we store or load we update our page table same as operating system
and by that we manage our pages directions.
The swap file can be created or it creates automaticly.
the Program fd need to be created and loaded.
our main_memory initialized with '0' as built in and it got his size (100).
Page_table represent the following about functions we did and the changes on memory,
we got in our struct array:
Valid - does it valid or not in mm
Dirty - does it dirty (written two times or more in the same place)
Permission - what permissions it have, read or read + write
frame - what frame it takes in the main memory.

How to use:
The commands that the program support is:
Load(int x) - load from the pm
Store(int x,char c) - store into pm - what location and what information
void print_memory(); - to show our main memory situation
void print_swap (); - to print our swap table
void print_page_table(); - to print our page table



How to compile:
Use Visual Studio Code
1)create text file named exec_file.txt and fill or not fill it..
2)press CTRL+SHIFT+B
3)press CTRL+F5 / go to Run -> Run without debugging

Input:
In big we virtualize how memory works so we represent "memory follow" with our information
1)From exec_file.txt
2)Things we store/load in the main.cpp

Output:
1)We recive SWAPFILE which represents our swap table and what saved inside
2)we recieve Pysichal MEMORY which represent our RAM - what are in the pysichal memory right now
3)We recieve PAGE_TABLE that represents us the following about our memory, like which frames are valid and their permissions


