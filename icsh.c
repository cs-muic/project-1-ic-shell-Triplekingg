#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <sys/wait.h>
#include<unistd.h>

char cmd[256]; //command
char *args[32]; //scriptCommands and args split into array of strings
char prev[256]; //stores previous command
int found = 0; //indicates whether the script that user input exists or not
int argNo; //stores number of arguments from user when starting shell
char *file; // stores script name
char path[200]; //stores working directory path
char *scriptCommands[256][256] = {""}; // stores each line of scriptCommands from script
int lines; // number of lines from the script

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

void getCommand() {
    memset(cmd, 0, sizeof(cmd)); //deleting the command that was stored in cmd before
    printf("icsh $\t");
    fgets(cmd, 100, stdin);
    int len = strlen(cmd);
    cmd[len - 1] = '\0';
}

void getCommandFromScript(int line) {
    memset(cmd, 0, sizeof(cmd)); //deleting the command that was stored in cmd before
    strcpy(cmd,scriptCommands[line]);
}

//checks whether the script exists
void scriptFinder(char *f){
    getcwd(path, 200);
    strcat(path,"/");
    strcat(path,f);
    if(access( path, F_OK ) != -1)
    {
        found=1;
        readScript(f);
        startShell();
    }
    else
    {
        found=0;
        printf("file is not found\n");
    }

}

void readScript(char* script){
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
            if(c=='\n'){
                each[i] = '\0';
            }
        }
        c = fgetc(ptrScript);
        strcpy(scriptCommands[line],each);
        memset(each, 0, sizeof(each));
        line++;
    }
    lines = line;
}

void startShell() {
    if(found==0){
        while (1) {
            getCommand();
            if (strcmp("!!", cmd)) {
                if (strstr(cmd, "echo")) {
                    memset(prev, 0, sizeof(prev));
                    memcpy(prev, cmd, strlen(cmd));
                }
            }
            splitCmd(cmd);
            if (!strcmp("", cmd)) {
                continue;
            } else if (!strcmp("exit", args[0])) {
                systemExit();
                break;
            } else if (!strcmp("echo", args[0])) {
                echo(args);
            } else if (!strcmp("!!", args[0])) {
                printf("%s\n", prev);
                if(strstr(prev,"echo")){
                    splitCmd(prev);
                    echo(args);
                }
            } else {
                printf("bad command\n");
            }
        }
    }
    if(found==1){
        for(int i = 0; i<lines;i++){
            getCommandFromScript(i);
            if (strcmp("!!", cmd)) {
                if (strstr(cmd, "echo")) {
                    memset(prev, 0, sizeof(prev));
                    memcpy(prev, cmd, strlen(cmd));
                }
            }
            splitCmd(cmd);
            if (!strcmp("", cmd)) {
                continue;
            } else if (!strcmp("exit", args[0])) {
                systemExit();
                break;
            } else if (!strcmp("echo", args[0])) {
                echo(args);
            } else if (!strcmp("!!", args[0])) {
                printf("%s\n", prev);
                if(strstr(prev,"echo")){
                    splitCmd(prev);
                    echo(args);
                }
            } else {
                printf("bad command\n");
            }
        }
    }

}


void splitCmd(char *c) {
    int i = 0;
    char *p = strtok(c, " ");
    while (p != NULL) {
        args[i++] = p;
        p = strtok(NULL, " ");
    }
}


int main(int argc, char *argv[]) {
    if(argc>1){
        argNo = argc;
        file = malloc(sizeof(char)* strlen(argv[1]));
        strcpy(file,argv[1]);
        scriptFinder(file);
    } else{
        printf("Starting IC shell\n");
        startShell();
    }
    return 0;
}


