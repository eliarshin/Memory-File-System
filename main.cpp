#include "sim_mem.hh"
#include <stdio.h>

int main(){

    char* exe = "exe.txt";
    char * swap = "swap";
    sim_mem test(exe, swap, 25,25,25,25,25,5);
    test.store(98, 'x');
    test.store(29, 'a');
    test.store(31, 'b');
    test.store(38, 'c');
    test.store(40,'d');
    test.store(47,'e');
    test.load(98);
    test.load(2);
    test.load(6);
    test.load(11);
    test.load(16);
    test.load(22);
    test.store(26, 'l');
    test.store(31, 'l');
    test.store(36, 'l');
    test.store(41, 'l');
    test.store(46, 'l');
    test.store(52, 'l');
    test.load(26);
    test.store(50,'f');
    test.store(56, 'g');
    test.store(60,'h');
    test.store(67,'i');
    test.store(71,'j');
    test.load(71);
    test.store(79,'k');
    test.store(80,'l');
    test.store(88,'m');
    test.load(88);
    test.store(90,'n');
    test.store(101,'o');
    test.store(106,'p');
    test.store(111, 'q');
    test.load(111);
    test.store(116, 'r');
    test.store(121, 's');
    test.print_memory();
    test.print_page_table();
    test.print_swap();
    return 0;
}