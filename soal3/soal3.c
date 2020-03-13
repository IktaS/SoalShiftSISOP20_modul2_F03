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

int is_regular_file( char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char * getdir(char* dir){
    char * finaldir = (char*) malloc(sizeof(char)*strlen(dir));
    strcpy(finaldir,dir);
    for (int i = strlen(finaldir)-1; i > 0; i--)
    {
        if(finaldir[i-1] == '/'){
            finaldir[i] = '\0';
            break;
        }
    }
    return finaldir;
}

void forkAndMakeDir(char * finalDir,char * dir){
    pid_t child_id;
    int status;
    char buffer[10000];
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

void getFileName(char * dest, char * file){
    char temp[10000];
    strcpy(temp,file);
    for(int i = strlen(temp);i>=0;i--){
        if(temp[i] == '.'){
            temp[i] = '\0';
        }
    }
    strcpy(dest,temp);
}

void forkAndUnzip(char * finalDir,char * file){
    pid_t child_id;
    int status;
    child_id = fork();

    char buffer[10000];
    getcwd(buffer,sizeof(buffer));
    strcpy(finalDir,buffer);
    strcat(finalDir,"/");
    getFileName(buffer,file);
    strcat(finalDir,buffer);
    strcat(finalDir,"/");

    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char * argv[] = {"unzip","-qq",file,NULL};
        execv("/usr/bin/unzip",argv);
    }else{
        wait(&status);
        return;
    }
}

void forkAndMove(char * src, char * dest){
    int status;
    pid_t child_id;
    child_id = fork();

    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char * argv[] = {"mv",src,dest,NULL};
        execv("/usr/bin/mv",argv);
    }else{
        wait(&status);
    }
}

void forkAndTouch(char * filename){
    int status;
    pid_t child_id;
    child_id = fork();

    if(child_id < 0){
        exit(EXIT_FAILURE);
    }
    if(child_id == 0){
        char * argv[] = {"touch",filename,NULL};
        execv("/usr/bin/touch",argv);
    }else{
        wait(&status);
        return;
    }
}

void sortThroughZip(char * folderPath, char * dirFolder, char * fileFolder){
    char beforePath[10000];
    getcwd(beforePath,sizeof(beforePath));
    strcat(beforePath,"/");
    if(chdir(folderPath) < 0 ){
        exit(EXIT_FAILURE);
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(is_regular_file(dir->d_name)){
                forkAndMove(dir->d_name,fileFolder);
            }else{
                if(strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,".") != 0){
                    forkAndMove(dir->d_name,dirFolder);
                }
            }
        }
        closedir(d);
    }
    if(chdir(beforePath) < 0){
        exit(EXIT_FAILURE);
    }
}

void sortThroughDirectory(char * folderPath, char * file1, char * file2){
    char beforePath[10000];
    getcwd(beforePath,sizeof(beforePath));
    strcat(beforePath,"/");
    if(chdir(folderPath) < 0 ){
        exit(EXIT_FAILURE);
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(!is_regular_file(dir->d_name)){
                if(strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,".") != 0){
                    char buffer[10000];
                    strcpy(buffer,dir->d_name);
                    strcat(buffer,"/");
                    if(chdir(buffer) < 0){
                        exit(EXIT_FAILURE);
                    }
                    forkAndTouch(file1);
                    sleep(3);
                    forkAndTouch(file2);
                    if(chdir("..") < 0){
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        closedir(d);
    }
    if(chdir(beforePath) < 0){
        exit(EXIT_FAILURE);
    }

}

int main() {
    char indomie[] = "indomie";
    char sedaap[] = "sedaap";
    char modul2Dir[] = "/home/ikta/modul2/";
    char jpgFile[] = "jpg.zip";
    char coba1File[] = "coba1.txt";
    char coba2File[] = "coba2.txt";

    

    if((chdir(modul2Dir)) < 0){
        exit(EXIT_FAILURE);
    }

    char indomieDir[10000]; 
    forkAndMakeDir(indomieDir,indomie);
    sleep(5);
    char sedaapDir[10000];
    forkAndMakeDir(sedaapDir,sedaap);
    char jpgDir[10000];
    forkAndUnzip(jpgDir,jpgFile);
    sortThroughZip(jpgDir,indomieDir,sedaapDir);
    sortThroughDirectory(indomieDir,coba1File,coba2File);
}