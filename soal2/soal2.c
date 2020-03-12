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
    // printf("%s\n",buffer);
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
void forkAndRemoveDir(char * dirPath){
    pid_t child_id;
    int status;
    child_id = fork();

    if(child_id<0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char * argv[] = {"rm","-rf",dirPath,NULL};
        execv("/usr/bin/rm",argv);
    }else{
        wait(&status);
        return;
    }
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
        // printf("%s\n",finalDir);
        return;
    }
}

void forkAndDownloadImage(char * name, char * link){
    // printCurrentDirectory();
    pid_t child_id;
    int status;

    child_id = fork();
    // printf("%d\n",child_id);
    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        // printf("masuk");
        char *argv[] = {"wget","-q","-O",name,link,NULL};
        execv("/usr/bin/wget",argv);
    }else{
        wait(&status);
        return;
    }
}

int main(){
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
            // printf("%s\n",timeString);
            forkAndMakeDir(dirPath,timeString);
            // printf("%s\n",dirPath);
            if(chdir(dirPath) < 0){
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < 20; i++)
            {
                char link[] = "https://picsum.photos/";
                char name[10000];
                getTime(name);
                int size = (time(NULL)%1000) + 100;
                char sizeString[10000];
                intToString(size,sizeString,10);
                strcat(link,sizeString);
                // printf("%s\n%s\n",name,link);
                forkAndDownloadImage(name,link);
                sleep(5);
                // printf("%d\n",i);
            }
            if(chdir("..") < 0){
                exit(EXIT_FAILURE);
            }
            char zipName[10000];
            char folderName[10000];
            strcpy(zipName,timeString);
            strcpy(folderName,timeString);
            forkAndZipDir(zipName,folderName);
            forkAndRemoveDir(folderName);
            // printf("keluar");
            kill((int)getpid(),SIGKILL);
            return 0;
        }else{
            sleep(30);
            continue;
            // return 0;
        }
    }
}