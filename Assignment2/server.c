/*
 * Copyright 2017 Yongjia Huang, Yuhan Ye,CMPUT379, University of Alberta, All Rights Reserved.
 * You may use distribut, or modify this code under terms and conditions of the ode of Student Behavior at University of Alberta
 * You may find a copy of the license in this project. Otherwise please contact jajayongjia@gmail.com
 */
#include <stdio.h>
#include <strings.h>   //strlen
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/timeb.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <netinet/in.h>
#include<pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <math.h>
#define START_Y 10
#define START_X 10

struct playerPosition{
    int exist;    // 1 if this player is enabled
    int boardsize;  // user input boardsize
    double updatePeriod; // user input updateperiod
    int MY_PORT;    // user input my_port
    int new_sock;   // sock id
    int id;          // player id
    int x;           // player position x
    int y;            // player position y
    char direction[2];  // player direction
    char move;          // player's input
    int fire; // if fire == 1 -> draw o on screen
    int o[4]; // contains x1,y1,x2,y2
    int score; // 0 in default
};


// This structure is the message pass from server to clients
struct allplayer{
    struct playerPosition players[30];  // contains 30 clients, Max number of client is 30
    int currentIndex;                   // whenever a message is passed to client, it will tell its client its ID
    int death[30];                      // death contains which client  is dead
};
pthread_mutex_t lock;
pthread_mutex_t lock1;
struct allplayer allplayers;
int num_client;

// this function provides signal handling
// exit if the server receives SIGTERM
void signalHandler(int signal){
    if(signal = SIGTERM){
        printf("BYEBYE\n");
        exit(0);
    }
    else{
        printf("catch a signal");
    }

}

//this function clear and initialize the fire information of the current player
void clearfireo(struct playerPosition * currentplayer){
    currentplayer->fire = 0;
    currentplayer->o[0] = -1;
    currentplayer->o[1] = -1;
    currentplayer->o[2] = -1;
    currentplayer->o[3] = -1;
}

void *connection_handler(void *player1)
{
    //Get the socket descriptor
    struct playerPosition currentplayer;
    currentplayer = *(struct playerPosition *)player1;
    int nth = currentplayer.id;
    int sock = currentplayer.new_sock;
    int period,recvreturn;
    period =(int) (allplayers.players[nth].updatePeriod * 1000.0);


    allplayers.currentIndex = nth;


    /*Each Client thread enters a continuely conversion loop */
    while (1){
    allplayers.players[nth] = currentplayer;

    char move;

    // send data to client
    pthread_mutex_lock(&lock1);
    send(sock,&allplayers,sizeof(struct allplayer),0);
    pthread_mutex_unlock(&lock1);
    usleep(period);

    // Now we receive from the client,
    pthread_mutex_lock(&lock);
    recvreturn = recv(sock,&move,sizeof( char),0);
    pthread_mutex_unlock(&lock);

    // if a client is disconnected
    if((recvreturn == 0)||(recvreturn == -1) ){
        allplayers.players[nth].exist = 0;
        puts("Client Exit");
        pthread_exit("");
    }


    // the switch statment below should check all constraint of all the clients,
    // and do the update
     switch(move){
        // decide whether it can move
        bool moveflag = false;
        case '^':
            for (int i=0;i<=num_client;i++){
                    if (allplayers.players[i].exist == 1){
                            if(((allplayers.players[i].x == currentplayer.x)&&(allplayers.players[i].y == currentplayer.y-1))
                            ||((allplayers.players[i].x == currentplayer.x-1)&&(allplayers.players[i].y == currentplayer.y-1)&&(allplayers.players[i].move=='>'))
                            ||((allplayers.players[i].x == currentplayer.x+1)&&(allplayers.players[i].y == currentplayer.y-1)&&(allplayers.players[i].move=='<'))
                            ||((allplayers.players[i].x == currentplayer.x)&&(allplayers.players[i].y == currentplayer.y-2)&&(allplayers.players[i].move=='v'  ))){
                                moveflag = true;
                                break;
                            }
                    }
            }
            if(moveflag == true){
                ;
            }
            else{
                if(currentplayer.y > 1){
                    currentplayer.y-=1;
                    strcpy(currentplayer.direction,"^");
                }
            }
            moveflag = false;
            clearfireo(&currentplayer);
            break;
        case 'v':
             for (int i=0;i<=num_client;i++){
                    if (allplayers.players[i].exist == 1){
                            if(((allplayers.players[i].x == currentplayer.x)&&(allplayers.players[i].y == currentplayer.y+1))
                            ||((allplayers.players[i].x == currentplayer.x-1)&&(allplayers.players[i].y == currentplayer.y+1)&&(allplayers.players[i].move=='>'))
                            ||((allplayers.players[i].x == currentplayer.x+1)&&(allplayers.players[i].y == currentplayer.y+1)&&(allplayers.players[i].move=='<'))
                            ||((allplayers.players[i].x == currentplayer.x)&&(allplayers.players[i].y == currentplayer.y+2)&&(allplayers.players[i].move=='v'  ))){
                                moveflag = true;
                                break;
                            }
                    }
             }
             if(moveflag == true){
                ;
             }
             else{
                if((currentplayer.y) < (currentplayer.boardsize) ){
                    currentplayer.y+=1;
                    strcpy(currentplayer.direction,"v");
                }
            }
            moveflag = false;
            clearfireo(&currentplayer);
            break;
        case '<':
             for (int i=0;i<=num_client;i++){
                    if (allplayers.players[i].exist == 1){
                            if(((allplayers.players[i].x == currentplayer.x-1)&&(allplayers.players[i].y == currentplayer.y))
                            ||((allplayers.players[i].x == currentplayer.x-1)&&(allplayers.players[i].y == currentplayer.y+1)&&(allplayers.players[i].move=='^'))
                            ||((allplayers.players[i].x == currentplayer.x-2)&&(allplayers.players[i].y == currentplayer.y)&&(allplayers.players[i].move=='>'))
                            ||((allplayers.players[i].x == currentplayer.x-1)&&(allplayers.players[i].y == currentplayer.y-1)&&(allplayers.players[i].move=='v'  ))){
                                moveflag = true;
                                break;
                            }
                    }
             }
             if(moveflag == true){
                ;
             }
             else{
                if(currentplayer.x > 1){
                    currentplayer.x-=1;
                    strcpy(currentplayer.direction,"<");
                }
             }
            moveflag = false;
            clearfireo(&currentplayer);
            break;
        case '>':
             for (int i=0;i<=num_client;i++){
                    if (allplayers.players[i].exist == 1){
                            if(((allplayers.players[i].x == currentplayer.x+1)&&(allplayers.players[i].y == currentplayer.y))
                            ||((allplayers.players[i].x == currentplayer.x+1)&&(allplayers.players[i].y == currentplayer.y+1)&&(allplayers.players[i].move=='^'))
                            ||((allplayers.players[i].x == currentplayer.x+2)&&(allplayers.players[i].y == currentplayer.y)&&(allplayers.players[i].move=='<'))
                            ||((allplayers.players[i].x == currentplayer.x+1)&&(allplayers.players[i].y == currentplayer.y-1)&&(allplayers.players[i].move=='v' ) )){
                                moveflag = true;
                                break;
                            }
                    }
             }
             if(moveflag == true){
                ;
             }
             else{
                if((currentplayer.x) < (currentplayer.boardsize)){
                    currentplayer.x+=1;
                    strcpy(currentplayer.direction,">");
                }
             }
            clearfireo(&currentplayer);
            break;


    // enable currentplayer.fire ( =1);
    // give o[4] to currentplayer o[4] contains two bullet position, [x1,y1,x2,y2]
    // do check fire range is in boundary or not
    // also check if other clients current position is in o[4] or not
    // if yes,  death[id] = 1 where id is the id of the died player
    // currentplayer.score +=1
        case 'f':
            currentplayer.fire = 1;
            switch(currentplayer.direction[0]){
                case '^':
                    if(currentplayer.y==1){
                        ;
                    }
                    else if(currentplayer.y==2){
                        currentplayer.o[0] = currentplayer.x;
                        currentplayer.o[1] = 1;
                    }
                    else{
                        currentplayer.o[0] = currentplayer.x;
                        currentplayer.o[1] = currentplayer.y-1;
                        currentplayer.o[2] = currentplayer.x;
                        currentplayer.o[3] = currentplayer.y-2;
                    }
                    break;
                case 'v':
                    if(currentplayer.y == currentplayer.boardsize){
                        ;
                    }
                    else if(currentplayer.y == currentplayer.boardsize-1){
                        currentplayer.o[0] = currentplayer.x;
                        currentplayer.o[1] = currentplayer.boardsize-1;
                    }
                    else{

                        currentplayer.o[0] = currentplayer.x;
                        currentplayer.o[1] = currentplayer.y+1;
                        currentplayer.o[2] = currentplayer.x;
                        currentplayer.o[3] = currentplayer.y+2;
                    }
                    break;
                case '<':
                    if(currentplayer.x == 1){
                        ;
                    }
                    else if(currentplayer.x == 2){
                        currentplayer.o[0] = currentplayer.x-1;
                        currentplayer.o[1] = currentplayer.y;
                    }
                    else{
                        currentplayer.o[0] = currentplayer.x-1;
                        currentplayer.o[1] = currentplayer.y;
                        currentplayer.o[2] = currentplayer.x-2;
                        currentplayer.o[3] = currentplayer.y;
                    }
                    break;
                case '>':
                    if(currentplayer.x == currentplayer.boardsize){
                        ;
                    }
                    else if(currentplayer.x == currentplayer.boardsize-1){
                        currentplayer.o[0] = currentplayer.x+1;
                        currentplayer.o[1] = currentplayer.y;
                    }
                    else{
                        currentplayer.o[0] = currentplayer.x+1;
                        currentplayer.o[1] = currentplayer.y;
                        currentplayer.o[2] = currentplayer.x+2;
                        currentplayer.o[3] = currentplayer.y;
                    }
                    break;



            }
            // Here check whether bullet hit or not;
            // death contains the id of other player;
            for (int i=0;i<=num_client;i++){
                    if (allplayers.players[i].exist == 1){
                        if(((allplayers.players[i].x == currentplayer.o[0]) && (allplayers.players[i].y == currentplayer.o[1]))
                           || ((allplayers.players[i].x == currentplayer.o[2]) && (allplayers.players[i].y == currentplayer.o[3]))){
                                 allplayers.death[i]=1;
                                 currentplayer.score++;
                           }
                    }
            }




        break;
        default :
          clearfireo(&currentplayer);
    }


    }



}
int main(int argc, char * argv[])
{
  int   sock, snew;
  socklen_t fromlength;
    struct  sockaddr_in master, from;


    int i = 0;
    for(i=0;i<30;i++){
        allplayers.death[i] = -1;
    }
    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror ("Server: cannot open master socket");
        exit (1);
    }

    master.sin_family = AF_INET;
    master.sin_addr.s_addr = inet_addr("127.0.0.1");
    master.sin_port = htons (atoi(argv[3]));
    int seed = atoi(argv[4]);
    srand(seed);
    if (bind (sock, (struct sockaddr*) &master, sizeof (master))) {
        perror ("Server: cannot bind master socket");
        exit (1);
    }

    puts("bind done");

    //Listen
   listen (sock, 5);
    signal(SIGTERM,signalHandler);
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while(1)
    {
        if (( snew = accept (sock, (struct sockaddr*) &from, &fromlength)) < 0)
        {
            perror("accept failed\n");
            exit(1);
        }
        puts("Connection accepted");
        //Reply to the client


        // init the player information
        struct playerPosition currentplayer;
        pthread_t sniffer_thread;
        currentplayer.id = num_client;
        currentplayer.exist = 1;
        int rand_value1 = ceil((float)rand()/RAND_MAX * 16);
    int rand_value2 = ceil((float)rand()/RAND_MAX * 16);
        currentplayer.x = rand_value1;
        currentplayer.y = rand_value2;

        strcpy(currentplayer.direction,"^");
        currentplayer.move = '?';
        currentplayer.boardsize = atoi(argv[1]);
        currentplayer.updatePeriod = atof(argv[2]);
        currentplayer.MY_PORT = atoi(argv[3]);

        currentplayer.new_sock = snew;
        currentplayer.score = 0;


        if( pthread_create( &sniffer_thread , NULL ,  connection_handler ,  (void*)&currentplayer) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        num_client++;

    }



    return 0;
}
