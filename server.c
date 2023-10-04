#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>

#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <signal.h>

#include <pthread.h>



#define PORT 1234



extern int errno;



static void *treat(void *); ///functia threadului

void dothejob(int client, int IdThread);  ///tratarea clientului 



typedef struct 

{

    pthread_t IdThread;

    int thcount;

}Thread;



Thread *threadpool;



int sd;

int nthreads;

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;



int nobles=0; //cati nobili sunt la un moment

int isSherif = 0, isRobin = 0;  ///retin daca sunt robin sau seriful in padure



int main(int argc, char *argv[])

{

    struct sockaddr_in server;

    

    if(argc<2)

    {

        fprintf(stderr,"Eroare: Primul argument este numarul de fire de executie...");

	    exit(1);

    }



    nthreads=atoi(argv[1]); //nr de threaduri 



    if(nthreads <=0) ///daca e nr neg 

	{

        fprintf(stderr,"Eroare: Numar de fire invalid...");

	    exit(1);

	}



    threadpool = calloc(sizeof(Thread), nthreads); ///alocare zona de mem pentru nr de threaduri 



    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)

    {

        perror("[server]Eroare la socket().\n");

        return errno;

    }



    int on = 1;

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



    bzero(&server, sizeof(server)); ///setare biti pe 0 



    server.sin_family = AF_INET;

    server.sin_addr.s_addr = htonl(INADDR_ANY);

    server.sin_port = htons(PORT);



    if(bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)

    {

        perror ("[server]Eroare la bind().\n");

        return errno;

    }



    if(listen(sd, 2) == -1)

    {

        perror("[server]Eroare la listen().\n");

        return errno;

    }



    ///printf("Numar de threaduri: %d\n", nthreads);



    int i;

    for(i=0;i<nthreads;i++) //creeare threaduri 

    {

        void *treat(void *);

        pthread_create(&threadpool[i].IdThread, NULL, &treat, (void*)i);

    }

    for( ; ;) ///pentru executia threadurilor 

    {

        printf("[server] Astept la portul %d...\n", PORT);

        pause();

    }



}





void *treat(void *arg) ///deschide threadul deja creat

{

    int client;

    

    struct sockaddr_in from;

    bzero(&from, sizeof(from));

    printf("[thread]Thread %d pornit...\n", (int) arg); 

    fflush(stdout);



    for( ; ; )

    {

        int length = sizeof(from);



        if((client = accept(sd, (struct sockaddr *)&from, &length)) <0)

        {

            perror("[thread] Eroare la accept().\n");

        }



        threadpool[(int)arg].thcount++; ///mareste contorul de threadrui 



        dothejob(client, (int)arg);

        close(client);

    }



}



void dothejob(int client, int IdThread)

{

    int coins;

    if(read(client, &coins, sizeof(int)) <= 0)

	{

	    printf("[Thread %d]\n",IdThread);

	    perror ("Eroare la read() de la client.\n");

	}

	printf ("[Thread %d]Mesajul a fost receptionat...%d\n",IdThread, coins);



    if(coins == -101) ///clientul este robin 

    {

        isRobin = 1; ///exista robin conectat la server 

        printf("[thread_robin]Robin e in padure!\n");

        while(isRobin == 1)

        {   

            if(isSherif == 1) //daca apare seriful 

            {  printf("Am intrat");

                isRobin = 0; ///se deconecteaza robin 

                printf("[thread_robin]Vine seriful...am plecat!\n");

                if (write (client, &coins, sizeof(int)) <= 0)

	            {

		            printf("[Thread %d] ",IdThread);

		            perror ("[Thread]Eroare la write() catre client.\n");

	            }

            }

        }

    }

    else if(coins == -103) ///daca e serif 

    {

        isSherif = 1; ///conectare serif la server 

        sleep(1);

        printf("[thread_serif]Seriful e in padure!\n");

        if (write (client, &coins, sizeof(int)) <= 0) ///trimite acelasi nr de bani 

        {

            printf("[Thread %d] ",IdThread);

            perror ("[Thread]Eroare la write() catre client.\n");

        }

        isSherif = 0; ///se deconecteaza 

    }

    else  ///daca e nobil

    {   nobles++; ///creste nr de nobili 

        if(nobles > 3) ///daca sunt mai mult de 3 nobili 

        {

            coins= 1000000; ///prea multi nobili in padure, se trimite in client pentru acest caz

            sleep(3); 

            if (write (client, &coins, sizeof(int)) <= 0)

            {

                printf("[Thread %d] ",IdThread);

                perror ("[Thread]Eroare la write() catre client.\n");

            }

            nobles--;

        }

        else /// daca este loc in padure 

        {

            if(isRobin == 1) ///daca e robin in padure 

            {

                coins = coins - 100; ///se scade cant de bani si se trimite la client 



                printf("[Thread %d]Trimitem mesajul inapoi...%d\n",IdThread, coins);

                if (write (client, &coins, sizeof(int)) <= 0)

                {

                    printf("[Thread %d] ",IdThread);

                    perror ("[Thread]Eroare la write() catre client.\n");

                }

                nobles--; ///scade nr de nobili



            }

            else ///daca nu e robin in padure 

            {

                sleep(3); ///se asteapta 3 secunde 

                printf("[Thread %d]Trimitem mesajul inapoi...%d\n",IdThread, coins);

                if (write (client, &coins, sizeof(int)) <= 0) ///trimite la client acelasi nr de bani

                {

                    printf("[Thread %d] ",IdThread);

                    perror ("[Thread]Eroare la write() catre client.\n");

                }

                nobles--;

            }

            

        }



    }

}