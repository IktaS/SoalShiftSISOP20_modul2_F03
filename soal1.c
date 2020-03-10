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

int digits_only(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

int checkNumber(char *s,int num){
    if(strlen(s)>2 || strlen(s)==0) return 0;
    if(strlen(s) == 1){
        if(s[0] == '*') return 1;
    }
    if(!digits_only(s)) return 0;
    // printf("%s\n",s);
    int sum = 0;
    int multiplier = 1;
    for(int i=strlen(s)-1;i>=0;i--){
        // printf("loop %d\n",sum);
        sum += (s[i]-'0')*multiplier;
        multiplier *= 10;
    }
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
int main(int argc, char * argv[]) {

    if(argc <= 4 || !checkInput(argv[1],argv[2],argv[3])){
        printf("Bad Argument\n");
        exit(EXIT_FAILURE);
    }

    
    // pid_t pid, sid;        // Variabel untuk menyimpan PID

    // pid = fork();     // Menyimpan PID dari Child Process

    // /* Keluar saat fork gagal
    // * (nilai variabel pid < 0) */
    // if (pid < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // /* Keluar saat fork berhasil
    // * (nilai variabel pid adalah PID dari child process) */
    // if (pid > 0) {
    //     exit(EXIT_SUCCESS);
    // }

    // umask(0);

    // sid = setsid();
    // if (sid < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // if ((chdir("/")) < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);

    // while (1) {
    //     // Tulis program kalian di sini

    //     sleep(30);
    // }
}