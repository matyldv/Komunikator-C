#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdbool.h>


typedef struct uzytkownicy{
    char login[20];
    char haslo[20];
    char wiadomosci[50][256];
    int pid;
}uzyt;

typedef struct grupy{
    char nazwa[50];
    int czlonkowie[50];
}grp;

struct msgbuf{
    long mtype;
    int pid;
    char text1[256];
    char text2[256];
    int zalogowany;
    int liczby;
}msg;

uzyt uzytkownik[50];
grp grupa[20];

int main(){
    int ilosc_u = 0;
    int ilosc_g = 0;
    //wczytanie plikow
    
    FILE *plik = fopen("uzytkownicy.txt", "r");
    if(plik == NULL){
        printf("Nie mozna otworzyc pliku uzytkownicy\n");
        return 0;
    }
    
    while(!feof(plik)){
        fscanf(plik, "%s %s", uzytkownik[ilosc_u].login, uzytkownik[ilosc_u].haslo);
        ilosc_u++;
    }
    
    fclose(plik);
    
    plik = fopen("grupy.txt", "r");
    if(plik == NULL){
        printf("Nie mozna otworzyc pliku grupy\n");
        return 0;
    }
    
    while(!feof(plik)){
        fscanf(plik, "%s", grupa[ilosc_g].nazwa);
        ilosc_g++;
    }
    
    fclose(plik);
    ilosc_u--;
    ilosc_g--;
    
    for(int i = 0; i<ilosc_u; i++){
       // printf("%s %s \n", uzytkownik[i].login, uzytkownik[i].haslo);
    }
    
    int zalogowani[50] = {0};
    int tempx = 0, dogrp, dousr, dla,temppid;
    bool szukanie;
    int mid = msgget(0x123, 0644 | IPC_CREAT);
    while(true){
    msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
    
    switch(msg.mtype){
        case 1:
            for(int i = 0; i < ilosc_u; i++){
                if(!strcmp(msg.text1,uzytkownik[i].login))
                    if(!strcmp(msg.text2,uzytkownik[i].haslo)){
                        zalogowani[i] = msg.pid;
                        uzytkownik[i].pid = msg.pid;
                        msg.zalogowany = 1;
                       // msg.id = i;
                    }
                }
                
            //msg.mtype = 2;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
            
        case 3:
            for(int i = 0; i < ilosc_u; i++){
                if(zalogowani[i] == msg.pid) zalogowani[i] = 0;
                msg.zalogowany = 0;
                break;
            }
            //msg.mtype = 4;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
            
        case 5:
            strcpy(msg.text1, "");
            for(int i = 0; i < ilosc_u;i++){
                if(zalogowani[i] != 0) {
                    strcat(msg.text1, uzytkownik[i].login);
                    strcat(msg.text1, "\n");
                }
            }
            //msg.mtype = 6;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            
            break;
        case 7:
            strcpy(msg.text1, "");
            for(int i = 1; i <= ilosc_g;i++){
                    strcat(msg.text1, grupa[i-1].nazwa);
                    strcat(msg.text1, "\n");
                }
            msg.liczby = ilosc_g;
            //msg.mtype = 8;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
        case 9:
            temppid = msg.pid;
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == temppid) dla = i;
            }
            grupa[msg.liczby-1].czlonkowie[dla] = 1;
            //msg.mtype = 10;
            strcpy(msg.text1, "");
            strcat(msg.text1, grupa[msg.liczby-1].nazwa);
            msgsnd(mid, &msg, sizeof(msg)+1,0);            
            break;
            
        case 11:
            strcpy(msg.text1, "");
            for(int i = 0; i < ilosc_u;i++){
                if(grupa[msg.liczby-1].czlonkowie[i] == 1) strcat(msg.text1, uzytkownik[i].login);
                    strcat(msg.text1, "\n");
                }
            msg.liczby = ilosc_g;
            //msg.mtype = 12;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
            
        case 13:
            temppid = msg.pid;
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == temppid) dla = i;
            }
            tempx = 0;
            strcpy(msg.text1, "");
            for(int i = 0; i < ilosc_g; i++){
                if(grupa[i].czlonkowie[dla] == 1){
                    tempx++;
                    strcat(msg.text1, grupa[i].nazwa);
                    strcat(msg.text1, "\n");
                }
               
            } 
            msg.liczby = ilosc_g;
                //msg.mtype = 14;
                msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
            
        case 15:
            temppid = msg.pid;
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == temppid) dla = i;
            }
            grupa[msg.liczby-1].czlonkowie[dla] = 0;
            msg.liczby = 0;
           // msg.mtype = 16;
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
            
        case 17:
            dogrp = msg.liczby-1;
            char od[20];
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == msg.pid) strcpy(od, uzytkownik[i].login);
            }
            
            for(int i = 0; i<ilosc_u; i++){
                if(grupa[dogrp].czlonkowie[i] != 0){
                    int j = 0;
                    while(strcmp(uzytkownik[i].wiadomosci[j],""))j++;
                    
                    strcat(uzytkownik[i].wiadomosci[j],"Wiadomosc od uzytkownika ");
                    strcat(uzytkownik[i].wiadomosci[j],od);
                    strcat(uzytkownik[i].wiadomosci[j],"\n");
                    strcat(uzytkownik[i].wiadomosci[j],msg.text1);
                    strcat(uzytkownik[i].wiadomosci[j],"\n");
                    //uzytkownik[i].wiadomosci[j] = msg.text1;
                    
                    //printf("%s\n",uzytkownik[i].wiadomosci[j]);
                    
                }                
            }
            
//             if(strcmp(uzytkownik[0].wiadomosci[0],"")) printf("Tak\n");
//                else printf("Nie\n"); //tak
//             if(strcmp(uzytkownik[0].wiadomosci[1],"")) printf("Tak\n");
//                else printf("Nie\n");//nie
//              
            break;
            
        case 20:
            temppid = msg.pid;
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == temppid) dla = i;
            }
           // printf("%s\n",uzytkownik[dla].login);
            strcpy(msg.text1, "");
            szukanie = true;
            int i = 0;
            while(szukanie){
                
                if(!strcmp(uzytkownik[dla].wiadomosci[i],""))
                {
                    strcat(msg.text1,"Brak wiadomosci\n");
                    szukanie = false;
                }
                else{
                    strcat(msg.text1,uzytkownik[dla].wiadomosci[i]);
                    strcat(msg.text1,"\n");
                    
                    strcpy(uzytkownik[dla].wiadomosci[i],"");
                    i++;
                }
                
            }
            msgsnd(mid, &msg, sizeof(msg)+1,0);
            break;
           
    
        case 21:
            dousr = msg.liczby-1;
            char odusr[20];
            for(int i = 0; i<ilosc_u; i++){
                if(zalogowani[i] == msg.pid) strcpy(odusr, uzytkownik[i].login);
            }
            
            
                    int j = 0;
                    while(strcmp(uzytkownik[dousr].wiadomosci[j],""))j++;
                    
                    strcat(uzytkownik[dousr].wiadomosci[j],"Wiadomosc od uzytkownika ");
                    strcat(uzytkownik[dousr].wiadomosci[j],odusr);
                    strcat(uzytkownik[dousr].wiadomosci[j],"\n");
                    strcat(uzytkownik[dousr].wiadomosci[j],msg.text1);
                    strcat(uzytkownik[dousr].wiadomosci[j],"\n");
                    //uzytkownik[i].wiadomosci[j] = msg.text1;
                    
                    //printf("%s\n",uzytkownik[i].wiadomosci[j]);

            break;
    
    }
    
    

    }
    return 0;
}
