//C++ header
#include <iostream>
#include <cassert>

//allegro header
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

//header interni
#include "struttura_dati.h"
#include "hud.h"

using namespace std;

void anim_pistola(pistola_dat &pist) {

	static float anim = pist.FERMO_off;  //animazione su giù della pistola 
	static bool giu = true;
	static int tempo=0;
	static bool avanti = true;
	
	assert(anim>= -pist.CAMMINA_off && anim<= 2*pist.CAMMINA_off);
	assert(tempo>= 0 && tempo <= pist.t_SPARA);

	float x = lschermo/2 - pist.frame_l/2 - 18;
	float y = hschermo - pist.img_h;
	

	switch (pist.stato_anim) {
		case FERMO:
			pist.frame = 0;
			giu ? anim -= pist.FERMO_vel : anim += pist.FERMO_vel;
			anim >= pist.FERMO_off? giu = true : giu ;
			anim <= 0? giu = false : giu ;
			
			y+= anim;
			break;
		
		case CAMMINA:
			pist.frame = 0;
			giu ? anim -= pist.CAMMINA_vel : anim += pist.CAMMINA_vel;
			anim >= pist.CAMMINA_off? giu = true : giu ;
			anim <= 0? giu = false : giu ;
				
			y += anim;
			break;
			
		case SPARA:
			pist.frame = tempo*pist.nframe/pist.t_SPARA;
			avanti? tempo++ : tempo--;
			
			if (!avanti && pist.frame == 1)   // al ritorno non c'è fiamma
				pist.frame = 5;   
			
			if (tempo >= pist.t_SPARA - pist.t_SPARA/pist.nframe/2) 
				avanti= false;
			
			if (tempo <= 0) {
				tempo =0;
				pist.stato_anim = FERMO;
				avanti = true;
			}
			break;
	}
	
	pist.x = x;
	pist.y = y;
}


void disegna_pistola(pistola_dat &pist) {
	al_draw_bitmap_region(	pist.img, pist.frame_l*pist.frame, 0, 
							pist.frame_l, pist.img_h, pist.x, pist.y, 0);
							
}
							
							
							
void anim_titolo(immagine_dat &tit, immagine_dat &sf, STATO stato_gioco, char lv)
{
	static float anim = 0.9;
	static int su =1;
	
	assert(anim>= -1 && anim <= 2);

	if (stato_gioco == CARICA) {
		//animazione titolo, sfondo statico
		if (anim <= 0)
			anim = 0.9;
	
		anim += su*0.008;
	
		if (anim > 1)
			su = -1;
	
		if (anim < 0.9)
			su = 1;
			
		tit.sizex = anim;
		tit.sizey = anim;
		sf.sizex = 1;
		sf.sizey = 1;
	}
	
	if (stato_gioco == PLAY) {
		//titolo e sfondo si rimpiccioliscono ruotando
		anim -= 0.02;
			
		tit.sizex = anim;
		tit.sizey = anim;
		tit.angle += 0.6;
		sf.sizex = anim;
		sf.sizey = anim;
		sf.angle += 0.6;
	}
	
	//lo sfondo e il titolo appaiono dall'alto
	if (sf.y < hschermo/2)  
		sf.y = tit.y += 10;  
	else
		sf.y = tit.y = hschermo/2;
		
}						
		
void disegna_titolo(immagine_dat &titolo, immagine_dat &sfondo)
{
	al_draw_scaled_rotated_bitmap(sfondo.img, sfondo.l/2, sfondo.h/2, 
		sfondo.x, sfondo.y, sfondo.sizex, sfondo.sizey, sfondo.angle, 0);

	al_draw_scaled_rotated_bitmap(titolo.img, titolo.l/2, titolo.h/2, 
		titolo.x, titolo.y, titolo.sizex, titolo.sizey, titolo.angle, 0);

}	

			
							
