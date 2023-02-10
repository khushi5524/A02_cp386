#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "process_management.h"

int main(int argc, char *args[]) {
    if(argc>1){
        int shmid = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666);

        if (shmid == -1)
        {
            printf("\n Error occured during creating a shared memory area\n");
            exit(1);
        }
        ftruncate(shmid, SHMSIZE);
        char *numptr = mmap(0, SHMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
        if (numptr == MAP_FAILED) {
            printf("\nError while mapping the shared memory area to process\n");
            exit(1);
        }
        char *fd = args[1];
        File_shm(numptr, fd);

        commandsopRun(numptr);

    } else {
            printf("\nError because no file name specified as argument\n");
            exit(1);
    }
}

void File_shm(char *file_ptr, char *flinenm) {
    char *f_ptr = file_ptr;
    pid_t pro_id = fork();
    if (pro_id == 0) {
        char *len = NULL;
        FILE *fd = fopen(flinenm,"r");
        size_t num_len = 0;
        ssize_t read;

        while ((read = getline(&len, &num_len, fd)) != -1){
             f_ptr += sprintf(f_ptr, "%s", len);
        }
        exit(0);
    } else if (pro_id > 0) {
        
        int varstatus;

        wait(&varstatus);
        if (WEXITSTATUS(varstatus) == -1) {
            perror("\nError while waiting\n");
            exit(1);
        }
    } else {
        printf("\nError occured executing fork in function\n");
        exit(1);
    }

}

void pipe_fd(int p_id) {
    char buff[SHMSIZE];
    read(p_id, buff, SHMSIZE);

    FILE *fd_open = fopen(FILE_NAME_OUTPUT, "w");
    char *fdptr = strtok(buff, "\r\n");

    while (fdptr) {
        fprintf(fd_open, "%s\n", fdptr);
        fdptr = strtok(NULL, "\r\n");
    }
    fclose(fd_open);
    printf("\nFinished!\n");
}

void commandsopRun(char *varptr) {
    char *pointer = varptr;
    char *ptr_name = "/tmp/pipe";
	
    mkfifo(ptr_name, 0666);
    pid_t pro_id = fork();

    if (pro_id == 0) {
        char buff[SHMSIZE];
        int pid = open(ptr_name, O_WRONLY);
        for (int i=0; i<64; i+=1) {
            char nxt = (char)pointer[i];
            strncat(buff, &nxt, 1);
        }
        char *buff_fd = strtok(buff, "\r\n");
        filecmdwrite(buff_fd, pid);
        close(pid);
    } else if (pro_id > 0) {
        int varstatus;
        int pid = open(ptr_name, O_RDONLY);
        wait(&varstatus);
        if (WEXITSTATUS(varstatus) == -1) {
            perror("Error occured while waiting for child to finish in the function");
            exit(1);
        }
        pipe_fd(pid);
        close(pid);
    } else {
        perror("Error occured while using fork in function");
        exit(1);
    }
}

void filecmdwrite(char *buff_fd, int p_id) {
    char buff[SHMSIZE];
    while (buff_fd) {
        FILE *fv = popen(buff_fd, "r");
        char lnum[1035];

        if (fv) {
            char fline[100];

            sprintf(fline, "The output of: %s : is\n>>>>>>>>>>>>>>>>\n", buff_fd);
            strcat(buff, fline);

            while(fgets(lnum, sizeof(lnum), fv) != NULL) {
                strcat (lnum,"\n");
                strcat(buff,lnum);
            }

            strcat(buff, "<<<<<<<<<<<<<<<<<\n");
        } else {

            printf("\nError occurred while executing command\n");
            exit(1);

        }
        fclose(fv);
        buff_fd = strtok(NULL, "\r\n");
    }
    write(p_id, buff, SHMSIZE + 1);
}