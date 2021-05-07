#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60
#define FPS2 30
#define WIDTH  1280
#define HEIGHT 960
#define MWIDTH 40
#define MHEIGHT 30
#define IP_MAX_SIZE 100
#define MAX_PLAYERS 8
#define MIN_PLAYERS 2
#define MAX_LOGIN_SIZE 15
#define LOBBY_WAIT_TIME 30
#define DURATION 300
#define NORMAL_SPEED 4
#define TAG_SPEED 6
#define FREEZE_SPEED 1
#define FREEZE_TIME 5

double startingTime;
double startingCountdownTime;
ALLEGRO_DISPLAY *main_window;
ALLEGRO_EVENT_QUEUE *eventsQueue;
ALLEGRO_TIMER *timer;
ALLEGRO_TIMER *timer1;

//========================
//FONT AND BITMAP POINTERS
ALLEGRO_FONT *ubuntu;
ALLEGRO_FONT *start;
ALLEGRO_FONT *largeStart;
ALLEGRO_FONT *vegan;
ALLEGRO_FONT *ostrich;
ALLEGRO_FONT *digitalt;
ALLEGRO_FONT *comfortaa_bold;
ALLEGRO_FONT *comfortaa_regular;
ALLEGRO_FONT *comfortaa_light;
ALLEGRO_FONT *comfortaa_bold_small;

ALLEGRO_BITMAP *objects;
ALLEGRO_BITMAP *objects2;
ALLEGRO_BITMAP *menuScreen;
ALLEGRO_BITMAP *characterSheet1;
ALLEGRO_BITMAP *tag;
ALLEGRO_BITMAP *box;
//========================
//MAP POINTER
//char **mapMatrix;
char mapMatrix_0[MHEIGHT][MWIDTH];
char mapMatrix_1[MHEIGHT][MWIDTH];
char mapMatrix_2[MHEIGHT][MWIDTH];
//========================

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

//EXAMPLE STRUCT
typedef struct DADOS
{
    char mensagem[100];
    int valor;
    int connectedPlayers;
}DADOS;

typedef struct PLAYERS
{
	  char IP[30];
   	char login[MAX_LOGIN_SIZE]; 
   	float pos_x;
   	float pos_y;
    int direction;
    int step;
   	ALLEGRO_BITMAP *character;
   	int initBitmapPos_x;
   	int initBitmapPos_y; 
    int id;
   	bool tag;
    int tagTime;
    char taunt1[30], taunt2[30], taunt3[30];
    bool ta1, ta2, ta3;
} PLAYERS;

typedef struct ID
{
    int id;
} ID;

typedef struct PLAYER_DATA
{
    PLAYERS all[MAX_PLAYERS];
    bool redraw;
    double timeLeft;
} PLAYER_DATA;

typedef struct KEYS
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool t1;
    bool t2;
    bool t3;
    int id;
} KEYS;

typedef struct CHECK
{
  bool valid;
  bool slow;
} CHECK;

typedef struct LIST_ELEMENT
{
  char login[MAX_LOGIN_SIZE];
  int tagTime;
} LIST_ELEMENT;

typedef struct LIST{
  LIST_ELEMENT playerList[MAX_PLAYERS];
  double timeLeft;
} LIST;

//MAIN ALLEGRO FUNCTIONS
bool coreInit();
bool windowInit(int W, int H, char title[]);
bool inputInit();
void allegroEnd();

//FPS CONTROL FUNCTIONS
void startTimer();
double getTimer();
void FPSLimit();

//RESOURCE LOADING FUNCTIONS
bool loadGraphics();
bool fontInit();
bool loadMap();

//INPUT READING FUNCTION
void readInput(ALLEGRO_EVENT event, char str[], int limit);


#endif
