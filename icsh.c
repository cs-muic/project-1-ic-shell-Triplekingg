#include <stdio.h>
#include <sys/file.h>
#include "string.h"
#include <stdlib.h>
#include <sys/wait.h>
#include<unistd.h>
#include<signal.h>
#include <ctype.h>

char cmd[256]; //command
char *args[32]; //scriptCommands and args split into array of strings
char prev[256]; //stores previous command
int found = 0; //indicates whether the script that user input exists or not
int argNo; //stores number of arguments from user when starting shell
char *file; // stores script name
char path[200]; //stores working directory path
char *scriptCommands[256][256] = {""}; // stores each line of scriptCommands from script
int lines; // number of lines from the script
int childId;//stores pid of child process that will be used for signal handling(to suspend/kill that process)
int childExitStatus;//stores exit status of child process and will be used when inputting "echo $?"
char out[100];
char* outputFile;// stores name of file where output will be directed
char redirectedCmd[256]; //stores command to be used for redirection
int isRedirected = 0; //store whether command is to be redirected or not


void startShell();

void getCommand();

void splitCmd();

void echo();

void systemExit();

void readScript();

void systemExit() { //For exit
    int num = atoi(args[1]);
    int converted = WEXITSTATUS(num);
    if (num > 255) {
        printf("Bye\n");
        exit(converted);
    } else {
        printf("Bye\n");
        exit(num);
    }

}

void echo() { //For echo command
    int length = sizeof(args) / sizeof(args[0]);
    for (int i = 1; i < length; i++) {
        if (args[i] == NULL) break;
        printf("%s ", args[i]);
    }
    printf("\n");
}

//get command from user
void getCommand() {
    memset(cmd, 0, sizeof(cmd)); //deleting the command that was stored in cmd before
    printf("icsh $\t");
    fgets(cmd, 100, stdin);
    int len = strlen(cmd);
    cmd[len - 1] = '\0';
}

//get command from the script
void getCommandFromScript(int line) {
    memset(cmd, 0, sizeof(cmd)); //deleting the command that was stored in cmd before
    strcpy(cmd, scriptCommands[line]);
}

void doExternalCommand(int redirected){
    //https://stackoverflow.com/questions/29154056/redirect-stdout-to-a-file for this if condition
    if(redirected){
        int filefd = open(outputFile, O_WRONLY|O_CREAT, 0666);
        if (!fork()) {
            close(1);//Close stdout
            dup(filefd);
            if (execvp(args[0], args) == -1) { //If the execution fails, it is a bad command
                printf("bad command\n");
                kill(childId, SIGKILL);
            }
        } else {
            close(filefd);
            wait(NULL);
        }
    } else{
        //If it is an external command, fork a new process
        int id = fork();
        if (id == 0) {// Execute the external program
            childId = getpid();
            if (execvp(args[0], args) == -1) { //If the execution fails, it is a bad command
                printf("bad command\n");
                kill(childId, SIGKILL);
            }
        }
//    wait(NULL);
        int status;
        waitpid(childId, &status, 0);
        childId = NULL;
        if (WIFEXITED(status)) {
            childExitStatus = WEXITSTATUS(status);
        }
    }
}


//checks whether the scriptfile exists
void scriptFinder(char *f) {
    getcwd(path, 200);
    strcat(path, "/");
    strcat(path, f);
    if (access(path, F_OK) != -1) {
        found = 1;
        readScript(f);
        startShell();
    } else {
        found = 0;
        printf("file is not found\n");
    }
}

//read from sh file
void readScript(char *script) {
    //To access the script
    FILE *ptrScript;
    char c;

    // Open script file
    ptrScript = fopen(script, "r");
    if (ptrScript == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    // Read contents from script
    int line = 0;
    c = fgetc(ptrScript);
    while (c != EOF) {
        int i = 0;
        char each[32];
        while (c != '\n') {
            if (c == EOF) {
                break;
            }
            each[i] = c;
            i++;
            c = fgetc(ptrScript);
            if (c == '\n') {
                each[i] = '\0';
            }
        }
        c = fgetc(ptrScript);
        strcpy(scriptCommands[line], each);
        memset(each, 0, sizeof(each));
        line++;
    }
    lines = line;
}

void handle_sigint(int sig) {
    if (sig == 2) {
        kill(childId, sig);
        printf("Process Terminated\n");
        startShell();
    }
}

char* removeLeadingSpaces(char* str)
{
    static char str1[99];
    int count = 0, j, k;
    while (str[count] == ' ') {
        count++;
    }
    for (j = count, k = 0;
         str[j] != '\0'; j++, k++) {
        str1[k] = str[j];
    }
    str1[k] = '\0';
    return str1;
}

void handle_sigstp() {
    kill(childId, SIGTSTP);
    printf("Process Suspended\n");
    startShell();
}

void startShell() {
    //if interactive mode
    if (found == 0) {
        while (1) {
            isRedirected = 0;
            getCommand();
            if (!strcmp("echo $?", cmd)) {
                printf("Exit status of previous process is %d\n", childExitStatus);
                continue;
            }
            if(strstr(cmd,">")){
                isRedirected = 1;
                memset(out, 0, sizeof(out));
                //split command and output file
                splitCmd(cmd,">");
                //store output file
                memcpy(out, args[1], strlen(args[1]));
                //reset previous redirected command
                memset(redirectedCmd, 0, sizeof(redirectedCmd));
                //store redirected command
                memcpy(redirectedCmd,args[0], strlen(args[0]));
                //remove leading spaces from outputfile name
                outputFile = removeLeadingSpaces(out);
                //reset command
                memset(cmd, 0, sizeof(cmd));
                //split the redirected command in case of arguments and store in args
                splitCmd(redirectedCmd," ");
                doExternalCommand(isRedirected);
                continue;
            }
            if (strcmp("!!", cmd)) {
                if (strstr(cmd, "echo")) {
                    memset(prev, 0, sizeof(prev));
                    memcpy(prev, cmd, strlen(cmd));
                }
            }
            splitCmd(cmd," ");
            if (!strcmp("", cmd)) {
                continue;
            } else if (!strcmp("exit", args[0])) {
                systemExit();
                break;
            } else if (!strcmp("echo", args[0])) {
                echo(args);
            } else if (!strcmp("!!", args[0])) {
                printf("%s\n", prev);
                if (strstr(prev, "echo")) {
                    splitCmd(prev," ");
                    echo(args);
                }
            } else {
                doExternalCommand(0);
            }
        }
    }
    //if batch mode
    if (found == 1) {
        for (int i = 0; i < lines; i++) {
            getCommandFromScript(i);
            if (strcmp("!!", cmd)) {
                if (strstr(cmd, "echo")) {
                    memset(prev, 0, sizeof(prev));
                    memcpy(prev, cmd, strlen(cmd));
                }
            }
            splitCmd(cmd," ");
            if (!strcmp("", cmd)) {
                continue;
            } else if (!strcmp("exit", args[0])) {
                systemExit();
                break;
            } else if (!strcmp("echo", args[0])) {
                echo(args);
            } else if (!strcmp("!!", args[0])) {
                printf("%s\n", prev);
                if (strstr(prev, "echo")) {
                    splitCmd(prev, " ");
                    echo(args);
                }
            } else {
                printf("bad command\n");
            }
        }
    }
}

//splits command into array of strings
void splitCmd(char *c, char *at) {
    memset(args, 0, 32);
    int i = 0;
    char *p = strtok(c, at);
    while (p != NULL) {
        args[i++] = p;
        p = strtok(NULL, at);
    }
}


int main(int argc, char *argv[]) {
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigstp);
    //if user enters argument
    if (argc > 1) {
        argNo = argc;
        file = malloc(sizeof(char) * strlen(argv[1]));
        strcpy(file, argv[1]);
        scriptFinder(file);
    } else {
        //if user does not enter argument
        printf("Starting IC shell\n");
        startShell();
    }
    return 0;
}


