#include "ACore.h"


/*
INITIALIZATION OF BASIC CORE MODULES:
--ALLEGRO
--ALLEGRO IMAGE
--ALLEGRO FONTS
--ALLEGRO TTF FONT SUPPORT
--ALLEGRO PRIMITIVES
--EVENT QUEUE
*/
bool coreInit()
{
    //modules and add-ons initialization
	if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }

    if (!al_init_font_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_font.\n");
        return false;
    }

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_primitives.\n");
        return false;
    }

    eventsQueue = al_create_event_queue();
    if (!eventsQueue)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        return false;
    }
    
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return false;
    }
    timer1 = al_create_timer(1.0 / FPS2);
    if(!timer) {
        fprintf(stderr, "failed to create timer1!\n");
        return false;
   }

 	

    return true;
}


//FOR INITIALIZING A WINDOW OF WxH SIZE WITH TEXT "title[]"
bool windowInit(int W, int H, char title[])
{
    //window and event queue creation
    main_window = al_create_display(W, H);
    if (!main_window)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }
    al_set_window_title(main_window, title);

    //registra janela na fila de eventos
    al_register_event_source(eventsQueue, al_get_display_event_source(main_window));

    return true;
}


//FOR INITIALIZING MAIN EXTERNAL INPUTS (KEYBOARD, MOUSE AND CURSOR)
bool inputInit()
{
	/*------------------------------MOUSE--------------------------------*/
	//Inicializa Mouse
	if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
        al_destroy_display(main_window);
        return false;
    }

    // Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(main_window, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(main_window);
        return false;
    }
    /*------------------------------MOUSE--------------------------------*/

    /*------------------------------KEYBOARD------------------------------*/
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    /*------------------------------KEYBOARD------------------------------*/


    //Registra mouse e teclado na fila de eventos
    al_register_event_source(eventsQueue, al_get_mouse_event_source());
    al_register_event_source(eventsQueue, al_get_keyboard_event_source());
    al_register_event_source(eventsQueue, al_get_timer_event_source(timer1));

    return true;
}


//FOR READING KEYBOARD INPUT WITH MAX SIZE = LIMIT AND SAVING AT STR[]
void readInput(ALLEGRO_EVENT event, char str[], int limit)
{
    if (event.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= limit)
        {
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
    }
}


//FPS CONTROL (FPS IS DEFINED IN ACORE.H)
void startTimer()
{
    startingTime = al_get_time();
}

double getTimer()
{
    return al_get_time() - startingTime;
}

void FPSLimit()
{
    if (getTimer() < 1.0/FPS)
    {
        al_rest((1.0 / FPS) - getTimer());
    }
}


//FOR DEALLOCATING ALL ALLEGRO STUFF
void allegroEnd()
{
    al_destroy_display(main_window);
    al_destroy_event_queue(eventsQueue);
}


//MODIFY THIS TO LOAD YOUR OWN FONTS (FONT POINTERS ARE DEFINED AT ACORE.H)
bool fontInit()
{
    /*------------------------------FONTE--------------------------------*/
    start = al_load_font("game/Resources/Fonts/pressStart.ttf", 16, 0);
    if (!start)
    {
        fprintf(stderr, "Falha ao carregar \"game/Resources/Fonts/pressStart.ttf\".\n");
        return false;
    }

    largeStart = al_load_font("game/Resources/Fonts/pressStart.ttf", 30, 0);
    if (!start)
    {
        fprintf(stderr, "Falha ao carregar \"game/Resources/Fonts/pressStart.ttf\".\n");
        return false;
    }

    ubuntu = al_load_font("game/Resources/Fonts/Ubuntu-R.ttf", 20, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Ubuntu-R.ttf\".\n");
        return false;
    }

    vegan = al_load_font("game/Resources/Fonts/Vegan_Style.ttf", 40, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Vegan_Style.ttf\".\n");
        return false;
    }

    ostrich = al_load_font("game/Resources/Fonts/ostrich.ttf", 60, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"ostrich.ttf\".\n");
        return false;
    }

    digitalt = al_load_font("game/Resources/Fonts/Digitalt.ttf", 60, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Digitalt.ttf\".\n");
        return false;
    }

    comfortaa_bold = al_load_font("game/Resources/Fonts/Comfortaa-Bold.ttf", 24, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Comfortaa-Bold.ttf\".\n");
        return false;
    }

    comfortaa_regular = al_load_font("game/Resources/Fonts/Comfortaa-Regular.ttf", 24, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Comfortaa-Regular.ttf\".\n");
        return false;
    }

    comfortaa_light = al_load_font("game/Resources/Fonts/Comfortaa-Light.ttf", 24, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Comfortaa-Light.ttf\".\n");
        return false;
    }

    comfortaa_bold_small = al_load_font("game/Resources/Fonts/Comfortaa-Bold.ttf", 16, 0);
    if (!ubuntu)
    {
        fprintf(stderr, "Falha ao carregar \"Comfortaa-Bold.ttf\".\n");
        return false;
    }

    return true;
}


//MODIFY THIS TO LOAD YOUR OWN GRAPHICS (BITMAP POINTERS ARE DEFINED AT ACORE.H)
bool loadGraphics()
{
    menuScreen = al_load_bitmap("game/Resources/Etc/3.png");
    if (!menuScreen)
    {
        fprintf(stderr, "Falha carregando menuScreen\n");
        return false;
    }

    objects = al_load_bitmap("game/Resources/Tilesets/objects.png");
    if (!objects){
        fprintf(stderr, "Falha carregando objects.png\n");
        return false;
    }
    objects2 = al_load_bitmap("game/Resources/Outside_B.png");
    if (!objects){
        fprintf(stderr, "Falha carregando Outside_B.png\n");
        return false;
    }

    characterSheet1 = al_load_bitmap("game/Resources/People4.png");
    if (!objects){
        fprintf(stderr, "Falha carregando objects.png\n");
        return false;
    }
    tag = al_load_bitmap("game/Resources/!Other1.png");
    if (!objects){
        fprintf(stderr, "Falha carregando tag\n");
        return false;
    }

    return true;
}

bool loadMap()
{
    int i, j;
    char element;
    FILE *mapFile_0;
    mapFile_0 = fopen("game/Resources/Maps/prototipo.txt", "rt");
    if(mapFile_0 != NULL) {
        for(i=0;i<MHEIGHT;i++){
            for(j=0;j<MWIDTH;j++){
                fscanf(mapFile_0, " %c", &element);
                mapMatrix_0[i][j] = element;
            }
        }
        fclose(mapFile_0);
    }
    FILE *mapFile_1;
    mapFile_1 = fopen("game/Resources/Maps/foco.txt", "rt");
    if(mapFile_1 != NULL) {
        for(i=0;i<MHEIGHT;i++){
            for(j=0;j<MWIDTH;j++){
                fscanf(mapFile_1, " %c", &element);
                mapMatrix_1[i][j] = element;
            }
        }
        fclose(mapFile_1);
    }
    FILE *mapFile_2;
    mapFile_2 = fopen("game/Resources/Maps/skull.txt", "rt");
    if(mapFile_2 != NULL) {
        for(i=0;i<MHEIGHT;i++){
            for(j=0;j<MWIDTH;j++){
                fscanf(mapFile_2, " %c", &element);
                mapMatrix_2[i][j] = element;
            }
        }
        fclose(mapFile_2);
    }
    return true;
}