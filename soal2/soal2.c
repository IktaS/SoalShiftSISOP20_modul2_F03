#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <wait.h>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include <signal.h>

void intToString(int num, char * string, int base){
    char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"%d",num);
    strcpy(string,buffer);
}
void printCurrentDirectory(){
    char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    getcwd(buffer,sizeof(buffer));
    printf("%s\n",buffer);
}

void getTime(char * dest){
    char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buffer,sizeof(buffer),"%F_%T",&tm);
    strcpy(dest,buffer);
}
int digitNum(int num){
    int i=0;
    while(num){
        num /= 10;
        i++;
    }
    return i;
}
void RemoveDir(char * dirPath){

    char * argv[] = {"rm","-rf",dirPath,NULL};
    execv("/usr/bin/rm",argv);
}



void forkAndZipDir(char * finalFileName,char * dir){
    pid_t child_id;
    int status;
    strcat(finalFileName,".zip");
    char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    strcpy(buffer,dir);
    strcat(buffer,"/");
    child_id = fork();


    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char * argv[] = {"zip","-q","-r",finalFileName,buffer,NULL};
        execv("/usr/bin/zip",argv);
    }else{
        wait(&status);
        return;
    }

}


void forkAndMakeDir(char * finalDir,char * dir){
    pid_t child_id;
    int status;
    char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    getcwd(buffer,sizeof(buffer));
    strcpy(finalDir,buffer);
    strcat(finalDir,"/");
    strcat(finalDir,dir);
    strcat(finalDir,"/");
    child_id = fork();

    if(child_id < 0){
        exit(EXIT_FAILURE);
    }

    if(child_id == 0){
        char * argv[] = {"mkdir","-p",dir,NULL};
        execv("/usr/bin/mkdir",argv);
    }else{
        wait(&status);
        return;
    }
}

void forkAndDownloadImage(char * name, char * link){
    pid_t child_id;
    int status;

    child_id = fork();
    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char *argv[] = {"wget","-q","-O",name,link,NULL};
        execv("/usr/bin/wget",argv);
    }else{
        return;
    }
}

void forkAndDownloadNImage(int number){
    pid_t child_id;
    int status;
    child_id = fork();

    if(child_id<0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        for (int i = 0; i < number; i++){
            char link[] = "https://picsum.photos/";
            char name[10000];
            getTime(name);
            int size = (time(NULL)%1000) + 100;
            char sizeString[10000];
            intToString(size,sizeString,10);
            strcat(link,sizeString);
            forkAndDownloadImage(name,link);
            sleep(5);
        }
    }else{
        wait(&status);
        return;
    }
    
}

void makeKiller1(){
    FILE * file = fopen("killer","w+");
    fprintf(file,"#!/bin/bash\n");
    fprintf(file,"pkill soal2\n");
    fprintf(file,"rm -- \"$0\"\n");
    fclose(file);
    chmod("killer", ~0);
}

void makeKiller2(){
    FILE * file = fopen("killer","w+");
    fprintf(file,"#!/bin/bash\n");
    fprintf(file,"parent_id=$(ps -aux | grep soal2 | grep Ss | cut -d \" \" -f 6)\n");
    fprintf(file,"kill -9 $parent_id\n");
    fprintf(file,"rm -- \"$0\"\n");
    fclose(file);
    chmod("killer", ~0);
}

int main(int argc, char ** argv){
    if(strcmp(argv[1],"-a") == 0){
        makeKiller1();
    }else if(strcmp(argv[1],"-b")==0){
        makeKiller2();
    }

    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    char buffer[10000];
    getcwd(buffer,sizeof(buffer));
    strcat(buffer,"/");
    if ((chdir(buffer)) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        pid_t child_id;
        child_id = fork();
        if(child_id<0){
            exit(EXIT_FAILURE);
        }
        if(child_id == 0){
            char timeString[10000];
            getTime(timeString);
            char dirPath[10000];
            forkAndMakeDir(dirPath,timeString);
            if(chdir(dirPath) < 0){
                exit(EXIT_FAILURE);
            }
            forkAndDownloadNImage(20);
            if(chdir("..") < 0){
                exit(EXIT_FAILURE);
            }
            char zipName[10000];
            char folderName[10000];
            strcpy(zipName,timeString);
            strcpy(folderName,timeString);
            forkAndZipDir(zipName,folderName);
            RemoveDir(folderName);
        }else{
            sleep(30);
            continue;
        }
    }
}