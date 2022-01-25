#include <stdio.h>
#include "string.h"

char cmd[100];

void getCommand(){
    printf("icsh $\t");
    fgets(cmd,100,stdin);
    int len = strlen(cmd);
    cmd[len-1] = '\0';
}

int main() {
    printf("Starting IC shell\n");
    getCommand();
    if(!strcmp(cmd,"exit")){
        printf("bye\n");
    }
    return 0;
}


