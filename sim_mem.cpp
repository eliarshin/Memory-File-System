/*******************************Includes & Define****************************************/
#include "sim_mem.hh"
#include <utility>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <queue>

using namespace std;
char main_memory[MEMORY_SIZE];            //Global char array main_memory
queue<pair<int,int>> free_frames;         //Global queue free_frame holding pair (frame, page)
queue<pair<int,int>> used;                //Global queue used holding pair (frame, page)

/*******************************Make_Free_Farme_Method**********************************/

void sim_mem::make_free_frame(int page){    //make_free_frame method is a method that freeing up memory space if the memory is full
    if(free_frames.empty() == true){        //If there is no more free frames
        pair<int,int> used_frame = used.front();
        used.pop();                         //Pop the first frame that used (for FIFO)
        int pageU = used_frame.second;      
        if(page_table[pageU].D == 0){       //If the Dirty bit of the page that sat in the frame that we want to free is equal to 0
            page_table[pageU].V = 0;        //Change his valid -> 0
            page_table[pageU].frame = -1;   //Change his frame -> -1
            used_frame.second = -1;
            for(int i = 0; i < page_size; i++){     //Zeros the main memory
                main_memory[used_frame.first*page_size + i] = '0';
            }
            free_frames.push(used_frame);   //Push that frame to free_frame
        }
        else{                               //If the Dirty bit of the page that sat in the frame that we want to free is equal to 1
            char buf[page_size];
            for(int i = 0; i < page_size; i++){ //Copy the main_memory to buf 
                buf[i] = main_memory[page_table[pageU].frame*page_size+i];
            }
            lseek(swapfile_fd, pageU*page_size, SEEK_SET);  //Set the pointer in the file to the correct position
            write(swapfile_fd, buf, page_size);             //Write buf to the file 
            page_table[used_frame.second].V = 0;            //Change his valid -> 0
            page_table[used_frame.second].frame = -1;       //Change his frame -> -1
            for(int i = 0; i < page_size; i++){             //Zeros the main memory
                main_memory[used_frame.first*page_size + i] = '0';
            }
            free_frames.push(used_frame);                   //Push that frame to free_frame
        }
    }
}

/***********************************Consturctor*****************************************/

sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size,
    int data_size, int bss_size, int heap_stack_size,
    int num_of_pages, int page_size){                                   //Constructor for sim_mem
    this->program_fd = open(exe_file_name, O_RDONLY, 0);                //Open program_fd name as exe_file_name
    if(program_fd < 0){                                                 //If the file can't open exit the program
        perror("Program file doesn't exist");
        exit(1);
    }
    this->swapfile_fd = open(swap_file_name, O_CREAT| O_RDWR, 0777);    //Open swapfile_fd name as swap_file_name, if there is no file like this, make a new one with file permission 0777 (read,write,exc)
    if(swapfile_fd < 0){                                                //If the file can't open exit the program
        perror("Error by open Swap File");
        close(program_fd);
        exit(1);
    }
    int size = page_size * num_of_pages;
    for(int i = 0; i < MEMORY_SIZE; i++){                               //Set all the main_memory to be 0
        main_memory[i] = '0';
    }
    char* zeros = (char*)malloc(sizeof(char)*size);
    for(int i = 0; i < size; i++){
        zeros[i] = '0';
    }
    write(swapfile_fd, zeros, size);                                    //Set all the swap memory to be 0
    free(zeros);
    this->page_table = (page_descriptor*)malloc(sizeof(page_descriptor) * num_of_pages);    //Set for page_table to be sizeof(page_descriptor) * num_of_pages size
    if(this->page_table < 0){                                                               //If there is a malloc Error
        perror("malloc failed");
        close(program_fd);
        close(swapfile_fd);
        exit(1);
    }
    for(int i = 0; i < num_of_pages; i++){                               //Set page_table data
        page_table[i].V = 0;
        page_table[i].frame = -1;
        page_table[i].D = 0;
        page_table[i].P = 1;
    }
    for(int i = 0; i < text_size/page_size; i++){                       //Set the part of the code to be with permission 0
        page_table[i].P = 0;
    }
    for(int i = 0; i < MEMORY_SIZE/page_size; i++){                     //Set the queue for free_frame
        pair<int,int> toPush;
        toPush.first = i;
        toPush.second = -1;
        free_frames.push(toPush);
    }
    this->text_size = text_size;                                        //Initialize data
    this->data_size = data_size;                                        //Initialize data
    this->bss_size = bss_size;                                          //Initialize data
    this->heap_stack_size = heap_stack_size;                            //Initialize data
    this->num_of_pages = num_of_pages;                                  //Initialize data
    this->page_size = page_size;                                        //Initialize data
}

/************************************Disturctor*****************************************/

sim_mem::~sim_mem(){        //Destructor for sim_mem
    free(page_table);
    close(swapfile_fd);
    close(program_fd);
}

/***********************************Load_Method*****************************************/

char sim_mem::load(int adress){                         //Load Method
    int page = adress/page_size;
    int offset = adress%page_size;
    if(page > num_of_pages || page < 0){                //If the logical adress impossible
        cout << "not available adress" << "\n";
    }
    else if(page_table[page].V == 1){                   //If valid = 1 return from the main memory
    }
    else if(page_table[page].V == 0 && page_table[page].P == 0){       //If valid = 0 && permission = 0 read from logical memory to main memory. return from the main memory
        char buf[page_size];
        lseek(program_fd, page*page_size, SEEK_SET);    //Set the pointer in the file to the correct position
        int fd = read(program_fd, buf, page_size);      //Read from file to buf 
        if(fd < 0){                                     //If read fail
            free(page_table);
            close(swapfile_fd);
            close(program_fd);
            exit(1);
        }
        make_free_frame(page);                          //Check if there are available frame
        pair<int,int> pushTo;                           //New pair (frame,page)
        pushTo = free_frames.front();
        pushTo.second = page;
        page_table[page].frame = pushTo.first;
        page_table[page].V = 1;                         //Set valid = 1
        free_frames.pop();                      
        used.push(pushTo);                              //Push the used frame to used queue
        for(int i = 0; i < page_size; i++){             //Copy from buf to main memory
            main_memory[page_table[page].frame*page_size+i] = buf[i];
        }
    }
    else if(page_table[page].V == 0 && page_table[page].P == 1 && page_table[page].D == 0){       //If valid = 0, permission = 1 && Dirty bit = 0 this is data from heap/stuck/bss. so there is no available data
        if(page >= (text_size/page_size) && page < ((text_size+data_size)/page_size)){
            char buf[page_size];
            lseek(program_fd, page*page_size, SEEK_SET);    //Set the pointer in the file to the correct position
            int fd = read(program_fd, buf, page_size);      //Read from file to buf 
            if(fd < 0){                                     //If read fail
                free(page_table);
                close(swapfile_fd);
                close(program_fd);
                exit(1);
            }
            make_free_frame(page);                          //Check if there are available frame
            pair<int,int> pushTo;                           //New pair (frame,page)
            pushTo = free_frames.front();
            pushTo.second = page;
            page_table[page].frame = pushTo.first;
            page_table[page].V = 1;                         //Set valid = 1
            free_frames.pop();                      
            used.push(pushTo);                              //Push the used frame to used queue
            for(int i = 0; i < page_size; i++){             //Copy from buf to main memory
                main_memory[page_table[page].frame*page_size+i] = buf[i];
            }
            return main_memory[(page_table[page].frame*page_size) + offset]; //Return from main memory
        }
        else{
            cout << "Not a valid data" << "\n";
            return '\0';
        }
    }
    else if(page_table[page].V == 0 && page_table[page].P == 1 && page_table[page].D == 1){       //If valid = 0, permission = 1 && Dirty bit = 1. read from swap to main memory. return from the main memory
        char buf[page_size];
        lseek(swapfile_fd, page*page_size, SEEK_SET);     //Set the pointer in the file to the correct position 
        int fd = read(swapfile_fd, buf, page_size);       //Read from file to buf 
        if(fd < 0){                                       //If read fail
            free(page_table);
            close(swapfile_fd);
            close(program_fd);
            exit(1);
        }
        make_free_frame(page);                          //Check if there are available frame
        pair<int,int> pushTo;
        pushTo = free_frames.front();
        pushTo.second = page;
        page_table[page].frame = pushTo.first;
        page_table[page].V = 1;                         //Set valid = 1
        free_frames.pop();
        used.push(pushTo);                              //Push the used frame to used queue
        for(int i = 0; i < page_size; i++){             //Copy from buf to main memory
            main_memory[page_table[page].frame*page_size+i] = buf[i];
        }
    }
    return main_memory[(page_table[page].frame*page_size) + offset]; //Return from main memory
}

/***********************************Store_Method****************************************/

void sim_mem::store(int adress, char value){                         //Load Method
    int page = adress/page_size;
    int offset = adress%page_size;
    if(page > num_of_pages || page < 0){                             //If the logical adress impossible
        cout << "not available adress" << "\n";
        return;
    }
    else if(page_table[page].P == 0){                                //If permission = 0. there is no able to write to a read file
        cout << "permission denied" << "\n";
        return;
    }
    else if(page_table[page].P == 1 && page_table[page].V == 1){     //If valid = 1 && permission = 1 store in the main memory in the correct spot
        page_table[page].D = 1;                                      //Change the Dirty bit -> 1
        main_memory[page_table[page].frame*page_size+offset] = value;
        return;
    }
    else if(page_table[page].V == 0 && page_table[page].P == 1 && page_table[page].D == 1){       //If valid = 0, permission = 1 && Dirty bit = 1. read from swap to main memory. store in the main memory in the correct spot
        char buf[page_size];
        lseek(swapfile_fd, page*page_size, SEEK_SET);     //Set the pointer in the file to the correct position 
        int fd = read(swapfile_fd, buf, page_size);       //Read from file to buf 
        if(fd < 0){                                       //If read fail
            free(page_table);
            close(swapfile_fd);
            close(program_fd);
            exit(0);
        }
        make_free_frame(page);                          //Check if there are available frame
        pair<int,int> pushTo;
        pushTo = free_frames.front();
        pushTo.second = page;
        page_table[page].frame = pushTo.first;
        page_table[page].V = 1;                         //Set valid = 1
        page_table[page].D = 1;                         //Set Dirty Bit = 1
        free_frames.pop();
        used.push(pushTo);                              //Push the used frame to used queue
        for(int i = 0; i < page_size; i++){             //Copy from buf to main memory
            main_memory[page_table[page].frame*page_size+i] = buf[i];
        }
        main_memory[page_table[page].frame*page_size+offset] = value; //Set the corect spot to be -> value
    }
    else if(page_table[page].V == 0 && page_table[page].P == 1 && page_table[page].D == 0){       //If valid = 0, permission = 1 && Dirty bit = 0. store in any available spot in the main memory
        if(page >= (text_size/page_size) && page < ((text_size+data_size)/page_size)){
            char buf[page_size];
            lseek(program_fd, page*page_size, SEEK_SET);    //Set the pointer in the file to the correct position
            int fd = read(program_fd, buf, page_size);      //Read from file to buf 
            if(fd < 0){                                     //If read fail
                free(page_table);
                close(swapfile_fd);
                close(program_fd);
                exit(1);
            }
            make_free_frame(page);                          //Check if there are available frame
            pair<int,int> pushTo;                           //New pair (frame,page)
            pushTo = free_frames.front();
            pushTo.second = page;
            page_table[page].frame = pushTo.first;
            page_table[page].V = 1;                         //Set valid = 1
            page_table[page].D = 1;
            free_frames.pop();                      
            used.push(pushTo);                              //Push the used frame to used queue
            for(int i = 0; i < page_size; i++){             //Copy from buf to main memory
                main_memory[page_table[page].frame*page_size+i] = buf[i];
            }
            main_memory[page_table[page].frame*page_size+offset] = value; //Set the corect spot to be -> value
        }
        else{
            make_free_frame(page);                          //Check if there are available frame
            pair<int,int> pushTo;
            pushTo = free_frames.front();
            pushTo.second = page;
            page_table[page].frame = pushTo.first;
            page_table[page].D = 1;                         //Set Dirty Bit = 1
            page_table[page].V = 1;                         //Set valid = 1
            free_frames.pop();
            used.push(pushTo);                              //Push the used frame to used queue
            main_memory[page_table[page].frame*page_size+offset] = value; //Set the corect spot to be -> value
        }
    }
}

/********************************Print_Memory_Method***********************************/

void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }
}

/*********************************Print_Swap_Method***********************************/

void sim_mem::print_swap() {
    char* str = (char*)malloc(this->page_size *sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(swapfile_fd, str, this->page_size) == this->page_size) {
        for(i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
    printf("\n");
    }
    free(str);
}

/*****************************Print_Page_Table_Method*******************************/

void sim_mem::print_page_table() {
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
    for(i = 0; i < num_of_pages; i++) {
    printf("[%d]\t[%d]\t[%d]\t[%d]\n",
    page_table[i].V,
    page_table[i].D,
    page_table[i].P,
    page_table[i].frame);
    }
}