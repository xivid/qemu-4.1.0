#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

static void mem_cpy_tmpfs(char *src_file, char *dst_file){
        int sfd, dfd, trunc;
        size_t fsize, rw_bytes;
        char* buffer;
        long off_begin, off_end, offset, total_data, byte_length;
    
        sfd = open (src_file, O_CREAT | O_RDWR, 0644);
        dfd = open(dst_file, O_CREAT | O_RDWR, 0644);
        if (sfd == -1 || dfd == -1) {
                perror("File open error");
                
        }

        offset = 0;
        total_data = 0;
        byte_length = 0;
        fsize = lseek(sfd, 0, SEEK_END);
        trunc = ftruncate(dfd, fsize);
        size_t buff_size =(1024L*1024L*1024L);
        buffer = (char*)malloc(buff_size);
        if(trunc == -1)
                 perror("Trunc error");


       

        do{  
                off_begin = lseek(sfd, offset, SEEK_DATA);
                off_end = lseek(sfd, off_begin, SEEK_HOLE);
                if(off_begin == -1 || off_end ==-1)
                        perror("Data/hole seek error\n");


                byte_length = off_end - off_begin;
                total_data = total_data + byte_length;
 
 
                
                lseek(sfd, off_begin, SEEK_SET);
                lseek(dfd, off_begin, SEEK_SET);
                
               

                
                rw_bytes = read(sfd, buffer, byte_length);
                rw_bytes = write(dfd, buffer, rw_bytes);
                memset(buffer,0,rw_bytes);


                offset = off_end;
                lseek(sfd, offset, SEEK_SET);
                lseek(dfd, offset, SEEK_SET);

           }while(offset < fsize);

                  
        
        close(sfd);
        close(dfd);
        free(buffer);
}
