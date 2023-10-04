#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <netdb.h>

#include <string.h>

#include <time.h>



extern int errno;

int port;



int robin(char *adress); 

int nobil(int coins, char *adress);

int serif(char *adress);



int main (int argc, char *argv[])

{

    char role[10]; //rolul clientului trimis ca argument 

    int coins=0;  //numarul de banuti pentru nobil 



    if(argc < 4 || argc >5)

    {

        printf ("Sintaxa: %s <adresa_server> <port> <rol> <bani(daca rol = nobil)>\n", argv[0]);

        return -1;

    }

    else if(argc == 4) ///ori rol de serif, ori rol de robin

    {

        if(strcmp(argv[3], "serif") != 0 && strcmp(argv[3], "robin") !=0) ///daca al 4-lea argument e serif sau robin 

        {

            printf ("Sintaxa: %s <adresa_server> <port> <rol> <bani(daca rol = nobil)>\n", argv[0]);

            return -1;

        }

    }

    else if(argc == 5) //daca are 5 argumente, al 4-lea e "nobil", iar al 5-lea e nr-ul de bani

    {

        if(strcmp(argv[3], "nobil") != 0) ///daca al 4-lea argument e nobil

        {

            printf ("Sintaxa: %s <adresa_server> <port> <rol> <bani(daca rol = nobil)>\n", argv[0]);

            return -1;

        }

        else coins = atoi(argv[4]); //daca e nobil se preia nr de bani

    }



    if(coins > 10000) //daca are mai mult de max de bani se deconecteaza

    {

        printf("Prea multi bani. Deconectare.\n");

        exit(0);

    }

    

    strcpy(role, argv[3]); //preluare rol

    port = atoi (argv[2]); //preluare port

  

    if(strcmp(role, "robin") == 0) //daca e robin apeleaza fct pt robin

    {

        robin(argv[1]);

       

    }

    else if(strcmp(role, "serif") == 0) //daca e serif apeleaza fct pt serif

    {

        serif(argv[1]);

       

    }

    else  ///daca e nobil apeleaza fct pt nobil

    {

        nobil(coins, argv[1]);

    }

}





int robin(char *adress)

{

    while(1)

    {

        int sd;

        struct sockaddr_in server;

        

        if((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) //creare socket

        {

            perror ("Eroare la socket().\n");

            return errno;

        }

        //populare structura

        server.sin_family = AF_INET;

        server.sin_addr.s_addr = inet_addr(adress);

        server.sin_port = htons (port);



        if(connect (sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) //conectare la server

        {

            perror ("[client]Eroare la connect().\n");

            return errno;

        }

        

        printf("Sunt Robin Hood si asta e padurea mea!\n");

        fflush(stdout);



        int coins = -101; ///codificare nr bani robin, pt recunoastere rol robin 



        if(write(sd, &coins, sizeof(int)) <= 0) //trimitere nr bani catre server 

        {

            perror ("[client]Eroare la write() spre server.\n");

            return errno;

        }

        

        if(read(sd, &coins, sizeof(int)) < 0) //primire nr bani inapoi 

        {

            perror ("[client]Eroare la read() de la server.\n");

            return errno;

        }

        printf("Seriful a fost in padure...Revin in 15 secunde\n");

         sleep(15);

         

        close(sd);

    }

}



int serif(char *adress)

{

    srand(time(NULL)); ///se genereaza un random seed pentru generarea nr-ului de secunde pentru serif

    printf("Eu sunt seriful de Nottingham!\n");

    fflush(stdout);

    while(1)

    {

        int timesec = rand() %60 + 1; ///nr-ul de secunde 

        printf("O sa fac o razie in %d secunde... \r\n", timesec);

        fflush(stdout);

        sleep(timesec); ///asteapta timesec pentru a se conecta si reconecta 



        int sd;

        struct sockaddr_in server;

        

        if((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

        {

            perror ("Eroare la socket().\n");

            return errno;

        }



        server.sin_family = AF_INET;

        server.sin_addr.s_addr = inet_addr(adress);

        server.sin_port = htons (port);



        if(connect (sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)

        {

            perror ("[client]Eroare la connect().\n");

            return errno;

        }



        int coins = -103; ///codificare coins pentru serif 



        if(write(sd, &coins, sizeof(int)) <= 0)

        {

            perror ("[client]Eroare la write() spre server.\n");

            return errno;

        }

        

        if(read(sd, &coins, sizeof(int)) < 0)

        {

            perror ("[client]Eroare la read() de la server.\n");

            return errno;

        }

        printf("Am facut o razie!...Planning the next one!...\n");

        close(sd);

    }

}





int nobil(int coins, char *adress)

{

    printf("nobil: Am %d galbeni in punguta\n", coins); ///numarul de banuti 

    fflush(stdout);



    while(coins > 0)

    {

        int sd;

        struct sockaddr_in server;



        if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

        {

            perror ("Eroare la socket().\n");

            return errno;

        }



        server.sin_family = AF_INET;

        server.sin_addr.s_addr = inet_addr(adress);

        server.sin_port = htons (port);

        

        if(connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)

        {

            perror ("[client]Eroare la connect().\n");

            return errno;

        }



        int coinsammount = coins; ///copie la nr-ul de bani pentru cazul cand "padurea este prea aglomerata"



        if(write(sd,&coins,sizeof(int)) <= 0)

        {

            perror ("[client]Eroare la write() spre server.\n");

            return errno;

        }



        if(read(sd, &coins, sizeof(int)) < 0)

        {

            perror ("[client]Eroare la read() de la server.\n");

            return errno;

        }

        if(coins == 1000000)  ///daca sever ul trimite mai mult, atunci exista prea multi nobili

        {

            printf("Cam aglomerata padurea azi...stau la rand!\n");

            coins = coinsammount;

            sleep(5);
        }

        else if(coinsammount == coins) ///daca serverul trimite acelasi nr de bani, nobilul a trecut padurea 
        {

            printf("nobil: Am trecut prin padure! Peace Robin!\n");
            exit(0);

        }

        else ///robin a luat bani
        {

            printf ("nobil: Am ramas cu %d galbeni!\n", coins);
            sleep(5);

        }

        close (sd);

    }

    if(coins <= 0 ) printf("Am ramas fara galbeni!...Blestemat fie Robin Hood!\n"); ///ramane nobilul fara bani 

}