#include "sim_mem.h"
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<queue>

//initialize data of sim mem
sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size,  int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size)
{   
    //initalize the data from sim_mem.h
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;

    //initalize program fd and swap fd
    this->program_fd = open(exe_file_name, O_RDONLY,0);
    if(this->program_fd < 0)
    {
        perror("EXE OPEN FAIL"); 
        exit(1);
    } 

    this->swapfile_fd = open(swap_file_name, O_CREAT|O_RDWR,0777);
    if(this->swapfile_fd < 0)
    {
        perror("SWAP OPEN FAIL"); 
        exit(1);
    }


    // initalize framesqueue and free frames array
    this->freeFrames = new int[MEMORY_SIZE/page_size];
    this->framesQueue = new int[MEMORY_SIZE/page_size];
    for(int i;i<(MEMORY_SIZE/page_size);i++)
    {
        freeFrames[i] = 0; // all free frames are 0
        framesQueue[i] = -1;     // all framesques values are -1
    }

    //Insert zeros to main memory
    for(int i=0;i<MEMORY_SIZE;i++)
        main_memory[i] = '0';

    //create the page table and initial all values,  
    page_table = new page_descriptor[num_of_pages];
    for(int i = 0; i < num_of_pages; i++)
    {
        page_table[i].D = 0;
        page_table[i].V = 0;
        
        if(i < text_size/page_size)
        { 
            page_table[i].P = 0;
            page_table[i].frame = -1;
        }    
        else
        {
            page_table[i].P = 1;
            page_table[i].frame = -1;
        }    
    }

    //string to fill the swap in zeros
    char fill[page_size*num_of_pages];
    for(int i = 0;i<page_size*num_of_pages;i++)    
        fill[i] = '0';

    if(write(this->swapfile_fd, fill, page_size*num_of_pages) == -1)
    {
        perror("WRITING TO SWAP FAILED"); 
        exit(1);
    }
}
//PRINT FUNCTIONS

//print memory
void sim_mem::print_memory() 
{
	int i;
	printf("\n Physical memory\n");
	for(i = 0; i < MEMORY_SIZE; i++) 
    {
		printf("[%c]\n", main_memory[i]);
	}
}

//print swap
void sim_mem::print_swap()
 {
	char* str = (char*)malloc(this->page_size *sizeof(char));
	int i;
	printf("\n Swap memory\n");
	lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
	while(read(swapfile_fd, str, this->page_size) == this->page_size) 
    {
		for(i = 0; i < page_size; i++) 
        {
			printf("%d - [%c]\t", i, str[i]);
		}
		
		printf("\n");
	}
    free(str);
}

//print page table
void sim_mem::print_page_table() 
{ 
	int i;
	printf("\n page table \n");
	printf("Valid\t Dirty\t Permission \t Frame\n");
	for(i = 0; i < num_of_pages; i++)
        printf("[%d]\t[%d]\t[%d]\t[%d]\n", page_table[i].V, page_table[i].D,page_table[i].P,page_table[i].frame);
	
}

// function that check if theres free frames in our frame array
int sim_mem::frameCheck()
{
    for(int i=0 ;i<(MEMORY_SIZE/page_size);i++)
        if(freeFrames[i] == 0) // if there is free location without frame return i = location
            return i;
    return -1; // return -1 in case of not
} 

//check in swamp and make swap
int sim_mem::swapCheck(int page)
{
    int frame = pop(); // take from FIFO
    freeFrames[frame] = 0; // update that it is 0 at the moment 
    lseek(swapfile_fd, page*page_size,SEEK_SET); // find location
    write(swapfile_fd,&main_memory[frame*page_size],page_size); // write to sw
    page_table[page].V = 0; // update valid to 0 its taken
    page_table[page].frame = -1; // no frame anymore because it went to swapfile
    return frame; // return the frame
}

// pop from our framequee array for FIFO
int sim_mem::pop()
{
    int tmp = framesQueue[0]; // take 0
    int location = MEMORY_SIZE/page_size; // the location we need
    for(int i = location-1 ; i > 0 ; i-- ) //shift everything >> by one
        framesQueue[i-1] = framesQueue[i];
    framesQueue[location-1] = -1; // update the queue
    return tmp;
}

// push into frame queue by FIFO
int sim_mem::push(int frame)
{
    for(int i;i<(MEMORY_SIZE/page_size);i++) // check if free slot if yes put FRAME into
        if(framesQueue[i] == -1)
            framesQueue[i] = frame; 
}

//LOAD Function
char sim_mem::load( int address)
{
    int page = address/page_size ; // the page to know in which page we are
	int offset = address%page_size; // the offset to know what cell needed
    int fd;

    if(page_table[page].V == 1) //The page is in the memory
    {
        int frame = page_table[page].frame;
        return(main_memory[(frame*page_size)+offset]); // return the location
    }

    if(page_table[page].P == 0) // if no such page
    {
        int frame = frameCheck(); // ask if theres free frame
        if(frame == -1) // if the function return no free frames
            frame = swapCheck(page); //frame gives from the swap in location page
        lseek(program_fd, page*page_size,SEEK_SET); // put pointer on program fd at page*pagesize
        fd = read(program_fd,&main_memory[frame*page_size],page_size);
        if(fd<0)
        {
            delete [] freeFrames;
            delete [] page_table;
            close(swapfile_fd);
            close(program_fd);
        }
        page_table[page].V = 1; // update to valid 
        page_table[page].frame = frame; // update the frame
        push(frame); // push frame in queue
        freeFrames[frame] = 1; // the frame is not free anymore
        return(main_memory[frame*page_size+offset]);  
    }

    if(page_table[page].D == 1) // if page dirty
    {
        int frame = frameCheck(); // check if free locations at frame array
        if(frame == -1) // if no free location
            frame = swapCheck(page);
        lseek(swapfile_fd, page*page_size,SEEK_SET); // look for the first pointer
        fd=read(swapfile_fd,&main_memory[frame*page_size],page_size);
        if(fd<0)
        {
            delete [] freeFrames;
            delete [] page_table;
            close(swapfile_fd);
            close(program_fd);
        }
        page_table[page].V = 1;  // update to valid = 1
        page_table[page].frame = frame; // update the frame in the page
        push(frame); // put the frame into queue
        freeFrames[frame] = 1; // free frames updated
        return(main_memory[frame*page_size+offset]);  
    }
    else
    {
        if(address < text_size+data_size) //in case we are in data
        {
            int frame = frameCheck();  // check for free frame
            if(frame == -1)
                frame = swapCheck(page);  
            lseek(program_fd, page*page_size,SEEK_SET);
            fd=read(program_fd,&main_memory[frame*page_size],page_size);
            if(fd<0)
        {
            delete [] freeFrames;
            delete [] page_table;
            close(swapfile_fd);
            close(program_fd);
        }
            page_table[page].V = 1; // update to valid frame 1
            page_table[page].frame = frame; // update the frame to the frame number
            push(frame); // put frame for FIFO
            freeFrames[frame] = 1; // free frames array at frame cell are not free anymore
            return(main_memory[frame*page_size+offset]);    
        }
        else if(address < text_size+data_size+bss_size) //BSS+DATA option
        {
            int frame = frameCheck(); // check for free frame
            if(frame == -1) // if no free frames take from swap
                frame = swapCheck(page);
            page_table[page].V = 1; // update valid 
            page_table[page].frame = frame; // update frame location
            for(int i =0; i<page_size;i++) // update the main memory
                main_memory[(frame*page_size)+i] = '0';
            return(main_memory[frame*page_size+offset]);                 
        }
        else // when Valid = 0 and Dirty = 0 so we cannot load from heap
        {
            perror("CANT LOAD FROM HEAP & STACK"); 
           // exit(1);    
        }        
    } 
}

//STORE Function
void sim_mem::store(int address, char value)
{
    int page = address/page_size ; // to find what page
	int offset = address%page_size; // to find offset location on page
    int fd;

    if(page_table[page].P == 0) // in case theres no permission
    {
        perror("READ ONLY PERMISSION"); 
       // exit(1);         
    }

    if(page_table[page].V == 1) //page in memory - valid = 1
    {
        int frame = page_table[page].frame; // take the frame from page table because its valid
        main_memory[(frame*page_size)+offset] = value; // put the value on the main memory at the right location
        page_table[page].D = 1; // now its dirty
        return;
    }

    if(page_table[page].D == 1) // if the page is dirty
    {
        int frame = frameCheck(); // free frame check
        if(frame == -1) // check if no free frames
            frame = swapCheck(page); 
        lseek(swapfile_fd, page*page_size,SEEK_SET); // put pointer on the right location
        fd=read(swapfile_fd,&main_memory[frame*page_size],page_size);
         if(fd<0)
        {
            delete [] freeFrames;
            delete [] page_table;
            close(swapfile_fd);
            close(program_fd);
        }
        main_memory[(frame*page_size)+offset] = value;
        page_table[page].frame = frame; // update the right frame
        page_table[page].V = 1; // now valid
        push(frame); // put the frame into FIFO queue
        freeFrames[frame] = 1; // now update the fre frames array
        return;
    }
    else
    {
        if(address < text_size+data_size) //in situation of data
        {
            int frame = frameCheck(); // free frame check
            if(frame == -1)
                frame = swapCheck(page);  
            lseek(program_fd, page*page_size,SEEK_SET); // put pointer on the right location
            fd=read(program_fd,&main_memory[frame*page_size],page_size); // read from main memory without buffer straight by &
             if(fd<0)
        {
            delete [] freeFrames;
            delete [] page_table;
            close(swapfile_fd);
            close(program_fd);
        }
            page_table[page].V = 1;//update valid
            page_table[page].D = 1;// update dirty
            page_table[page].frame = frame;//update frame
            push(frame);//put on frame queue for FIFO
            freeFrames[frame] = 1;// update free frame array
            main_memory[frame*page_size+offset] = value; // update main memory
            return;    
        }
        else
        {
            int frame = frameCheck();
            if(frame == -1)
                frame = swapCheck(page);
            page_table[page].V = 1;
            page_table[page].D = 1;
            page_table[page].frame = frame;
            push(frame);
            freeFrames[frame] = 1;
            for(int i =0; i<page_size;i++)
                main_memory[(frame*page_size)+i] = '0';
            return;    
        }  
    }
}



//destructor
sim_mem::~sim_mem()
{
    delete [] freeFrames;
    delete [] page_table;
    close(swapfile_fd);
    close(program_fd);
}
