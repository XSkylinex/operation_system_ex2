#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define SIZE 1024


void initConfigPage(int **init, char *configFile);
void addPathToExe(char* basePath);
char *customStrcpy(char *string_1,char *string_2);
void combineAll(char *base, char *sub, char *file, char *combineAllpaths,int addN);
char * customStrcat(char* destination, const char* source);
size_t customStrlen (char *str);
int countUsers();
void getAllUsersPathToRun(char* buffer);
void executeFunctionEveryUser(char* userPath,char* adminInput,char* user);
void readAdminInput(char* inputpath,char* buffer);
void makeUsersFolder();
void getAllUsers(char* userName);
int runCompOut(char* user,char* output);
void printResult(char* user, int grade,int fd);

int main(int argc, char *argv[]) {
    char users[SIZE];
    char inputPath[SIZE];
    char outputPath[SIZE];
    char bufferToGetAllUsersPathToRun[SIZE];
    char mainname[SIZE];

    char adminInput[SIZE];
    char userName[SIZE];
    char usersNameToFolder[SIZE];
    char bufferToExecution[SIZE];
    int *initAll[3] = {users, inputPath, outputPath};
    int status = 0;
    int checkError = 0;
    int howManyUsers = 0;
    int i = 0, k = 0; // this is for "for + while 1" get all paths to run users programs
    int a = 0, b = 0; // this is for "for + while 2" get all paths to run users programs
    initConfigPage(initAll, argv[1]);
    makeUsersFolder();
    int fdUserOutput = open("userFolders.txt", O_WRONLY | O_CREAT | O_RDONLY, 0666);
    char *pathToExc[] = {"ls", "-t", users, NULL};
    int fd = open("result.csv", O_WRONLY | O_CREAT | O_RDONLY, 0666);
    pid_t pid = fork(); /// dad and son split in the same spot
    if (pid < 0) {
        write(2, "fork error", 10);
    } else if (pid == 0) {
        close(1);
        if ((dup(fdUserOutput)) < 0) {
            write(2, "dup error", 9);
            _exit(1);
        }
        checkError = execvp(pathToExc[0], pathToExc);
        //done!
        if (checkError == -1) {
            write(2, "command error", 13);
            _exit(1);
        }
    } else {
        waitpid(pid, &status, 0);
    }

    close(fdUserOutput);
    addPathToExe(users);

    howManyUsers = countUsers();

    customStrcpy(bufferToGetAllUsersPathToRun, "");

    getAllUsersPathToRun(bufferToGetAllUsersPathToRun);
    readAdminInput(inputPath, adminInput);
    getAllUsers(userName);
    customStrcpy(mainname, "main.exe");
    for (i = 0; howManyUsers > i; i++) {

        customStrcpy(bufferToExecution, "");
        while (userName[a] != '\n') {// while 2
            usersNameToFolder[b] = userName[a];
            b++;
            a++;
        }

        usersNameToFolder[b] = '\0';

        combineAll(users, usersNameToFolder, mainname, bufferToExecution,0);

        executeFunctionEveryUser(bufferToExecution, adminInput, usersNameToFolder);
        if (runCompOut(usersNameToFolder, outputPath) == 1) {
            printResult(usersNameToFolder, 100, fd);
        } else {
            printResult(usersNameToFolder, 0, fd);
        }
        k++;
        b = 0;
        a++;
    }

    close(fd);
    return 0;

}

void initConfigPage(int **init, char *configFile){

    char buffer[SIZE];
    int i =0,j=0,k=0,q=0;
    int letters = 0;
    int flog = 0;
    char* users = init[0];
    char* inputPath = init[1];
    char* outputPath = init[2];


    int fd = open(configFile,O_RDONLY,0666); //this good one
    //int fd = open("../config.txt",O_RDONLY,0666);

    if(fd < 0){
        write(2,"file error",10);
        _exit(1);
    }

    letters = read(fd,buffer,SIZE);
    if(letters < 0){
        write(2,"read error",10);
    }
    for(i = 0 ; i <= letters ; i++){
        if(buffer[i] == '\n'){
            if(flog == 0){
                users[j] = '\0';
            } else if(flog == 1){
                inputPath[k]='\0';
            } else if(flog == 2){
                outputPath[q]='\0';
            }
            flog++;
            continue;
        }
        if(flog == 0){
            users[j] = buffer[i];
            j++;
        } else if(flog == 1){
            inputPath[k] = buffer[i];
            k++;
        } else if(flog == 2){
            outputPath[q] = buffer[i];
            q++;
        }
    }

    close(fd);

}

void addPathToExe(char *basePath){
    int letters = 0;
    char buffer[SIZE];
    char keepNames[SIZE];
    char toCopy[SIZE];
    char exeFile[SIZE];
    customStrcpy(exeFile,"main.exe");
    int i = 0,j =0;
    int fdFolderUsersToExe = open("userFolders.txt",O_RDONLY,0666);
    if(fdFolderUsersToExe < 0){
        write(2,"file error",10);
        _exit(1);
    }
    letters = read(fdFolderUsersToExe,buffer,SIZE);
    if(letters < 0){
        write(2,"read error",10);
        _exit(1);
    }
    close(fdFolderUsersToExe);

    int usersToExe = open("toRun.txt",O_WRONLY | O_RDONLY | O_CREAT,0666);
    if(usersToExe < 0){
        write(2,"file error",10);
        _exit(1);
    }

    customStrcpy(toCopy,"");
    while(buffer[i] != '\0'){
        if(buffer[i] != '\n'){
            keepNames[j] = buffer[i];
            j++;
        }else{
            keepNames[j] = '\0';
            combineAll(basePath, keepNames, exeFile, toCopy,1);
            j = 0;
        }
        i++;
    }
    write(usersToExe,toCopy,customStrlen(toCopy));
    close(usersToExe);

}

void combineAll(char *base, char *sub, char *file, char *combineAllpaths,int addN) {
    char fullPath[SIZE];
    customStrcpy(fullPath, "");
    customStrcat(fullPath, base);

    if (sub != NULL) {
        customStrcat(fullPath, "/");
        customStrcat(fullPath, sub);
    }

    if (file != NULL) {
        customStrcat(fullPath, "/");
        customStrcat(fullPath, file);
    }
    customStrcat(combineAllpaths,fullPath);
    if (addN!=0)
        customStrcat(combineAllpaths,"\n");
}

char * customStrcat(char* destination, const char* source){
    char* ptr = destination + customStrlen(destination);
    while (*source != '\0') {
        *ptr++ = *source++;
    }
    *ptr = '\0';
    return destination;
}

char *customStrcpy(char *string_1,char *string_2){
    unsigned i=0;
    for (i=0; string_2[i] != '\0'; ++i) {
        string_1[i] = string_2[i];
    }

    string_1[i]= '\0';

    return string_1;
}

size_t customStrlen (char *str){
    size_t i = 0;
    while(str[i++]);
    return i-1;
}

int countUsers(){
    int letters = 0;
    char buffer[SIZE];
    int users = 0,i=0;

    int fdFolderUsersToExe = open("userFolders.txt",O_RDONLY,0666);
    if(fdFolderUsersToExe < 0){
        write(2,"file error",10);
        _exit(1);
    }
    letters = read(fdFolderUsersToExe,buffer,SIZE);
    if(letters < 0){
        write(2,"read error",10);
        _exit(1);
    }
    close(fdFolderUsersToExe);

    for(i=0;i<letters;i++){
        if(buffer[i] == '\n') {
            users++;
        }
    }

    return users;

}

void getAllUsersPathToRun(char* buffer){
    int letters = 0;
    int fd = open("toRun.txt",O_RDONLY,0666);

    if(fd < 0){
        write(2,"file error",10);
        _exit(1);
    }

    letters = read(fd,buffer,1024);
    if(letters < 0){
        write(2,"read error",10);
    }

}

void executeFunctionEveryUser(char* userPath,char* adminInput,char* user){
    int checkError = 0;
    int status = 0;
    char tempbuffer[SIZE];
    customStrcpy(tempbuffer,"users/");
    customStrcat(tempbuffer,user);
    customStrcat(tempbuffer,".txt");

    int fd = open(tempbuffer, O_WRONLY | O_CREAT | O_RDONLY, 0666);
    char *runTest[] = {userPath, adminInput, NULL};
    pid_t pid = fork(); /// dad and son split in the same spot
    if (pid < 0) {
        write(2, "fork error", 10);
    } else if (pid == 0) {
        close(STDOUT_FILENO);
        dup(fd);
        checkError = execvp(runTest[0], runTest);
        if (checkError == -1) {
            write(2, "command error a\n", 15);
            _exit(1);
        }
    } else {
        waitpid(pid, &status, 0);
    }
    close(fd);
}

void readAdminInput(char* inputpath,char* buffer){
    int letters = 0;
    int fd = open(inputpath,O_RDONLY,0666);

    if(fd < 0){
        write(2,"file error",10);
        _exit(1);
    }

    letters = read(fd,buffer,1024);
    if(letters < 0){
        write(2,"read error",10);
    }

    buffer[letters] = '\0';
    close(fd);
}

void makeUsersFolder(){
    int status = 0;
    int checkError = 0;
    char* pathToExc[] = {"mkdir","users",NULL};
    pid_t pid = fork(); /// dad and son split in the same spot
    if (pid < 0){
        write(2, "fork error", 10);
    } else if (pid == 0) {
        checkError = execvp(pathToExc[0],pathToExc);
        //done!
        if(checkError == -1){
            write(2,"command error",13);
            _exit(1);
        }
    } else {
        waitpid(pid, &status, 0);
    }
}

void getAllUsers(char* userName){
    int letters = 0;
    int fd = open("userFolders.txt",O_RDONLY,0666);

    if(fd < 0){
        write(2,"file error",10);
        _exit(1);
    }

    letters = read(fd,userName,1024);
    if(letters < 0){
        write(2,"read error",10);
    }
    userName[letters] = '\0';
    close(fd);

}

int runCompOut(char* user,char* output){

    int checkError = 0;
    int status =0;
    char buffer[SIZE];
    int result = 0;
    customStrcpy(buffer,"");
    customStrcat(buffer,"users/");
    customStrcat(buffer,user);
    customStrcat(buffer,".txt");

    char *runTest[] = {"./comp.out",buffer,output, NULL};
    pid_t pid = fork(); /// dad and son split in the same spot
    if (pid < 0) {
        write(2, "fork error", 10);
    } else if (pid == 0) {
        checkError = execvp(runTest[0], runTest);
        if (checkError == -1) {
            write(2, "command error", 13);
            _exit(1);
        }
    } else {
        waitpid(pid, &status, 0);
        result = (WEXITSTATUS(status));
    }
    return result == 2;

}

void printResult(char* user, int grade,int fd){
    char sGrade[SIZE];
    sprintf(sGrade, "%d", grade);
    //int fd = open("result.csv",O_WRONLY | O_CREAT | O_RDONLY,0666);
    write(fd, user,customStrlen(user));
    write(fd, ",",customStrlen(","));
    write(fd, sGrade,customStrlen(sGrade));
    write(fd, "\n",customStrlen("\n"));
    //close(fd);
}