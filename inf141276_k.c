#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

struct msgbuf{
    long mtype;
    int pid;
    char text1[256];
    char text2[256];
    int zalogowany;
    int liczby;
    
}msg;


int main(){
    
    int mid = msgget(0x123, 0644 | IPC_CREAT);
    int pid = getpid();
    msg.zalogowany = 0;
    while(true){
    while(!msg.zalogowany){
    printf("Logowanie\nPodaj login: ");
    scanf("%s",msg.text1);
    printf("Podaj haslo: ");
    scanf("%s",msg.text2);
    msg.pid = pid;
    msg.mtype = 1;
    msgsnd(mid, &msg, sizeof(msg)+1,0);
    msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
    if(msg.zalogowany==1) printf("Zalogowano\n");
    else printf("Blad logowania! Sprobuj ponownie\n\n");
    }
    
    while(msg.zalogowany){
        printf("Co chcesz zrobic?\n1. Wylogowanie \n2. Lista zalogowanych uzytkowniko \n3. Lista uzytkownikow w wybranej grupie\n4. Zapis do grupy \n5. Wypisanie z grupy \n6. Dostepne grupy \n7. Wyslanie wiadomosci do grupy \n8. Wyslanie wiadomosci do uzytkownika \n9. Odebranie wiadomosci\n");
        int wybor;
        scanf("%d", &wybor);
        switch(wybor){
            case 1:
                msg.mtype = 3;
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                if(msg.zalogowany == 0) printf("Wylogowano!\n\n"); 
                break;
            case 2:
                msg.mtype = 5;
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                
                printf("Lista zalogowanych uzytkownikow:\n");
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s\n", msg.text1);
                break;
            case 3:
                msg.mtype = 7;
                printf("Uzytkownikow ktorej grupy chcesz wyswietlic?\n");
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s\n", msg.text1);
                scanf("%d", &wybor);
                if(wybor >= 1 && wybor <= msg.liczby){
                    msg.liczby = wybor;
                    msg.mtype = 11;
                    msgsnd(mid, &msg, sizeof(msg)+1,0);
                    msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                    printf("Uzytkownicy tej grupy to:\n%s\n", msg.text1);
                }
                else printf("Brak takiej grupy!\n");
                break;
                break;
            case 4:
                msg.mtype = 7;
                printf("Do ktorej grupy chcesz sie zapisac?\n");
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s\n", msg.text1);
                scanf("%d", &wybor);
                if(wybor >= 1 && wybor <= msg.liczby){
                    msg.liczby = wybor;
                    msg.pid = pid;
                    msg.mtype = 9;
                    msgsnd(mid, &msg, sizeof(msg)+1,0);
                    msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                    printf("Zostales dopisany do grupy %s\n", msg.text1);
                }
                else printf("Brak takiej grupy!\n");
                
                
                break;
            case 5:
                msg.mtype = 13;
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                if(msg.liczby == 0) printf("Nie nalezysz do zadnej grupy\n");
                else{
                    printf("Z ktorej grupy chcesz zostac usuniety?\n");
                    printf("%s\n",msg.text1);
                    scanf("%d", &wybor);
                    if(wybor >= 1 && wybor <= msg.liczby){
                    msg.liczby = wybor;
                    msg.pid = pid;
                    msg.mtype = 15;
                    msgsnd(mid, &msg, sizeof(msg)+1,0);
                    msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                    if(msg.liczby == 0) printf("Zostales usuniety z grupy\n");
                    
                    }
                }
                break;
            case 6:
                msg.mtype = 7;
                printf("Dostepne grupy to:\n");
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s\n", msg.text1);
                
                break;
            case 7:
                msg.mtype = 7;
                printf("Do ktorej grupy chcesz wyslac wiadomosc?\n");
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s\n", msg.text1);
                scanf("%d", &wybor);
                    if(wybor >= 1 && wybor <= msg.liczby){
                    msg.liczby = wybor;
                    printf("Podaj tresc wiadomosci:\n");
                    
                    strcpy(msg.text1, "");
                
                    char napis[256];
                    char testowy[256];
                    strcpy(testowy, ".");
                    bool skan = true;
                    while(skan){
                      scanf("%s", napis);
                    if(!strcmp(napis,testowy)) {
                          
                        skan = false;
                    }
                    else  {strcat(msg.text1, napis);
                        strcat(msg.text1," ");
                    }
                    }
                    //printf("%s",msg.text1);

                    msg.mtype = 17;
                    msg.pid = getpid();
                    msgsnd(mid, &msg, sizeof(msg)+1,0);
                    //msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                    //printf("%s",msg.text1);

                    
                    
                    }
                    else{
                        printf("Brak takiej grupy!");
                        break;
                    }
                
                
                break;
            case 8:
                printf("Do ktorego uzytkownika chcesz wyslac wiadomosc?\n");
                scanf("%d", &wybor);
                    if(wybor >= 1 && wybor <= 9){
                    msg.liczby = wybor;
                    printf("Podaj tresc wiadomosci:\n");
                    
                    strcpy(msg.text1, "");
                
                    char napis[256];
                    char testowy[256];
                    strcpy(testowy, ".");
                    bool skan = true;
                    while(skan){
                      scanf("%s", napis);
                    if(!strcmp(napis,testowy)) {
                          
                        skan = false;
                    }
                    else  {strcat(msg.text1, napis);
                        strcat(msg.text1," ");
                    }
                    }
                    //printf("%s",msg.text1);
                    msg.mtype = 21;
                    msg.pid = getpid();
                    msgsnd(mid, &msg, sizeof(msg)+1,0);
                    //msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                    //printf("%s",msg.text1);

                    
                    
                    }
                    else{
                        printf("Brak takiego uzytkownika!");
                        break;
                    }
                break;
            case 9:
                msg.mtype = 20;
                msg.pid = getpid();
                msgsnd(mid, &msg, sizeof(msg)+1,0);
                msgrcv(mid, &msg, sizeof(msg)+1, 0, 0);
                printf("%s",msg.text1);
                
                break;
            default:
                printf("Wybrano bledna wartosc!\n\n");
                break;
        }
        
        
        
        
        
        
    }
    
    
    

    }
    return 0;
}
