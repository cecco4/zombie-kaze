/**
 * @mainpage Zombie-kaze
 *
 * Zombie-kaze è un piccolo gioco arcade che si sviluppa su 10 livelli,
 * in ognuno dei quali bisogna uccidere tutti gli zombie sulla mappa.
 * Ogni nemico è dotato di una bomba e non esiterà a farsi esplodere nel caso
 * si trovi nelle vostre immediate vicinanze.
 * 
 * il loop principale del gioco è gestito dalla funzione ::main .
 * 
 * @author Francesco Gatti
 */



//C++ header
#include <iostream>
#include <math.h>
#include <cassert>
#define _USE_MATH_DEFINES

//allegro header
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>	
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


//header interni
#include "struttura_dati.h"
#include "input_output.h"
#include "raycast.h"
#include "logica_gioco.h"
#include "hud.h"

using namespace std;


//Debug
#ifdef DEBUG_MODE
	#include "debug.h"
#endif

/** Funzione principale.
 *
 * Si occupa di inizializzare le strutture dati del gioco e gestisce il ciclo
 * principale.
 *
 */

    //joystick    
    bool joy = false;   /**< true se c'è il joystick*/
    ALLEGRO_JOYSTICK *joystick = NULL;
    ALLEGRO_JOYSTICK_STATE joystate;


int main(void)
{

	bool done = false;
	bool redraw = true;

	
	int errore;
	bool caricato = false;

	bool tasto[12] = {false, false, false, false, false, false, false,
                      false, false, false, false, false};

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;


	//inizializzo Allegro
	if(!al_init())					
		return -1;

	//creo dysplay
	#ifdef DEBUG_MODE
        int scelta;
        cout<<"Seleziona dimensione schermo: \n";
        cout<<"1: 320x200\n";
        cout<<"2: 640x400\n";
        cout<<"3: 960x600\n";  
        cout<<"4: auto-fullscreen\n";
        cout<<"? ";
        cin>>scelta;

        if(scelta == 4)
            al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        
        display = al_create_display(lschermo*scelta, hschermo*scelta);
        
        int windowWidth, windowHeight;

        if(scelta ==4) {
            windowWidth = al_get_display_width(display);
            windowHeight = al_get_display_height(display);
        } else {
            windowWidth = lschermo*scelta;
            windowHeight = hschermo*scelta;
        }

        int screenWidth = lschermo;
        int screenHeight = hschermo;


        float sx = windowWidth / (float)screenWidth;
        float sy = windowHeight / (float)screenHeight;

        ALLEGRO_TRANSFORM trans;
        al_identity_transform(&trans);
        al_scale_transform(&trans, sx, sy);
        al_use_transform(&trans);        
	#else

        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        display = al_create_display(lschermo, hschermo);

        int windowWidth = al_get_display_width(display);
        int windowHeight = al_get_display_height(display);
        int screenWidth = lschermo;
        int screenHeight = hschermo;


        float sx = windowWidth / (float)screenWidth;
        float sy = windowHeight / (float)screenHeight;

        ALLEGRO_TRANSFORM trans;
        al_identity_transform(&trans);
        al_scale_transform(&trans, sx, sy);
        al_use_transform(&trans);

	#endif




	if(!display)					//test display
		return -2;

	//inizializzo addon allegro
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_acodec_addon();

	al_install_audio();
	al_reserve_samples(9);
	
	al_install_keyboard();


	//inizializza joystick
	if(al_install_joystick()) {

        	al_reconfigure_joysticks();
        	joystick=al_get_joystick(0);
        
        	if(joystick != NULL) {        
            		cout<<"Riconosciuto joystick "<<joystick<<endl;
            		joy = true;
        	}
    	}
	
	//font
	cout<<"Caricamento: data/ZOMBIE.TTF    ";
	ALLEGRO_FONT *font =al_load_font("data/ZOMBIE.TTF", 20, 0);
	if (font == NULL) {
		cout<<"errore\n";
		return -11;
	}
	cout<<"OK\n";

	//audio
	audio_dat audio;
	if (!carica_audio(audio))
		return -3;
	
	//immagini per i titoli
	immagine_dat sfondo = carica_immagine(sfondo, "data/sprite/sfondo.png");
	immagine_dat titolo = carica_immagine(titolo, "data/sprite/titolo.png");
	immagine_dat gameover = carica_immagine(titolo, "data/sprite/gameover.png");
	immagine_dat fine = carica_immagine(titolo, "data/sprite/fine.png");
	if (sfondo.img == NULL || titolo.img == NULL || 
		gameover.img == NULL || fine.img == NULL	)
		return -4;
	titolo.x = sfondo.x = lschermo/2;
	titolo.y = sfondo.y = hschermo/2;
	gameover.alpha = 0;
	
	//immagine finale del rendering
	ALLEGRO_BITMAP *render_bitmap = al_create_bitmap(lschermo, hschermo);
	//matrice del rendering
	ALLEGRO_COLOR render_matrix[hschermo][lschermo];
	
	//stato iniziale gioco
	STATO stato_gioco = CARICA;
	char livello = '0';

	//inizializzo il resto delle strutture
	mappa_dat map; 
	texture_dat *texture;
	
	player_dat player;
	pistola_dat pistola;
	nemici_dat nemici; 



	
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
    if(joy)
        al_register_event_source(event_queue, al_get_joystick_event_source());

	al_start_timer(timer);
	//GAME LOOP:
	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
        
        if(joy)
            al_get_joystick_state(joystick, &joystate);

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			aggiorna_tasti(ev, tasto, true);
			 
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
			aggiorna_tasti(ev, tasto, false);
		 
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{			
			#ifdef DEBUG_MODE
				if(tasto[P]) {
					DEBUG_CONSOLE;
				}
			#endif
		
            //update
            if(joy) 
                tasto[INVIO] = joystate.button[0];

            
			switch (stato_gioco) {
        
        			
			case CARICA:
				if (!caricato) {
					errore = carica_ambiente(	livello, map, texture, player, 
												pistola, nemici);

					if (errore != 0)
						return errore;
					caricato = true;
					
					al_play_sample(	audio.titolo, 1.0, 0.0,1.0,
									ALLEGRO_PLAYMODE_LOOP, &audio.m_id);
				}
				anim_titolo(titolo, sfondo, stato_gioco, livello);
				
				if (tasto[INVIO]) {
					//inizia la fase di gioco
					stato_gioco = PLAY;
					al_stop_sample(&audio.m_id);
					al_play_sample(	audio.atmosfera, 1.0, 0.0,1.0,
									ALLEGRO_PLAYMODE_LOOP, &audio.m_id);				
				}
			break;
				
			case PLAY:
				assert(caricato);
				
				if (titolo.sizex >0)
					anim_titolo(titolo, sfondo, stato_gioco, livello);
			
				muovi_player(player, map, tasto, audio);
				
				gestisci_sparo(player, nemici, pistola, tasto, audio);
                anim_pistola(pistola);

				muovi_nemici(player, map, nemici);
				anima_nemici(nemici);
				stato_gioco = controlla_stato_nemici(nemici, audio);
				
				if (stato_gioco == CARICA) {
					al_stop_sample(&audio.m_id);
					
					/* se è all'ultimo livello si ricicla lo stato game over 
					   sostituendo l'immagine della sconfitta con quella della 
					   fine del gioco 										  */
					if (livello == '9') {   
						stato_gioco = GAME_OVER;
						gameover.img = fine.img;
						al_stop_sample(&audio.m_id);
						al_play_sample(	audio.vittoria, 2.5, 0, 1, 
										ALLEGRO_PLAYMODE_ONCE, NULL);
					}
					livello++;
                    pistola.tmax -= 3;
                    pistola.t = 0;
					dealloca_ambiente(map, texture, nemici, pistola);
					caricato = false;
					titolo.angle = sfondo.angle = 0;
					titolo.y = sfondo.y = -hschermo/2;
				}
				
			break;
			
			case GAME_OVER:
                static int wait=0;
                wait++;
				gameover.alpha < 255? gameover.alpha+=5 : gameover.alpha = 255;
				
				if (wait>700 && tasto[INVIO]) {	
					stato_gioco = CARICA;
                    livello='0'; 
                    dealloca_ambiente(map, texture, nemici, pistola);     
                    caricato = false;
                    wait=0;          
                }
				break;
			}
			
			
			redraw = true;
		}

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

			//draw
			
			switch (stato_gioco) {
		
			case CARICA:
				al_draw_bitmap(render_bitmap,0,0,0);
				disegna_pistola(pistola);
				
				disegna_titolo(titolo, sfondo);

			break;
			
			case PLAY:
						
				pulisci_render_matrix(render_matrix);	
						
				al_set_target_bitmap(render_bitmap);	
					al_clear_to_color(al_map_rgb(0,0,0)); 
			
					raycast(player, nemici, map, texture, render_matrix);
					applica_render_matrix(render_matrix, render_bitmap);
					
					al_draw_textf(	font, al_map_rgb(255,15,15), 10, 10, 0,
									"Livello %d", int(livello)-int('0')+1	);

					al_draw_textf(	font, al_map_rgb(255,15,15), 10, 30, 0, 
									"Zombie Rimasti %d", nemici.nem_vivi	);
																						
				al_set_target_bitmap(al_get_backbuffer(display));	
		
				al_draw_bitmap(render_bitmap,0,0,0);
				disegna_pistola(pistola);

				if (titolo.sizex >0)	
					disegna_titolo(titolo, sfondo);
			
			break;
			
			case GAME_OVER:
				al_draw_bitmap(render_bitmap,0,0,0);
				disegna_pistola(pistola);
				
				al_draw_tinted_bitmap(	gameover.img, al_map_rgba(255, 255, 255, 
										gameover.alpha), 0, 0, 0);
			break;
			}
			
			al_flip_display();   //applico disegno
			al_clear_to_color(al_map_rgb(0,0,0));   // pulisco display
			
		}
		

	}



	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);		//distruggo display

	return 0;
}


