/*******************************Includes & Define****************************************/
#include <utility>
#include <iostream>
#include <fstream> 
#include <queue>
using namespace std;

#ifndef SIM_MEM
#define SIM_MEM

#define MEMORY_SIZE 100
#define READ 0
#define WRITE 1
extern char main_memory[MEMORY_SIZE];           //Extern for char array main_memory
extern queue<pair<int,int>> free_frames;        //Extern for queue free_frame holding pair (frame, page)
extern queue<pair<int,int>> used;               //Extern for queue used holding pair (frame, page)

/************************************Struct*********************************************/

typedef struct page_descriptor
{
unsigned int V; // valid
unsigned int D; // dirty
unsigned int P; // permission
unsigned int frame; //the number of a frame if in case it is page-mapped
} page_descriptor;

/************************************Sim_mem********************************************/

class sim_mem {
    
    int swapfile_fd; //swap file fd
    int program_fd; //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    
    page_descriptor *page_table; //pointer to page table
    
    public:
    
    sim_mem(char exe_file_name[], char swap_file_name[], int text_size,
    int data_size, int bss_size, int heap_stack_size,
    int num_of_pages, int page_size);

    ~sim_mem();
    
    char load(int address);
    
    void store(int address, char value);
    
    void print_memory();
    
    void print_swap ();
    
    void print_page_table();

    void make_free_frame(int page);
};

#endif

