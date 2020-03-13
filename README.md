# SoalShiftSISOP20_modul2_F03
Pengerjaan Praktikum 2 SisOp Kelompok F03

## Soal 1
[Source Code](https://github.com/IktaS/SoalShiftSISOP20_modul2_F03/blob/master/soal1/soal1.c)

Diminta membuat program seperti crontab, diminta : 
  A. menerima 4 argumen :
    i. Detik: 0-59 atau * (any value)
    ii. Menit: 0-59 atau * (any value)
    iii. Jam: 0-23 atau * (any value)
    iv. Path file .sh
  B. mengeluarkan pesan error jika argumen yang diberikan tidak sesuai
  C. hanya menerima 1 config cron
  D. berjalan di background

main function :
```c
int main(int argc, char * argv[]) {

    if(argc <= 4 || !checkInput(argv[1],argv[2],argv[3])){
        printf("Bad Argument\n");
        exit(EXIT_FAILURE);
    }

    if(!fopen(argv[4], "r")){
        printf("Bad Path");
        exit(EXIT_FAILURE);
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

        if(!stopDoing(currentTime,argv[1],argv[2],argv[3])){
            sleep(1);
            continue;
        }

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
        sleep(1);
    }
}
```
Mari breakdown per bagian.
```c
if(argc <= 4 || !checkInput(argv[1],argv[2],argv[3])){
    printf("Bad Argument\n");
    exit(EXIT_FAILURE);
}

if(!fopen(argv[4], "r")){
    printf("Bad Path");
    exit(EXIT_FAILURE);
}
```
akan mengecek apakah argumen yang dimasukkan sudah sesuai.
```c
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
    int sum = to_number(s);
    if(sum <0) return 0;
    return (sum<=num);
}

int checkInput(char * arg1, char * arg2, char * arg3){
    if(!checkNumber(arg1,59)){
        return 0;
    } 
    if(!checkNumber(arg2,59)) {
        return 0;
    } 
    if(!checkNumber(arg3,23)){
        return 0;
    } 
}
```
keempat fungsi ini adalah utility function untuk membantu mentransformasi int menjadi string dan melakukan pengecekan.
```c
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
```
akan menjalankan program di background
Sekarang masuk ke bagian pentingnya, apa yang dijalankan.
Di dalam while,
```c
time_t rawTime;
struct tm * currentTime;
time(&rawTime);
currentTime = localtime(&rawTime);

int sleepTime = calculateNeedToSleep(currentTime,argv[1],argv[2],argv[3]);
sleep(sleepTime);

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
```
mari lihat lagi per bagiannya,
```c
time_t rawTime;
struct tm * currentTime;
time(&rawTime);
currentTime = localtime(&rawTime);


if(!stopDoing(currentTime,argv[1],argv[2],argv[3])){
    sleep(1);
    continue;
}
```
ini digunakan untuk mengulagi loop sampai sudah waktunya mengeksekusi.
```c
int stopDoing(struct tm * curTime,char * sec, char * min, char * hour){
    int sleepTime = 0;

    if(curTime->tm_sec != to_number(sec) && strcmp(sec,"*") != 0){
        sleepTime += 1;
    }
    if(curTime->tm_min != to_number(min) && strcmp(min,"*") != 0){
        sleepTime += 1;
    }
    if(curTime->tm_hour != to_number(hour) && strcmp(hour,"*") != 0){
        sleepTime += 1;
    }
    if(sleepTime == 0) return 1;
    else return 0;
}
```
stopDoing() adalah utility function untuk membantu mengetahui apakah sudah waktu untuk menjalankan fungsi.
```c
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
```
bagian ini akan membuat child, dan childnya akan menjalankan shell script dengan execv.
```c
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
```
getdir() adalah utility function untuk mendapatkan directory dari file.

# Soal 2
[Source Code](https://github.com/IktaS/SoalShiftSISOP20_modul2_F03/blob/master/soal2/soal2.c)
Diminta membuat sebuah program "downloader" yang berjalan di background, yang bisa :
  a. membuat folder dengan nama timestamp "YYYY-mm-dd_HH:ii:ss" setiap 30 detik
  b. setiap folder itu diisi dengan 20 image berukuran t%1000 + 100(t adalah epoch unix), yang diambil dari picsum.photos setiap 5 detik.
  c. setelah terisi 20 image, folder tersebut di zip, lalu folder tersebut dihapus.
  d. program mengenerate killer executable yang akan menghentikan semua process, berdasarkan mode yang diinput sebagai parameter di program utama, "-a" agar killer executable menghentikan semua process dari soal2, "-b" agar killer executable menghentikan process utama, tapi process yang masih berjalan dibiarkan.
 
 main function:
 ```c
 int main(int argc, char ** argv){
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    if(strcmp(argv[1],"-a") == 0){
        makeKiller1();
    }else if(strcmp(argv[1],"-b")==0){
        makeKiller2();
    }else{
        makeKiller2();
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
            for (int i = 0; i < 20; i++)
            {
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
```
  
