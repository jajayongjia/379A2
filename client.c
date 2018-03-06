/*
 * Copyright 2017 Yongjia Huang, Yuhan Ye,CMPUT379, University of Alberta, All Rights Reserved.
 * You may use distribut, or modify this code under terms and conditions of the ode of Student Behavior at University of Alberta
 * You may find a copy of the license in this project. Otherwise please contact jajayongjia@gmail.com
 */


#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <time.h>
#include <sys/timeb.h>
#define START_Y 10
#define START_X 10
/* ---------------------------------------------------------------------
   This is a sample client program for the number server. The client and
   the server need to run on the same machine.
   --------------------------------------------------------------------- */

long long current_time() {
    struct timeval t;
   // gettimeofday(&t, NULL);
    long long milliseconds = t.tv_sec*1000LL + t.tv_usec/1000;

    return milliseconds;
}
struct playerPosition{
    int exist;
    int boardsize;
    double updatePeriod;
    int MY_PORT;
    int seed;
    int *new_sock;
    int id;
    int x;
    int y;
    char direction[2];
    char move;
};
struct allplayer{
    struct playerPosition players[30];
    int currentIndex;
};
char changePosition(WINDOW* win,int period);

void drawScreen(WINDOW* win,struct playerPosition player[30],int current);


int main(int argc, char * argv[])
{

	int	s, number;
    struct allplayer allplayers;
	struct	sockaddr_in	server;
	int ID,n;
	int a=0;
	int updatep;
	struct	hostent		*host;

	host = gethostbyname ("localhost");



	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}


	s = socket (AF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		perror ("Client: cannot open socket");
		exit (1);
	}

	bzero (&server, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons (atoi(argv[2]));

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}
	initscr();
	cbreak();
    WINDOW *win;
    struct timeb t_start,t_current,readtime;
	while(recv(s,&allplayers,sizeof(struct allplayer),0)){

		//printf("%d\n",allplayers->players[0].boardsize);



        if (a==0){
            win = newwin(allplayers.players[0].boardsize,allplayers.players[0].boardsize,START_Y,START_X);
            refresh();
             updatep =(int) (allplayers.players[0].updatePeriod * 1000.0);
             updatep = 1000;

         }
        a=1;
        ID = allplayers.currentIndex;


		drawScreen(win,allplayers.players,ID);
        int moves[10],i=0;


        ftime(&t_start);
        do{
        ftime(&readtime);
        moves[i++] = changePosition(win,updatep - ((int) (1000.0 * (readtime.time - t_start.time)
        + (readtime.millitm - t_start.millitm))));

        ftime(&t_current);
        } while ((int) (1000.0 * (t_current.time - t_start.time)
        + (t_current.millitm - t_start.millitm)) < updatep);

        for (i = 0; i<10; i++){
            if(moves[i] == -1){
                if (i == 0){
                    allplayers.players[ID].move = '?';
                }
                else{
                    allplayers.players[ID].move = (char) moves[i-1];
                }
            }
        }

        printw("%d\n", allplayers.players[ID].move);
        refresh();

		send(s,&allplayers,sizeof(struct allplayer),0);






	}
	close (s);
}

void drawScreen(WINDOW* win,struct playerPosition player[30],int current){
	wclear(win);
	char lr = '|';
	char tb = '-';
	box(win,(int)lr,(int)tb);
	refresh();
	wrefresh(win);


	mvwprintw(win,0,0,"+");
	mvwprintw(win,0,player[0].boardsize-1,"+");
	mvwprintw(win,player[0].boardsize-1,0,"+");
	mvwprintw(win,player[0].boardsize-1,player[0].boardsize-1,"+");
	for(int i=0;i<30;i++){
        if (player[i].exist == 1){
        strcat(player[i].direction,"\0");
        // bool your char!!!
        /***************************************/
		mvwprintw(win,player[i].y,player[i].x,player[i].direction);
	}
	}
	//mvwprintw(win,player[current].y,player[current].x,player[current].direction);
	wrefresh(win);
}


char changePosition(WINDOW* win,int period){
	keypad(win,true);
//    notimeout(win,true);
    wtimeout(win,period);
//   printw("time left is %d\n",period);
//        refresh();
	int c = wgetch(win);
	switch(c){
		case KEY_UP:
			return '^';

			break;
		case KEY_DOWN:

			return 'v';
			break;
		case KEY_LEFT:

			return '<';
			break;
		case KEY_RIGHT:
			return '>';
			break;
	}
//	printw("1 %d \n",player->move);
//	refresh();




}




