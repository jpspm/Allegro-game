#include "ACore.h"
#include "server.h"
#include <stdio.h>
#include <string.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13

int generateRandom(int min, int max);

CHECK validateUP(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]); 
CHECK validateDOWN(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]);
CHECK validateLEFT(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]);
CHECK validateRIGHT(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]);

LIST generateList(PLAYER_DATA players, int connectedPlayers);

int main() 
{
  if(!loadMap()) return -1;

  PLAYER_DATA players;
  DADOS msg;
  KEYS movements;
  ID idNumber;
  LIST list;

  CHECK check;

  char client_names[MAX_PLAYERS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];
  char character[2];
  serverInit(MAX_PLAYERS);
  puts("Server is running!!");
  //==================================//
  // USADOS PARA CALCULAR TEMPO
  //startTimer(timer); // INICIA UM CONTADOR
  double startingCountdownTime; // VALOR INICIAL DO CONTADOR
  double freeze_startingCountdownTime;
  double timeLeft; // TEMPO ATÉ O FIM DA PARTIDA OU DO LOBBY
  double currentTime; // TEMPO QUE SE PASSOU DESDE QUE O CONTADOR SE INICIOU 
  int lastTime; // VALOR DE TIME LERFT NO LOOP ANTERIOR
  //==================================//
  float speed; // VELOCIDADE EM QUE O PLAYER VAI ANDAR
  int connectedPlayers=0; // QUANTIDADE DE JOGADORES CONECTADOS
  //==================================//
  // SEPARAÇOES DA MAIN
  bool connect = true; // CONECTA JOGADORES E CUIDA DO LOBBY
  bool game = true; // INICIA O JOGO
  bool gameOver = true;
  //==================================//
  bool start = true;
  bool countdown = false;
  bool freeze_countdown = false;
  bool valid=true;
  bool slow = false;
  bool tagTransfered = false;
  bool freeze = false;
  //==================================//
  while (1) 
  { 
    start = true; // usado apenas uma vez para desenher os jogodores no inicio da partida
    //==================================//
    // ESPERA OS JOGADORES SE CONECTAREM
    while(connect)
    {
      int amntOfPlayers = connectedPlayers;
      int id = acceptConnection();
      if (id != NO_CONNECTION) {
        int i;
        int aux=0;
        recvMsgFromClient(players.all[id].login, id, WAIT_FOR_IT);
        strcpy(str_buffer, players.all[id].login);
        strcat(str_buffer, " connected.");
        connectedPlayers++;
        msg.connectedPlayers = connectedPlayers;
        strcpy(msg.mensagem, str_buffer);
        //msg.id = id;
        broadcast(&msg, sizeof(DADOS));
        recvMsgFromClient(character, id, WAIT_FOR_IT);
        //players.all[id].id = id;
        //players.all[id].character = characterSheet1;
        for(i=0;i<8;i++){
          if(character[0]==49+i) {
            if(i<4)players.all[id].initBitmapPos_x = 0+(i*3);
            else players.all[id].initBitmapPos_x = 0+((i-4)*3);
            if(i<4) players.all[id].initBitmapPos_y = 0;
            else players.all[id].initBitmapPos_y = 4;
          }
        }

        if(character[0] == '1') {
          strcpy(players.all[id].taunt1, "AFASTEM-SE, PLEBE!!");
          strcpy(players.all[id].taunt2, "A MENINOSA EH MINHA!!");
          strcpy(players.all[id].taunt3, "12 É O NOVO 18!!");

        }
        else if(character[0] == '2') {
          strcpy(players.all[id].taunt1, "HMMMMMMMM TOTOSA!!");
          strcpy(players.all[id].taunt2, "GADO D++ !!");
        }
        else if(character[0] == '3') {
          strcpy(players.all[id].taunt1, "EEEEEEE MANO!!");
          strcpy(players.all[id].taunt2, "OOOOOOO VEI!!");
          strcpy(players.all[id].taunt3, "VOU TE DAR UMA CABEÇADA!!");
        }
        else if(character[0] == '4') {
          strcpy(players.all[id].taunt1, "DA QUE EU TE DOU OUTRA!!");
          strcpy(players.all[id].taunt2, "AAAAAAAAAAAAHHHHHH!!");
          strcpy(players.all[id].taunt3, "");
        }
        else if(character[0] == '5') {
          strcpy(players.all[id].taunt1, "TE PEGUEI! MENOSJUM!!");
          strcpy(players.all[id].taunt2, "MORREU MARIA PREA!!");
          strcpy(players.all[id].taunt3, "EI DJ ABAIXA O SOM!!");
        }
        else if(character[0] == '6') {
          strcpy(players.all[id].taunt1, "GLUB GLUB GLAB GLAB!!");
          strcpy(players.all[id].taunt2, "PROJETO?? NEIN NEIN NEIN!!");
        }
         else if(character[0] == '7') {
          strcpy(players.all[id].taunt1, "EH MELHOR PROTEGER A RETAGUARDA!!");
          strcpy(players.all[id].taunt2, "TO CHEGANDO PELO FLANCO!!");
          strcpy(players.all[id].taunt3, "TAG QUE DOI OWO!!");
        }
         else if(character[0] == '8') {
          strcpy(players.all[id].taunt1, "A.C.A.B.R.A.L.I.A!!");
          strcpy(players.all[id].taunt2, "VAMO DAR UNS FLIPZZ!!");
          strcpy(players.all[id].taunt3, "VAI UMA BOLACHA GELADA??");
        }

        printf("%s connected id = %d\n", players.all[id].login, id);
      }
      //==================================//
      // TROCA DE MENSAGENS NO LOBBY
      struct msg_ret_t msg_ret = recvMsg(aux_buffer);
      if (msg_ret.status == MESSAGE_OK) {
        sprintf(str_buffer, "%s-%d: %s", players.all[msg_ret.client_id].login, msg_ret.client_id, aux_buffer);
        strcpy(msg.mensagem, str_buffer);
        msg.connectedPlayers = connectedPlayers;
        broadcast(&msg, sizeof(DADOS));
      } 
      else if (msg_ret.status == DISCONNECT_MSG) {
        connectedPlayers--;
        sprintf(str_buffer, "%s disconnected", players.all[msg_ret.client_id].login);
        printf("%s disconnected, id = %d is free\n", players.all[msg_ret.client_id].login, msg_ret.client_id);
        strcpy(msg.mensagem, str_buffer);
        msg.connectedPlayers = connectedPlayers;
        broadcast(&msg, sizeof(DADOS));
      }
      if (connectedPlayers != amntOfPlayers) countdown = false;
      //=================================//
      // VERIFICA A QUANTIDADE DE JOGADORES CONECTADOS E INFORMA O TEMPO PARA COMEÇAR A PARTIDA
      if(connectedPlayers >= MIN_PLAYERS && !countdown) {
          startingCountdownTime = al_get_time();
          countdown = true;
      }
      if(countdown){
        timeLeft = LOBBY_WAIT_TIME - (al_get_time() - startingCountdownTime);
      }
      //==================================//
      if(countdown && (int) timeLeft<0){
        int i;
        srand(time(NULL));
        int TaggedPlayer = generateRandom(0, connectedPlayers-1);
        for(i=0;i<connectedPlayers;i++){
          players.all[i].direction = KEY_DOWN;
          players.all[i].tag = false;
          players.all[i].tagTime=0;
          players.all[i].ta1=false;
          players.all[i].ta2=false;
          players.all[i].ta3=false;
          players.all[i].step=0;
        }

        //=============================================//
        // POSICOES INICIAIS
        players.all[0].pos_x = 3*32;
        players.all[0].pos_y = 3*32;

        players.all[1].pos_x = 3*32;
        players.all[1].pos_y = 26*32;

        players.all[2].pos_x = 36*32;
        players.all[2].pos_y = 3*32;

        players.all[3].pos_x = 36*32;
        players.all[3].pos_y = 26*32;

        players.all[4].pos_x = 1*32;
        players.all[4].pos_y = 16*32;

        players.all[5].pos_x = 38*32;
        players.all[5].pos_y = 16*32;

        players.all[6].pos_x = 20*32;
        players.all[6].pos_y = 1*32;

        players.all[7].pos_x = 20*32;
        players.all[7].pos_y = 28*32;
        //=============================================//

        players.all[TaggedPlayer].tag = true;
        
        countdown = false;
        connect = false;
        game = true;
      }
      //==================================//
    }

    //==================================//
    // =]    
    while(game)
    {
      int id, i, j, TaggedPlayer;
      struct msg_ret_t movements_struct = recvMsg(&movements);
      //
      for(i=0;i<connectedPlayers;i++){
        if (players.all[i].tag==true) TaggedPlayer = i; 
      }
      //=========================================================//
      // CALCULA O TEMPO PARA TERMINAR A PARTIDA
      lastTime = (int)timeLeft;
      if(!countdown){
        startingCountdownTime = al_get_time();
        countdown = true;
        timeLeft = DURATION;
      }
      if(countdown){
        timeLeft = DURATION - (al_get_time() - startingCountdownTime);
        players.timeLeft = timeLeft;
        if((int) timeLeft<0){
          game = false;
          countdown = false;
          gameOver = true;
        }
      }
      //=========================================================//

      if((int)timeLeft < lastTime) players.all[TaggedPlayer].tagTime++; // ACRESCENTA TEMPO COM A TAG

      if(start) players.redraw = true; 
      if(movements_struct.status==MESSAGE_OK || start || lastTime != (int) timeLeft){
        if(timeLeft>=0) broadcast(&players, sizeof(PLAYER_DATA));
        movements_struct.status = NO_MESSAGE;
        id = movements_struct.client_id;
        if(players.all[id].tag) speed = TAG_SPEED;
        if(players.all[id].tag && freeze) speed = FREEZE_SPEED;
        else speed = NORMAL_SPEED;
        players.redraw = false;
        start = false;
        //==================================//
        // TRANSFERIR A TAG
        for(i=0;i<connectedPlayers;i++){
          if(!freeze 
            && players.all[i].tag == false
            && players.all[i].pos_x>players.all[TaggedPlayer].pos_x-32 
            && players.all[i].pos_x<players.all[TaggedPlayer].pos_x+32 
            && players.all[i].pos_y>players.all[TaggedPlayer].pos_y-32
            && players.all[i].pos_y<players.all[TaggedPlayer].pos_y+32)
          {
            players.all[i].tag = true;
            tagTransfered = true;
          }
        }
        if(tagTransfered){
          freeze = true;
          players.all[TaggedPlayer].tag = false;
          tagTransfered = false;
        }
        if(freeze){
          if(!freeze_countdown){
            freeze_startingCountdownTime = al_get_time();
            freeze_countdown = true;
          }
          if(al_get_time()-freeze_startingCountdownTime > FREEZE_TIME){
            freeze = false;
            freeze_countdown = false;
          }
        }
        //==================================//
        // VALIDAR MOVIMENTOS
        if(movements.t1) {
          players.all[id].ta1 = true;
          players.redraw = true;
        }
        else if(!movements.t1) {
          players.all[id].ta1 = false;
          players.redraw = false;
        }
        if(movements.t2) {
          players.all[id].ta2 = true;
          players.redraw = true;
        }
        else if(!movements.t2) {
          players.all[id].ta2 = false;
          players.redraw = false;
        }
        if(movements.t3) {
          players.all[id].ta3 = true;
          players.redraw = true;
        }
        else if(!movements.t3) {
          players.all[id].ta3 = false;
          players.redraw = false;
        }

        if(movements.up && players.all[id].pos_y>=4){

          check = validateUP(players, id, mapMatrix_0);

          if(check.valid){
            if(check.slow){
              players.all[id].pos_y-=(speed/2);
              check.slow=false;
            }
            else players.all[id].pos_y-=speed;

            if(players.all[id].step < 2) players.all[id].step++;
            else players.all[id].step = 0;

            players.all[id].direction=KEY_UP;
            players.redraw = true;
          }
        }
        if(movements.down && players.all[id].pos_y<=HEIGHT-32){

          check = validateDOWN(players, id, mapMatrix_0);

          if(check.valid){
            if(check.slow){
              players.all[id].pos_y+=(speed/2);
              slow=false;
            }
            else players.all[id].pos_y+=speed;

            if(players.all[id].step < 2) players.all[id].step++;
            else players.all[id].step = 0;

            players.all[id].direction=KEY_DOWN;
            players.redraw = true;
          }
        }
        if(movements.left && players.all[id].pos_x>=4){
          
          check = validateLEFT(players, id, mapMatrix_0);

          if(check.valid){
            if(check.slow){
              players.all[id].pos_x-=(speed/2);
              slow = false;
            }
            else players.all[id].pos_x-=speed;

            if(players.all[id].step < 2) players.all[id].step++;
            else players.all[id].step = 0;

            players.all[id].direction=KEY_LEFT;
            players.redraw = true;
          }
        }
        if(movements.right && players.all[id].pos_x<=WIDTH-32){
          
          check = validateRIGHT(players, id, mapMatrix_0);
          
          if(check.valid){
            if(check.slow){
              players.all[id].pos_x+=(speed/2);
              slow = false;
            }
            else players.all[id].pos_x+=speed;

            if(players.all[id].step < 2) players.all[id].step++;
            else players.all[id].step = 0;

            players.all[id].direction=KEY_RIGHT;
            players.redraw = true;
          }
        }
        //==================================//
      }
      //==================================//
    }

    countdown = false;
    while(gameOver)
    {
      int i;
      if(!countdown){
        list = generateList(players, connectedPlayers);
        startingCountdownTime = al_get_time();
        timeLeft = LOBBY_WAIT_TIME;
        countdown = true;
      }
      if(countdown){
        timeLeft = LOBBY_WAIT_TIME - (al_get_time() - startingCountdownTime);
        list.timeLeft = timeLeft;
        if((int) timeLeft<=0){
          gameOver = false;
          connect = true;
          game = true;
          connectedPlayers = 0;
        }
      }
      printf("\n%lf", timeLeft);
      broadcast(&list, sizeof(LIST));
    }

    serverReset();
  }
}

//==============================================================================================================================================================================//
//================================================================================= FUNCTIONS ==================================================================================//

int generateRandom(int min, int max){ 
  int randomNumber;
  randomNumber = min + rand() %(max+1-min); 
  return randomNumber;
}

CHECK validateUP(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]){
  CHECK check;
  int i, j;
  check.valid = true;
  check. slow = false;
  for(i=0;i<MHEIGHT;i++){
    for(j=0;j<MWIDTH;j++){
      if(mapMatrix[i][j]=='r' || mapMatrix[i][j]=='t' || mapMatrix[i][j]=='k'){
        if(players.all[id].pos_x+32-4>j*32 && players.all[id].pos_x<(j+1)*32-4 && players.all[id].pos_y-4<(i+1)*32 && players.all[id].pos_y-4>i*32){
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='g' || mapMatrix[i][j]=='f'){
        if(players.all[id].pos_x+8-4>j*32 && players.all[id].pos_x<(j+1)*32-4+8 && players.all[id].pos_y-4<(i+2)*32 && players.all[id].pos_y-4>(i+1)*32){
          check.valid = false;
          break;        
        }
      }
      else if(mapMatrix[i][j]=='s'){
        if(players.all[id].pos_x+32-4>j*32 && players.all[id].pos_x<(j+1)*32-4 && players.all[id].pos_y-4<(i+1)*32 && players.all[id].pos_y-4>i*32){
          check.slow = true;
          break;
        }
      }
    }
  }
  return check;
}

CHECK validateDOWN(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]){
  CHECK check;
  int i, j;
  check.valid = true;
  check.slow = false;
  for(i=0;i<MHEIGHT;i++){
    for(j=0;j<MWIDTH;j++){
      if(mapMatrix[i][j]=='r' || mapMatrix[i][j]=='t' || mapMatrix[i][j]=='k'){
        if(players.all[id].pos_x+32-4>j*32 && players.all[id].pos_x<(j+1)*32-4 && players.all[id].pos_y+4>(i-1)*32 && players.all[id].pos_y+4<i*32){
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='g' || mapMatrix[i][j]=='f'){
        if(players.all[id].pos_x+8-4>j*32 && players.all[id].pos_x<(j+1)*32-4+8 && players.all[id].pos_y+4>i*32+8 && players.all[id].pos_y+4<i*32+16){
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='s'){
        if(players.all[id].pos_x+32-4>j*32 && players.all[id].pos_x<(j+1)*32-4 && players.all[id].pos_y+4>(i-1)*32 && players.all[id].pos_y+4<i*32){
          check.slow = true;
          break;
        }
      }
    }
  }
  return check;
}

CHECK validateLEFT(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]){
  CHECK check;
  int i, j;
  check.valid = true;
  check.slow = false;
  for(i=0;i<MHEIGHT;i++){
    for(j=0;j<MWIDTH;j++){
      if(mapMatrix[i][j]=='r' || mapMatrix[i][j]=='t' || mapMatrix[i][j]=='k'){
        if(players.all[id].pos_y+32-4>i*32 && players.all[id].pos_y<(i+1)*32-4 && players.all[id].pos_x-4<(j+1)*32 && players.all[id].pos_x-4>j*32){
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='g' || mapMatrix[i][j]=='f'){
        if(players.all[id].pos_y+8-4>(i+1)*32-8 && players.all[id].pos_y<(i+2)*32-8 && players.all[id].pos_x-4+16<(j+2)*32 && players.all[id].pos_x-4>(j+1)*32) {
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='s'){
        if(players.all[id].pos_y+32-4>i*32 && players.all[id].pos_y<(i+1)*32-4 && players.all[id].pos_x-4<(j+1)*32 && players.all[id].pos_x-4>j*32){
          check.slow = true;
          break;
        }
      }
    }
  }
  return check;
}

CHECK validateRIGHT(PLAYER_DATA players, int id, char mapMatrix[MHEIGHT][MWIDTH]){
  CHECK check;
  int i, j;
  check.valid = true;
  check.slow = false;
  for(i=0;i<MHEIGHT;i++){
    for(j=0;j<MWIDTH;j++){
      if(mapMatrix[i][j]=='r' || mapMatrix[i][j]=='t' || mapMatrix[i][j]=='k'){
        if(players.all[id].pos_y+32-4>i*32 && players.all[id].pos_y<(i+1)*32-4 && players.all[id].pos_x+4>(j-1)*32 && players.all[id].pos_x+4<j*32){
          check.valid = false;
          break;
        }
      }
      else if(mapMatrix[i][j]=='g' || mapMatrix[i][j]=='f'){
        if(players.all[id].pos_y+8-4>(i+1)*32-8 && players.all[id].pos_y<(i+2)*32-8 && players.all[id].pos_x+4+16<(j+1)*32 && players.all[id].pos_x+4>j*32){
          check.valid = false;
          break;
        }
      }
      if(mapMatrix[i][j]=='s'){
        if(players.all[id].pos_y+32-4>i*32 && players.all[id].pos_y<(i+1)*32-4 && players.all[id].pos_x+4>(j-1)*32 && players.all[id].pos_x+4<j*32){
          check.slow = true;
          break;
        }
      }
    }
  }
  return check;
}

LIST generateList(PLAYER_DATA players, int connectedPlayers){
  LIST list;
  int i, j;
  for(i=0;i<MAX_PLAYERS;i++){
    strcpy(list.playerList[i].login, players.all[i].login);
    list.playerList[i].tagTime = players.all[i].tagTime; 
  }
  for(i=0;i<connectedPlayers;i++){
    for(j=0;j<connectedPlayers;j++){
      if(list.playerList[i].tagTime < list.playerList[j].tagTime && i>j) {
        do {
          LIST_ELEMENT temp = list.playerList[i];
          list.playerList[i]=list.playerList[j]; 
          list.playerList[j]=temp;
        } while (0);
      }
    }
  }
  return list;
}
