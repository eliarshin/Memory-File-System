#include <iostream>
#include <math.h>
#include "sim_mem.h"
using namespace std;

char main_memory[MEMORY_SIZE];
int main()
{
  //  char val, val1, val2;
//sim_mem mem_sm((char*)"test", (char*)"swap_file" ,25, 50, 25,25, 25, 5);
// mem_sm.store(30,'X');
// val=mem_sm.load(30);
// mem_sm.store(41,'X');
// val1 = mem_sm.load(41);
// mem_sm.store(1,'X');
// val2 = mem_sm.load(1);
// mem_sm.store(0,'X');
// mem_sm.store(6,'W');
// mem_sm.store(10,'z');
// mem_sm.store(17,'x');
// mem_sm.store(23,'c');
// mem_sm.store(29,'v');;
// mem_sm.store(31,'b');
// mem_sm.store(36,'n');
// mem_sm.store(42,'m');
// mem_sm.store(47,'a');
// mem_sm.store(51,'s');
// mem_sm.store(58,'d');
// mem_sm.store(64,'f');
// mem_sm.store(69,'g');
// mem_sm.store(70,'h');
// mem_sm.store(75,'j');
// mem_sm.store(80,'k');
// mem_sm.store(85,'l');
// mem_sm.store(90,'q');
// mem_sm.store(95,'w');
// mem_sm.store(100,'e');
// mem_sm.store(105,'r');
// mem_sm.store(110,'t');
// mem_sm.store(115,'y');
// mem_sm.store(120,'u');
// mem_sm.store(98,'i');
// mem_sm.store(65,'o');
// mem_sm.store(24,'p');
// mem_sm.store(51,'g');
// val = mem_sm.load (11);
// val = mem_sm.load (32);
// val = mem_sm.load (54);
// val = mem_sm.load (64);
// val = mem_sm.load (27);
// val = mem_sm.load (32);
// val = mem_sm.load (23);
// val = mem_sm.load (120);
// val = mem_sm.load (180);
// val = mem_sm.load (55);
// val = mem_sm.load (13);
// val = mem_sm.load (89);
// val = mem_sm.load (85);
// val = mem_sm.load (48);
// val = mem_sm.load (29);
// val = mem_sm.load (41);
// val = mem_sm.load (78);
// val = mem_sm.load (73);

// printf("Val1 is: %c, Val2 is: %c\n", val1, val2);

sim_mem mem_sm((char*)"exec_file", (char*)"swap_file" ,25, 50, 25,25, 25, 5);
// mem_sm.store(41,'X');
// val1 = mem_sm.load(41);
// mem_sm.store(1,'X');
// val2 = mem_sm.load(1);
mem_sm.store(0,'a');
mem_sm.store(6,'b');
mem_sm.store(10,'c');
mem_sm.store(17,'d');
mem_sm.store(23,'e');
mem_sm.store(29,'f');;
mem_sm.store(31,'g');
mem_sm.store(36,'h');
mem_sm.store(42,'i');
mem_sm.store(47,'j');
mem_sm.store(51,'k');
mem_sm.store(58,'l');
mem_sm.store(64,'m');
mem_sm.store(69,'n');
mem_sm.store(70,'o');
mem_sm.store(75,'p');
mem_sm.store(80,'q');
mem_sm.store(85,'w');
mem_sm.store(90,'r');
mem_sm.store(95,'t');
mem_sm.print_memory();
mem_sm.print_swap();
mem_sm.print_page_table();

//    char* exe = (char*)"test";
//     char * swap = (char*)"swap_file";
//     sim_mem test(exe, swap, 25,50,25,25,25,5);
//     // test.load(0);
//     // test.store(26,'r');
//     // test.store(45,'q');
//     // test.store(45,'w');
//     // test.load(1);
//     // test.store(200,'c');
//     // // test.store(98,'X');
//     // // test.store(98,'W');
//     // // test.load(98);
    
    
    // test.print_swap();
    // test.print_memory();
    // test.print_page_table();

    return 0;


}
