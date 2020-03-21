# SoalShiftSISOP20_modul2_F03
Pengerjaan Praktikum 2 SisOp Kelompok F03

## Soal 1
[Source Code](https://github.com/IktaS/SoalShiftSISOP20_modul2_F03/blob/master/soal1/soal1.c)

Diminta membuat program yang berfungsi seperti crontab, diminta :   
  A. menerima 4 argumen :  
    i. Detik: 0-59 atau * (any value)  
    ii. Menit: 0-59 atau * (any value)  
    iii. Jam: 0-23 atau * (any value)  
    iv. Path file .sh  
  B. mengeluarkan pesan error jika argumen yang diberikan tidak sesuai  
  C. hanya menerima 1 config cron  
  D. berjalan di background  
  
Main function :
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
Mari breakdown per bagian:
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
akan mengecek apakah argumen yang dimasukkan sudah sesuai dengan bantuan fungsi-fungsi berikut:
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
Keempat fungsi tersebut adalah utility function untuk membantu mentransformasi int menjadi string dan melakukan pengecekan.\
Selanjutnya untuk menjalankan program di background sebagai daemon digunakan:

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

Untuk bagian pentingnya, di dalam ```while``` diisikan:  
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
Berikut adalah penjelasan tiap bagiannya::

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
Digunakan untuk mengulagi loop sampai waktu eksekusi yang ditetapkan tiba.
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
stopDoing() adalah utility function untuk membantu mengetahui apakah sudah waktunya untuk menjalankan fungsi.
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
Bagian ini akan membuat child, dan childnya akan menjalankan shell script dengan execv.
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
```
Mari lihat per modulnya.
```c
if(strcmp(argv[1],"-a") == 0){
    makeKiller1();
}else if(strcmp(argv[1],"-b")==0){
    makeKiller2();
}
```
bagian ini untuk mengenerate file executable berdasarkan argumen
```c
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
```
makeKiller1() dan makeKiller2() adalah utility function untuk mengenerate file executable. makeKiller1() akan membuat file executable yang akan menjalankan killall soal2 untuk menghentikan semua process soal2, sedangkan makeKiller2() akan mencari PID dari session leader soal2, lalu membunuh session leadernya. Dengan cara ini, process pembuatan folder baru akan berhenti, tapi process download image, dll akan tetap berjalan di yang sudah berjalan.  
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

char buffer[10000];
getcwd(buffer,sizeof(buffer));
strcat(buffer,"/");
if ((chdir(buffer)) < 0) {
    exit(EXIT_FAILURE);
}

close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
```
bagian ini membuat program utama menjadi daemon.
```c
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
```
di dalam while(1), pertama kita membuat child, di child inilah dijalankan process membuat folder dengan forkAndMakeDir(), mendownload 20 image dengan forkAndDownloadNImage(20), membuat zip dengan forkAndZipDir(), dan menghapus directory dengan RemoveDir(). dilakukan pembuatan child agar tetap bisa dilakukan pembuatan folder walaupun masih ada process mendownload di folder lain.  
```c
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
```
forkAndMakeDir(finalDir,dir) digunakan untuk membuat directory bernama dir, dan passing path akhir dari directory yang dibuat ke finalDir.  
```c
if(chdir(dirPath) < 0){
    exit(EXIT_FAILURE);
}
forkAndDownloadNImage(20);
if(chdir("..") < 0){
    exit(EXIT_FAILURE);
}
```
pertama kita perlu pindah ke dalam folder dimana kita ingin mendownload image kita, setelah selesai kita juga kembali ke parent folder folder itu.  
```c
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
```
untuk mendownload image sebanyak number dengan forkAndDownloadImage(), dijarak sebanyak 5 detik dengan sleep(5), dan hanya return ketika selesai menunggu semua terdownload.  
```c
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
```
bagian ini untuk membuat child lalu mendownload image dengan execv wget di child tersebut.  
```c
char zipName[10000];
char folderName[10000];
strcpy(zipName,timeString);
strcpy(folderName,timeString);
```
bagian ini untuk mendapatkan nama folder dan zip file
```c
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
```
forkAndZipDir(finalFileName,dir) akan menzip directory di dir, lalu passing output dari finalFileName menjadi finalFileName.zip, dan hanya return ketika zip selesai.  
```c
void RemoveDir(char * dirPath){
    char * argv[] = {"rm","-rf",dirPath,NULL};
    execv("/usr/bin/rm",argv);
}
```
RemoveDir(dirPath) untuk menghapus sebuah directory dirPath.  

# Soal 3
[Source Code](https://github.com/IktaS/SoalShiftSISOP20_modul2_F03/blob/master/soal3/soal3.c)

Diminta untuk membuat program yang bisa :  
  a. membuat dua directory, indomie dan sedaap setelah 5 detik di /home/[USER]/modul2/  
  b. mengekstrak file jpg.zip di /home/[USER]/modul2/  
  c. memindahkan hasil ekstrak zip, /home/[USER]/modul2/jpg/ , semua file ke folder sedaap, dan semua directory ke folder indomie  
  d. membuat dua file kosong coba1.txt dan coba2.txt setelah 3 detik ke semua folder di /home/[USER]/modul2/indomie/  
  
  main function :
```c
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
```

```c
char indomie[] = "indomie";
char sedaap[] = "sedaap";
char modul2Dir[] = "/home/ikta/modul2/";
char jpgFile[] = "jpg.zip";
char coba1File[] = "coba1.txt";
char coba2File[] = "coba2.txt";
```
bagian ini untuk menginisiasi semua nama nama yang akan digunakan.
```c
 char indomieDir[10000]; 
forkAndMakeDir(indomieDir,indomie);
sleep(5);
char sedaapDir[10000];
forkAndMakeDir(sedaapDir,sedaap);
```
bagian ini untuk membuat directory indomie, lalu setelah 5 detik, membuat directory sedaap, dua duanya menggunakan forkAndMakeDir()  
```c
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
```
forkAndMakeDir akan membuat child untuk menjalankan mkdir,dan mempassing string path dari directory yang dibuat ke finalDir  
```c
 char jpgDir[10000];
 forkAndUnzip(jpgDir,jpgFile);
```
untuk mengunzip file jpg
```c
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
```
forkAndUnzip akan membuat child yang akan mengunzip file, lalu mempassing path dari file yang di extract ke finalDir
```c
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
```
fungsi sortThroughZip menerima folderPath yaitu path directory yang akan di sort, dirFolder yaitu path tempat memindah semua directory, dan fileFolder yaitu path tempat memindah semua file.
```c
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
```
bagian ini akan mengecek semua yang ada dalam directory, dan jika merupakan file ( dicek dengan is_regular_file()) akan dipindah ke fileFolder dengan forkAndMove, dan jika bukan file, dan jika bukan "." maupun "..", akan dipindah ke dirFolder dengan forkAndMove.  
```c
int is_regular_file( char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
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
```
forkAndMove akan membuat child dan mengeksekusi execv mv di child itu.
```c
sortThroughDirectory(indomieDir,coba1File,coba2File);
```
akan membuat dua file, coba1.txt dan coba2.txt di setiap folder di indomieDir.
```c
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
```
sortThroughDirectory()
```c
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
```
bagian ini akan mengiterasi semua yang ada dalam dir, lalu jika ada yang berupa folder, akan membuat file1 dengan forkAndTouch(), lalu setelah 3 detik membuat file2 dengan cara yang sama.  
```c
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
```
forkAndTouch akan membuat child lalu mengeksekusi execv touch di child itu.
