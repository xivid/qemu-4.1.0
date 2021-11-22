#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sched.h>   //cpu_set_t , CPU_SET




void *lseek_cpy_tmpfs (void *arg);



int thread_count;
long total_bytes; 
long chunk; 
char *src, *dst;

static void mem_cpy_tmpfs(char *src_file, char *dst_file){
                char *str_thread_count;
                int r; 
                pthread_t *tid;
                int g_sfd;
                int *thread_index;

                src = src_file;
                dst = dst_file;

                g_sfd = open (src, O_RDONLY);
                if (g_sfd == -1 ) {
                        perror("File open error");
                }  

                total_bytes = lseek(g_sfd, 0, SEEK_END);
                if(total_bytes == -1)
                        perror ("Cannot seek the source file\n");

                str_thread_count = getenv("thread_count");
                if(str_thread_count == NULL)
                        perror("Set the env variable thread_count Eg: export thread_count=2\n");


                thread_count = atoi(str_thread_count); 
                chunk = total_bytes / thread_count;     


                thread_index = (int *) malloc(sizeof(int)*thread_count);
                tid = (pthread_t*) malloc(sizeof(pthread_t)*thread_count);
                if(tid == NULL || thread_index == NULL)
                        perror ("Cannot allocate memory for threads\n");


                
                for (int i = 0; i < thread_count; i++) {
                        thread_index[i] = i;
                        if ((r = pthread_create (&tid [i], NULL, lseek_cpy_tmpfs, (void*)&thread_index[i])) != 0) {

                                perror("Cannot create multiple threads\n");
                        }
                }


                // Wait for threads to terminate
                for (int i = 0; i < thread_count; i++) {
                        if ((r = pthread_join (tid [i], NULL)) != 0) {

                                perror("Cannot join multiple threads\n");
                        }
                }
                
                close(g_sfd);
        }

void *lseek_cpy_tmpfs(void *arg){

                int sfd = open (src, O_RDONLY);
                int dfd = open (dst, O_CREAT | O_RDWR, 0644);
                if (sfd == -1 || dfd == -1) {
                        perror("File open error: The file name should be in /mnt/template_ramX/memory format. X is the index that starts with 1");
                }

                cpu_set_t cpuset;
                int tid = *(int *)(arg);
                long off_begin, off_end, byte_length, rw_bytes;
                long offset = tid * chunk;
                char *buffer = (char *) malloc(1024L*1024L*1024L);
                if(buffer == NULL)
                        perror("Cannot allocate memory for Buffer\n");
                long total_data = 0;       
                long exceed_range = 0;


                CPU_ZERO(&cpuset);
                CPU_SET((tid+1), &cpuset);


                int s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
                if (s != 0)
                         perror("pthread_setaffinity_np failed");
                        



                s = pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
                if (s != 0)
                        perror("pthread_getaffinity_np failed");
                       



                if(total_bytes % thread_count)
                        if(tid == thread_count-1)
                                chunk = total_bytes - ((thread_count-1) * chunk);

                long end_range = chunk + offset;
                int lseek_err=1;
                

                do{
                        off_begin = lseek(sfd, offset, SEEK_DATA);
                        off_end = lseek(sfd, off_begin, SEEK_HOLE);
                        if(off_begin == -1 || off_end ==-1)
                                perror("Data/hole seek error\n");

                        if(off_begin >= end_range){
                                break;
                        }


                        byte_length = off_end - off_begin;
                        exceed_range = off_begin + byte_length;


                        if(exceed_range >= end_range){
                                byte_length = end_range - off_begin;
                        }




                        total_data = total_data + byte_length;


                        lseek_err = lseek(sfd, off_begin, SEEK_SET);
                        if(lseek_err == -1)
                                perror("Lseek set on source file causes error\n");
                        lseek_err = lseek(dfd, off_begin, SEEK_SET);
                        if(lseek_err == -1)
                                perror("Lseek set on destination file causes error\n");
                                


                        rw_bytes = read(sfd, buffer, byte_length);
                        if(rw_bytes == -1)
                                perror("Read error on source file\n");
                        rw_bytes = write(dfd, buffer, rw_bytes);
                        if(rw_bytes == -1)
                                perror("Write error on destination file\n");
                        memset(buffer,0,rw_bytes);


                        offset = off_end;
                        lseek_err = lseek(sfd, offset, SEEK_SET);
                        if(lseek_err == -1)
                                perror("Lseek set on source file causes error\n");
                        lseek_err = lseek(dfd, offset, SEEK_SET);
                        if(lseek_err == -1)
                                perror("Lseek set on destination file causes error\n");
 
                }while(offset  < end_range );

                close(sfd);
                close(dfd);
                return NULL;

}

