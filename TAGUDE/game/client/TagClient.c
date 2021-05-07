#include "ACore.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200

#define LARGURA 1280
#define ALTURA 960
#define MAX_LOG_SIZE 26

void printHello();
void assertConnection(char IP[], char login[]);
void runChat();

void printConnectScreen(char str[]);
void printChatLog( char chatLog[][MSG_MAX_SIZE]);
void printLobbyText(char str[], int connectedPlayers);
void printLoginScreen(char str[]);
void printSelectionScreen(char str[]);
void printGameScreen(char map[MHEIGHT][MWIDTH]);
void printScreenOverlay(char map[MHEIGHT][MWIDTH], int time);
void printPlayers(PLAYER_DATA players, int connectedPlayers);
void printTime(int timeLeft);
void printOverTime(int timeLeft);
void printBoard(PLAYER_DATA players, int connectedPlayers);
void printfGameOverScreen(LIST list, int connectedPlayers);
void printTaunt(PLAYER_DATA players, int connectedPlayers);

enum conn_ret_t tryConnect(char IP[]) 
{
  char server_ip[30];
  printf("Please enter the server IP: ");
  return connectToServer(IP);
}

int main() 
{
    //A primeira coisa a ser feita é inicializar os módulos centrais. Caso algum deles falhe, o programa já para por aí.
    if (!coreInit())
        return -1;
    //Agora vamos criar a nossa janela. Largura e Altura em pixels, o título é uma string.
    if (!windowInit(LARGURA, ALTURA, "TAGUDE"))
        return -1;
    //Agora inicializamos nosso teclado e mouse, para que a janela responda às entradas deles
    if (!inputInit())
        return -1;
    //Agora inicializamos nossas fontes
    if(!fontInit())
      return -1;
    //E por fim todas as imagens que vamos utilizar no programa.
    if(!loadGraphics())
        return -1;
 	  // Load map matrix
    if(!loadMap())
        return -1;

    int i, j;
    int previous;

    //para sair do jogo
    bool connectScreen = true;
    bool loginScreen = true;
    bool choosePlayer = true;
    bool lobby = true;
    bool game = true;
    bool gameOver = true;
 
    //Criamos uma nova estrutura que será enviada e recebida do servidor.
    DADOS pacote;
    PLAYERS MyPlayer; 
    PLAYER_DATA players;
    ID idNumber;
    KEYS movement;
    LIST list;

    //para guardar o log do chat
    char chatLog[MAX_LOG_SIZE][MSG_MAX_SIZE] = {{0}};
    //mensagem pra ser mandada no lobby
    char lobbyMessage[110]={0}; 
    //para guardar o login
    char loginMsg[BUFFER_SIZE]={0};
    //connectToServer(ServerIP);
    char selectMsg[2]={0};
    //usados para calcular tempo
    double time; 
    double timeLeft;
    //teclas de movimento
    bool key[4] = {false, false, false, false};
    //bool receiveId = true;
    bool countdown = false;

    while(1)
    {
      while(connectScreen)
      {
          startTimer();

          while(!al_is_event_queue_empty(eventsQueue))
          {
              ALLEGRO_EVENT connectEvent;
              al_wait_for_event(eventsQueue, &connectEvent);

              readInput(connectEvent, MyPlayer.IP, IP_MAX_SIZE);

              if (connectEvent.type == ALLEGRO_EVENT_KEY_DOWN)
              {
                  switch(connectEvent.keyboard.keycode)
                  {
                      case ALLEGRO_KEY_ENTER:
                          connectScreen = false;
                          break;
                  }
              }

              if(connectEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                  connectScreen = false;
                  loginScreen = false;
                  lobby = false;
                  choosePlayer = false;
                  game = false;
                  gameOver = false;
              }
          }

          printConnectScreen(MyPlayer.IP);
          al_flip_display();
          al_clear_to_color(al_map_rgb(0, 0, 0));
          FPSLimit();
      }

      while(loginScreen)
      {
          startTimer();


          while(!al_is_event_queue_empty(eventsQueue))
          {
              ALLEGRO_EVENT loginEvent;
              al_wait_for_event(eventsQueue, &loginEvent);

              readInput(loginEvent, MyPlayer.login, LOGIN_MAX_SIZE);

              if (loginEvent.type == ALLEGRO_EVENT_KEY_DOWN)
              {
                  switch(loginEvent.keyboard.keycode)
                  {
                      case ALLEGRO_KEY_ENTER:
                          loginScreen = false;
                          break;
                  }
              }

              if(loginEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                  loginScreen = false;
                  lobby = false;
                  choosePlayer = false;
                  game = false;
                  gameOver = false;
              }
          }


          printLoginScreen(MyPlayer.login);
          al_flip_display();
          al_clear_to_color(al_map_rgb(0, 0, 0));
          FPSLimit();
      }

      while(choosePlayer)
      {
          startTimer();
          
          while(!al_is_event_queue_empty(eventsQueue))
          {
              ALLEGRO_EVENT selectEvent;
              al_wait_for_event(eventsQueue, &selectEvent);

              readInput(selectEvent, selectMsg, 0);

              if (selectEvent.type == ALLEGRO_EVENT_KEY_DOWN)
              {
                  switch(selectEvent.keyboard.keycode)
                  {
                      case ALLEGRO_KEY_ENTER:
                          if(selectMsg[0] >= 49 && selectMsg[0]<=56) choosePlayer = false;
                          break;
                  }
              }

              if(selectEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                  choosePlayer = false;
                  lobby = false;
                  choosePlayer = false;
                  game = false;
                  gameOver = false;
              }
          }

          printSelectionScreen(selectMsg);
          al_flip_display();
          al_clear_to_color(al_map_rgb(0, 0, 0));
          FPSLimit();
      }

      //Realiza conexão com o server
      assertConnection(MyPlayer.IP, MyPlayer.login);
      sendMsgToServer(selectMsg, 4);

      while(lobby)
      {
          startTimer();
          int players = pacote.connectedPlayers;
          int rec = recvMsgFromServer(&pacote, DONT_WAIT);
          if(pacote.connectedPlayers != players){
            countdown = false;
          }

          //recebe as mensagens do servidor (jogador se conectou, avisos, etc)
          if(rec == sizeof(DADOS)){ 

                  //printf("RECEIVED: %s\n", pacote.mensagem);

                  for(i = 0; i < MAX_LOG_SIZE - 1; ++i)
                      strcpy(chatLog[i], chatLog[i+1]);
                  
                  strcpy(chatLog[MAX_LOG_SIZE - 1], pacote.mensagem);
                  
                  pacote.mensagem[0]='\0';
          }

          while(!al_is_event_queue_empty(eventsQueue))
          {
              ALLEGRO_EVENT lobbyEvent;
              al_wait_for_event(eventsQueue, &lobbyEvent);

              readInput(lobbyEvent, lobbyMessage, MSG_MAX_SIZE);



              if (lobbyEvent.type == ALLEGRO_EVENT_KEY_DOWN)
              {
                  switch(lobbyEvent.keyboard.keycode)
                  {
                      case ALLEGRO_KEY_ENTER:
                          strcpy(pacote.mensagem, lobbyMessage);
                          sendMsgToServer(&pacote, sizeof(DADOS));
                          lobbyMessage[0]='\0';
                          break;
                  }
              }

              if(lobbyEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                  game = false;
                  gameOver = false;
              }
          }
          //=========================================================//
          // CALCULA O TEMPO PARA COMECAR A PARTIDA
          if(pacote.connectedPlayers>=MIN_PLAYERS && !countdown){
            startingCountdownTime = al_get_time();
            countdown = true;
          }
          if(countdown){
            time = al_get_time() - startingCountdownTime;
            timeLeft = LOBBY_WAIT_TIME - time;
            if((int) timeLeft<=0){
              lobby = false;
           }
          }
          //=========================================================//
          printLobbyText(lobbyMessage, pacote.connectedPlayers);
          printChatLog(chatLog);
          if(pacote.connectedPlayers >= MIN_PLAYERS){
            printTime((int)timeLeft);
          }
          al_flip_display();
          al_clear_to_color(al_map_rgb(0, 0, 0));
          FPSLimit();
      }

      al_start_timer(timer1);
      bool start=true;
      bool draw_board=false;
      timeLeft = DURATION;

     	while(game)
      {
          previous = (int) timeLeft;
          int playersStruct = recvMsgFromServer(&players, DONT_WAIT);
          if (playersStruct == sizeof(PLAYER_DATA)) timeLeft = players.timeLeft;
          ALLEGRO_EVENT ev;
          al_wait_for_event(eventsQueue, &ev);
          if(ev.type != ALLEGRO_EVENT_TIMER){
            if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
              game = false;
              break;
            }
            else{
              if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(ev.keyboard.keycode) {
                  case ALLEGRO_KEY_UP:
                      movement.up = true;
                      break;

                  case ALLEGRO_KEY_DOWN:
                      movement.down = true;
                      break;

                  case ALLEGRO_KEY_LEFT: 
                      movement.left = true;
                      break;

                  case ALLEGRO_KEY_RIGHT:
                      movement.right = true;
                      break;

                  case ALLEGRO_KEY_B:
                      draw_board = true;
                      break;

                  case ALLEGRO_KEY_1:
                      movement.t1 = true;
                      break;
                
                  case ALLEGRO_KEY_2:
                      movement.t2 = true;
                      break;

                  case ALLEGRO_KEY_3:
                      movement.t3 = true;
                      break;

                  case ALLEGRO_KEY_ESCAPE:
                      game = false;
                      break;
                }
              }
              else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch(ev.keyboard.keycode) {
                  case ALLEGRO_KEY_UP:
                      movement.up = false;
                      break;

                  case ALLEGRO_KEY_DOWN:
                      movement.down = false;
                      break;

                  case ALLEGRO_KEY_LEFT: 
                      movement.left = false;
                      break;

                  case ALLEGRO_KEY_RIGHT:
                      movement.right = false;
                      break;

                  case ALLEGRO_KEY_B:
                      draw_board = false;
                      break;

                  case ALLEGRO_KEY_1:
                      movement.t1 = false;
                      break;

                  case ALLEGRO_KEY_2:
                      movement.t2 = false;
                      break;

                  case ALLEGRO_KEY_3:
                      movement.t3 = false;
                      break;

                  case ALLEGRO_KEY_ESCAPE:
                      game = false;
                      break;
                }
              }
              if(movement.up || movement.down || movement.left || movement.right || movement.t1 || movement.t2 || movement.t3){
                //if (movement.up) printf("up\n");
                //if (movement.down) printf("down\n");
                //if (movement.left) printf("left\n");
                //if (movement.right) printf("right\n");
                sendMsgToServer(&movement, sizeof(KEYS));
              }
            }
          }
          //=========================================================//
          // CALCULA O TEMPO PARA TERMINAR A PARTIDA
          if((int) timeLeft<=0){
            countdown = false;
            game = false;
            gameOver = true;
          }
          //=========================================================//
          if(players.redraw || start || previous != (int) players.timeLeft){
            printGameScreen(mapMatrix_0);
            printPlayers(players, pacote.connectedPlayers);
            printScreenOverlay(mapMatrix_0, timeLeft);
            printTaunt(players, pacote.connectedPlayers);
            if(draw_board) printBoard(players, pacote.connectedPlayers);
            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            start = false;
          }
          FPSLimit();
      }

      timeLeft = LOBBY_WAIT_TIME;

      while(gameOver)
      {
        previous = (int) timeLeft;
        int listOfPlayers = recvMsgFromServer(&list, DONT_WAIT);

        if(listOfPlayers == sizeof(LIST)) timeLeft = list.timeLeft;
    
        if((int) timeLeft<=0){
          gameOver = false;
          connectScreen = false;
          loginScreen = false;
          choosePlayer = false;
          lobby = true;
          game = true;
        }
        if(previous != (int) timeLeft){
          al_clear_to_color(al_map_rgb(255, 255, 50));
          printfGameOverScreen(list, pacote.connectedPlayers);
          printOverTime(timeLeft);
          al_flip_display();
        }
        FPSLimit();
      }
    }

  allegroEnd();
  return 0;
}

//========================================================FUNCTIONS========================================================
void printHello() 
{
  puts("Wellcome to TAGUDE.");
  puts("Get ready for cunegudz");
}

void assertConnection(char IP[], char login[]) 
{
  printHello();
  enum conn_ret_t ans = tryConnect(IP);

  while (ans != SERVER_UP) 
  {
    if (ans == SERVER_DOWN) {
      puts("Server is down!");
    } else if (ans == SERVER_FULL) {
      puts("Server is full!");
    } else if (ans == SERVER_CLOSED) {
      puts("Server is closed for new connections!");
    } else {
      puts("Server didn't respond to connection!");
    }
    printf("Want to try again? [Y/n] ");
    int res;
    while (res = tolower(getchar()), res != 'n' && res != 'y' && res != '\n'){
      puts("anh???");
    }
    if (res == 'n') {
      exit(EXIT_SUCCESS);
    }
    ans = tryConnect(IP);
  }
  //char login[LOGIN_MAX_SIZE + 4];
  //printf("Please enter your login (limit = %d): ", LOGIN_MAX_SIZE);
  //scanf(" %[^\n]", login);
  //getchar();
  int len = (int)strlen(login);
  sendMsgToServer(login, len + 1);
}

void runChat() 
{
  char str_buffer[BUFFER_SIZE], type_buffer[MSG_MAX_SIZE] = {0};
  char msg_history[HIST_MAX_SIZE][MSG_MAX_SIZE] = {{0}};
  int type_pointer = 0;
  
  while (1) {
    // LER UMA TECLA DIGITADA
    char ch = getch();
    if (ch == '\n') {
      type_buffer[type_pointer++] = '\0';
      int ret = sendMsgToServer((void *)type_buffer, type_pointer);
      if (ret == SERVER_DISCONNECTED) {
        return;
      }
      type_pointer = 0;
      type_buffer[type_pointer] = '\0';
    } else if (ch == 127 || ch == 8) {
      if (type_pointer > 0) {
        --type_pointer;
        type_buffer[type_pointer] = '\0';
      }
    } else if (ch != NO_KEY_PRESSED && type_pointer + 1 < MSG_MAX_SIZE) {
      type_buffer[type_pointer++] = ch;
      type_buffer[type_pointer] = '\0';
      
    }

    // LER UMA MENSAGEM DO SERVIDOR
    int ret = recvMsgFromServer(str_buffer, DONT_WAIT);
    if (ret == SERVER_DISCONNECTED) {
      return;
    } else if (ret != NO_MESSAGE) {
      int i;
      for (i = 0; i < HIST_MAX_SIZE - 1; ++i) {
        strcpy(msg_history[i], msg_history[i + 1]);
      }
      strcpy(msg_history[HIST_MAX_SIZE - 1], str_buffer);
    }

    // PRINTAR NOVO ESTADO DO CHAT
    system("clear");
    int i;
    for (i = 0; i < HIST_MAX_SIZE; ++i) {
      printf("%s\n", msg_history[i]);
    }
    printf("\nYour message: %s\n", type_buffer);
  }
}

void printConnectScreen(char str[])
{
    //al_draw_bitmap(menuScreen,0,0,0);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_tinted_bitmap_region(menuScreen,al_map_rgba_f(1, 1, 1, 0.2), 320, 80, 1280, 960, 0, 0, 0);
    //al_draw_bitmap_region(menuScreen, 640, 280, 640, 480, 0, 0, 0);

    al_draw_text(vegan, al_map_rgb(0, 0, 0), WIDTH / 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o IP do server:");
    //al_draw_text(start, al_map_rgb(0, 0, 0), WIDTH / 2, 40, ALLEGRO_ALIGN_CENTRE, mapMatrix[0]);

    if (strlen(str) > 0)
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "0.0.0.0");
    }
}

void printSelectionScreen(char str[])
{
    int i;
    //al_draw_bitmap(menuScreen,0,0,0);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_tinted_bitmap_region(menuScreen,al_map_rgba_f(1, 1, 1, 0.2), 320, 80, 1280, 960, 0, 0, 0);

    al_draw_text(vegan, al_map_rgb(0, 0, 0), WIDTH / 2, 30, ALLEGRO_ALIGN_CENTRE, "Escolha o seu personagem:");
    
    for(i=0;i<4;i++){
        al_draw_scaled_bitmap(characterSheet1, (0+(3*i))*32, 0*32, 32, 32, (WIDTH/4)+51+(i*(51+96)), (HEIGHT/4)+100, 96, 96, 0);
        char n[2] = {i+1+48,'\0'};
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), (WIDTH/4)+51+(i*(51+96))+(96/2), (HEIGHT/4)+100+96+5, ALLEGRO_ALIGN_CENTRE, n);
    }
    for(i=0;i<4;i++){
        al_draw_scaled_bitmap(characterSheet1, (0+(3*i))*32, 4*32, 32, 32, (WIDTH/4)+51+(i*(51+96)), (HEIGHT/4)+236, 96, 96, 0);
        char n[2] = {i+1+4+48,'\0'};
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), (WIDTH/4)+51+(i*(51+96))+(96/2), (HEIGHT/4)+236+96+5, ALLEGRO_ALIGN_CENTRE, n);
    }
    if (strlen(str) > 0)
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT - 30 - al_get_font_ascent(start), ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2, HEIGHT - 30 - al_get_font_ascent(start), ALLEGRO_ALIGN_CENTRE, "0");
    }
}


void printChatLog( char chatLog[][MSG_MAX_SIZE])
{
    int i;
    int initialY = 120;
    int spacing = al_get_font_ascent(comfortaa_regular)+5;
    
    for(i = 0; i < MAX_LOG_SIZE; ++i)
    {
        al_draw_text(comfortaa_regular, al_map_rgb(0, 0, 0), 40, initialY + (i*spacing), ALLEGRO_ALIGN_LEFT, chatLog[i]);
    }
}

void printLobbyText(char str[], int connectedPlayers)
{
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_tinted_bitmap_region(menuScreen,al_map_rgba_f(1, 1, 1, 0.2), 320, 80, 1280, 960, 0, 0, 0);
    //al_draw_bitmap_region(menuScreen, 640, 280, 640, 480, 0, 0, 0);
    char buffer[50];
    sprintf(buffer, "%d/%d Jogadores conectados", connectedPlayers, MAX_PLAYERS);
    al_draw_text(vegan, al_map_rgb(0, 0, 0), 20, 30, ALLEGRO_ALIGN_LEFT, "Sala De Espera");
    al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), 1260, 30, ALLEGRO_ALIGN_RIGHT, buffer);
    if (strlen(str) > 0)
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), 20,
                     (HEIGHT - al_get_font_ascent(start)) - 20,
                     ALLEGRO_ALIGN_LEFT, str);
    }
    else
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), 20,
                     (HEIGHT - al_get_font_ascent(start)) - 20,
                     ALLEGRO_ALIGN_LEFT, "Digite para conversar...");
    }
}

void printLoginScreen(char str[])
{
    //al_draw_bitmap(menuScreen,0,0,0);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_tinted_bitmap_region(menuScreen,al_map_rgba_f(1, 1, 1, 0.2), 320, 80, 1280, 960, 0, 0, 0);
    //al_draw_bitmap_region(menuScreen, 640, 280, 640, 480, 0, 0, 0);

    al_draw_text(vegan, al_map_rgb(0, 0, 0), WIDTH / 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o Login desejado: ");

    if (strlen(str) > 0)
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH / 2,
                     (HEIGHT - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "<login>");
    }
}

void printGameScreen(char map[MHEIGHT][MWIDTH])
{
   int i, j;
   al_clear_to_color(al_map_rgb(255, 255, 255));  	
   for(i=0;i<MHEIGHT;i++){
      for(j=0;j<MWIDTH;j++){
         if(map[i][j]== 't') al_draw_bitmap_region(objects, 4*32, 1*32, 32, 32, j*32, i*32, 0);
         else if(map[i][j]== 's') al_draw_bitmap_region(objects, 1*32, 2*32, 32, 32, j*32, i*32, 0);
         else if(map[i][j]== 'r') al_draw_bitmap_region(objects, 5*32, 1*32, 32, 32, j*32, i*32, 0);
         else if(map[i][j]== 'k') al_draw_bitmap_region(objects, 9*32, 1*32, 32, 32, j*32, i*32, 0);
         else al_draw_bitmap_region(objects, 0*32, 2*32, 32, 32, j*32, i*32, 0);
      }
   }
}

void printPlayers(PLAYER_DATA players, int connectedPlayers)
{
  int i;
  for(i=0;i<connectedPlayers;i++){
    al_draw_text(comfortaa_bold_small, al_map_rgb(255,255,50), players.all[i].pos_x+16, players.all[i].pos_y-24, ALLEGRO_ALIGN_CENTRE, players.all[i].login);
    if (players.all[i].tag == true) al_draw_bitmap_region(tag, 6*32, 4*32, 32, 32, players.all[i].pos_x, players.all[i].pos_y-40, 0);
    if(players.all[i].direction==KEY_UP) al_draw_bitmap_region(characterSheet1, (players.all[i].initBitmapPos_x + players.all[i].step)*32, (players.all[i].initBitmapPos_y+3)*32, 32, 32, players.all[i].pos_x, players.all[i].pos_y, 0);
    else if(players.all[i].direction==KEY_DOWN) al_draw_bitmap_region(characterSheet1, (players.all[i].initBitmapPos_x + players.all[i].step)*32, players.all[i].initBitmapPos_y*32, 32, 32, players.all[i].pos_x, players.all[i].pos_y, 0);
    else if(players.all[i].direction==KEY_LEFT) al_draw_bitmap_region(characterSheet1, (players.all[i].initBitmapPos_x + players.all[i].step)*32, (players.all[i].initBitmapPos_y+1)*32, 32, 32, players.all[i].pos_x, players.all[i].pos_y, 0);
    else if(players.all[i].direction==KEY_RIGHT) al_draw_bitmap_region(characterSheet1, (players.all[i].initBitmapPos_x + players.all[i].step)*32, (players.all[i].initBitmapPos_y+2)*32, 32, 32, players.all[i].pos_x, players.all[i].pos_y, 0);
  }
}

void printScreenOverlay(char map[MHEIGHT][MWIDTH], int time)
{
    int i=0, j=0;
    for(i=0;i<MHEIGHT;i++){
      for(j=0;j<MWIDTH;j++){
         if(map[i][j]== 'g') al_draw_bitmap_region(objects2, 0*64, 7*64, 64, 64, j*32, i*32, 0);
         else if(map[i][j]== 'f') al_draw_bitmap_region(objects2, 4*64, 0*64, 64, 64, j*32, i*32, 0);
      }
    }
    char str_buffer[20];
    sprintf(str_buffer, "-= %d =-", time);
    al_draw_text(digitalt, al_map_rgb(0, 0, 75), WIDTH/2, 30, ALLEGRO_ALIGN_CENTRE, str_buffer);
}

void printTime(int time)
{
  char str_buffer[30];
  sprintf(str_buffer, "INICIANDO EM %d SEGUNDOS", time);
  al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH-30, 60, ALLEGRO_ALIGN_RIGHT, str_buffer);
}

void printOverTime(int time)
{
  char str_buffer[30];
  sprintf(str_buffer, "RETORNANDO EM %d SEGUNDOS", time);
  al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH-30, 60, ALLEGRO_ALIGN_RIGHT, str_buffer);
}

void printBoard(PLAYER_DATA players, int connectedPlayers)
{
  int i;
  for(i=0;i<connectedPlayers;i++){
    char str_buffer[40];
    sprintf(str_buffer, "%s - %d SEGUNDOS", players.all[i].login, players.all[i].tagTime);
    al_draw_text(comfortaa_regular, al_map_rgb(255, 255, 50), WIDTH-30, 30+(i*30), ALLEGRO_ALIGN_RIGHT, str_buffer);
  }
}

void printfGameOverScreen(LIST list, int connectedPlayers)
{
  int i;
  al_clear_to_color(al_map_rgb(255, 255, 255));
  al_draw_tinted_bitmap_region(menuScreen,al_map_rgba_f(255, 255, 50, 0.2), 320, 80, 1280, 960, 0, 0, 0);
  al_draw_text(vegan, al_map_rgb(0, 0, 0), WIDTH/2, 30, ALLEGRO_ALIGN_CENTER, "Placar Final");
  for(i=0;i<connectedPlayers;i++){
    char str_buffer[60];
    sprintf(str_buffer, "%d - %s - %d SEGUNDOS", i+1, list.playerList[i].login, list.playerList[i].tagTime);
    al_draw_text(comfortaa_bold, al_map_rgb(0, 0, 0), WIDTH/2, 180+(i*30), ALLEGRO_ALIGN_CENTER, str_buffer);
  }
}

void printTaunt(PLAYER_DATA players, int connectedPlayers)
{
  int i;
  for(i=0;i<connectedPlayers;i++){
    if(players.all[i].ta1) {
      al_draw_text(comfortaa_bold_small, al_map_rgb(57, 0, 117), players.all[i].pos_x + 16, players.all[i].pos_y + 40, ALLEGRO_ALIGN_CENTER, players.all[i].taunt1);
    }
    else if(players.all[i].ta2){
      al_draw_text(comfortaa_bold_small, al_map_rgb(57, 0, 117), players.all[i].pos_x + 16, players.all[i].pos_y + 40, ALLEGRO_ALIGN_CENTER, players.all[i].taunt2);
    }
    else if(players.all[i].ta3){
      al_draw_text(comfortaa_bold_small, al_map_rgb(57, 0, 117), players.all[i].pos_x + 16, players.all[i].pos_y + 40, ALLEGRO_ALIGN_CENTER, players.all[i].taunt3);
    }
  }
}