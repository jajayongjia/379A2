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
    int new_sock;
    int id;
    int x;
    int y;
    char direction[2];
    char move;
    int fire; // if fire == 1 -> draw o on screen
    int o[4]; // contains x1,y1,x2,y2
    int death; // 1 if this player is hit
    int score; // 0 in default
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
    char move;
    struct timeb t_start,t_current,readtime;
	while(1){

        refresh();


        struct playerPosition currentplayer;
        recv(s,&allplayers,sizeof(struct allplayer),0);

        refresh();
	    if (a==0){
            ID = allplayers.currentIndex;
            win = newwin(allplayers.players[0].boardsize+2,allplayers.players[0].boardsize+2,START_Y,START_X);
            refresh();
             updatep =(int) (allplayers.players[0].updatePeriod * 1000.0);
//             updatep = 1000-50;

         }
        a=1;
		drawScreen(win,allplayers.players,ID);

        refresh();



         char moves[5],i=0;
         for (i=0;i<5;i++){
            moves[i] = changePosition(win,updatep/5);
         }

         for (i = 4;i>=0;i--){
            if(moves[i]!=-1){
                move = moves[i];
                break;
            }
            move = '?';
         }
        printw("%d\n", move);
refresh();

		send(s,&move,sizeof(char),0);
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
	mvwprintw(win,0,player[0].boardsize+1,"+");
	mvwprintw(win,player[0].boardsize+1,0,"+");
	mvwprintw(win,player[0].boardsize+1,player[0].boardsize+1,"+");
	for(int i=0;i<30;i++){
        if (player[i].exist == 1){
//        strcat(player[i].direction,"\0");

        // bool your char!!!
         // if '>'
         // mvwprintw ->
        /***************************************/
            mvwprintw(win,player[i].y,player[i].x,player[i].direction);

            if(player[i].fire ==1 ){
                for(int j = 0 ; j < 3;){
                    if ((player[i].o[j] !=-1)&&(player[i].o[j+1] !=-1)){
                        mvwprintw(win,player[i].o[j+1],player[i].o[j],"o");

                        }
                        j=j+2;
            }

        }
	}
    }	//mvwprintw(win,player[current].y,player[current].x,player[current].direction);
	wrefresh(win);

}

char changePosition(WINDOW* win,int period){
	keypad(win,true);
    wtimeout(win,period);
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
		case ' ':
            return 'f';

	}



}


