#ifndef SIM_MEM_H
#define SIM_MEM_H
#define MEMORY_SIZE 100
extern char main_memory[MEMORY_SIZE];

//create page_descriptor struct
typedef struct page_descriptor 
{
unsigned int V; // valid 
unsigned int D; // dirty  
unsigned int P; // permission 0 is read only
unsigned int frame; //the number of a frame if in case it is page-mapped 
} page_descriptor;

class sim_mem {
    int swapfile_fd; //swap file fd
    int program_fd;  //executable file fd
    int text_size; 
    int data_size;
    int bss_size; 
    int heap_stack_size; 
    int num_of_pages;
    int page_size;  
    page_descriptor *page_table;       //pointer to page table
    int *freeFrames;     //array to update which frames are free
    int *framesQueue;

  //functions
    public:
        sim_mem(char exe_file_name[], char swap_file_name[], int text_size,int data_size, int bss_size, int heap_stack_size,int num_of_pages, int page_size);
        ~sim_mem();
        char load(int address);
        void store(int address, char value);
        void print_memory();
        void print_swap ();
        void print_page_table();
        int frameCheck(); // check free frame
        int swapCheck(int page); // make swap
        int pop(); // pop from queue
        int push(int frame); // push into queue
};
#endif

