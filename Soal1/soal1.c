#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <wait.h>
#include <time.h>

int digits_only(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

int to_number(char *s){
    if(!digits_only(s))return -1;
    int sum = 0;
    int multiplier = 1;
    for(int i=strlen(s)-1;i>=0;i--){
        // printf("loop %d\n",sum);
        sum += (s[i]-'0')*multiplier;
        multiplier *= 10;
    }
    return sum;
}
int checkNumber(char *s,int num){
    if(strlen(s)>2 || strlen(s)==0) return 0;
    if(strlen(s) == 1){
        if(s[0] == '*') return 1;
    }
    if(!digits_only(s)) return 0;
    // printf("%s\n",s);
    int sum = to_number(s);
    // printf("%d\n",sum);
    return (sum<=num);
}

int checkInput(char * arg1, char * arg2, char * arg3){
    if(!checkNumber(arg1,59)){
        return 0;
    } 
    if(!checkNumber(arg2,59)) {
        // printf("%s\n",arg2);
        return 0;
    } 
    if(!checkNumber(arg3,23)){
        // printf("%s\n",arg3);
        return 0;
    } 
}

int calculateNeedToSleep(struct tm * curTime,char * sec, char * min, char * hour){
    int sleepTime = 0;
    if(curTime->tm_sec != to_number(sec) && strcmp(sec,"*") != 0){
        sleepTime += ((60-curTime->tm_sec) + to_number(sec))%60;
    }
    // printf("cur sec:%d %d\n",curTime->tm_sec,sleepTime);
    if(curTime->tm_min != to_number(min)+1 && strcmp(min,"*") != 0){
        sleepTime += (((60-curTime->tm_min) + to_number(min))%60)*60;
    }
    // printf("cur min:%d %d\n",curTime->tm_min,sleepTime);
    if(curTime->tm_hour != to_number(hour)+1 && strcmp(hour,"*") != 0){
        sleepTime += (((24-curTime->tm_hour) + to_number(hour))%24)*3600;
    }
    // printf("cur hour:%d %d\n",curTime->tm_hour,sleepTime);
    if(sleepTime == 0) sleepTime = 1;
    return sleepTime;
}

char * getdir(char* dir){
    char * finaldir = (char*) malloc(sizeof(char)*strlen(dir));
    strcpy(finaldir,dir);
    // printf("%s\n",finaldir);
    for (int i = strlen(finaldir)-1; i > 0; i--)
    {
        if(finaldir[i-1] == '/'){
            finaldir[i] = '\0';
            break;
        }
    }
    return finaldir;
}

int main(int argc, char * argv[]) {

    if(argc <= 4 || !checkInput(argv[1],argv[2],argv[3])){
        printf("Bad Argument\n");
        exit(EXIT_FAILURE);
    }

    if(!fopen(argv[4], "r")){
        printf("Bad Path");
        exit(EXIT_FAILURE);
    }

    // while(1){
    //     time_t rawTime;
    //     struct tm * currentTime;
    //     time(&rawTime);
    //     currentTime = localtime(&rawTime);
    //     int sleepTime = calculateNeedToSleep(currentTime,argv[1],argv[2],argv[3]);
    //     sleep(sleepTime);
    //     printf("%s\n",getdir(argv[4]));
    // }
    // return 0;

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

    if ((chdir(getdir(argv[4]))) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {

        time_t rawTime;
        struct tm * currentTime;
        time(&rawTime);
        currentTime = localtime(&rawTime);

        int sleepTime = calculateNeedToSleep(currentTime,argv[1],argv[2],argv[3]);
        sleep(sleepTime);
        // return 0;

        pid_t child_id;
        child_id = fork();

        if(child_id < 0 ){
            exit(EXIT_FAILURE);
        }

        if(child_id == 0){
            char * dir = getdir(argv[4]);
            if((chdir(dir)) < 0){
                exit(EXIT_FAILURE);
            }
            char *bashargv[] = {"bash",argv[4],NULL};
            execv("/usr/bin/bash",bashargv);
        }
    }
}