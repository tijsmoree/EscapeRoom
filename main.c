#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

HANDLE hSerial;

#define BAUDRATE CBR_9600

int whichCOM() {
    int c;

    printf("What COM-port? ");
    scanf("%d", &c);
    printf("\n");

    return c;
}

void initSio(HANDLE hSerial){
    COMMTIMEOUTS timeouts = {0};
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        /*//error getting state*/
        printf("Boss? I can't get to the Arduino...\n");
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams)){
        /*//error setting serial port state*/
        printf("Boss? I think I'm looking at the wrong place!\n");
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts)){
    /*//error occureed. Inform user*/
        printf("Boss, error! I don't understand.\n");
    }
}

HANDLE openCom() {
    HANDLE hSerial;

    char com[] = "COMx";
    com[3] = whichCOM() + 48;

    hSerial = CreateFile(com,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if(hSerial == INVALID_HANDLE_VALUE){
        if(GetLastError() == ERROR_FILE_NOT_FOUND){
            /*//serial port does not exist. Inform user.*/
            printf("Chief! There is something wrong with the connection to %s.\n", com);
        }

        /*//some other error occurred. Inform user.*/
        printf("Boss, error! I don't understand.\n");
    }

    initSio(hSerial);

    return hSerial;
}

void closeCom(HANDLE hSerial) {
    CloseHandle(hSerial);
}

int receiveByte() {
    unsigned char buffRead[BUFSIZ + 1];
    DWORD dwBytesRead = 0;

    buffRead[0] = 0;

    while(!buffRead[0])
        ReadFile(hSerial, buffRead, 1, &dwBytesRead, NULL);

    return buffRead[0];
}

void printTime(time_t start) {
    int diff = (int) difftime(time(NULL), start);
    int min = diff / 60;
    int sec = diff % 60;

    printf("They escaped after %d minutes and %d seconds.\n", min, sec);
}

int main() {
    int i;
    int q = 0;
    time_t start = time(NULL);
    char music[100];
    FILE *file;
    
    hSerial = openCom();
    initSio(hSerial);

    file = fopen("play.txt", "r");
    fgets(music, sizeof(music), file);
    fclose(file);

    while(q != 1) {
        q = receiveByte();
        if(q == 4) {
            printf("They guessed the following wrong number: ");
            for(i = 0; i < 7; i++) {
                printf("%d", receiveByte() - 48);
            }
            printf(".\n");
        } else if(q == 5) {
            printf("They guessed the right phone number!\n");
        }
    }

    printTime(start);
    system(music);
    getch();

    return 0;
}