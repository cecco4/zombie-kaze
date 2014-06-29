//C++ header
#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES

//allegro header
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

//header interni
#include "struttura_dati.h"
#include "raycast.h"

using namespace std;

//Funzioni interne al modulo

/** Ordina i nemici in base alla distanza dal giocatore.
 *  questa funzione implementa un algoritmo di sorting molto semplice
 *  (bubble sort) in quanto i nemici da ordinare sono di un ordine di grandezza
 *  basso.
 */
static void sort_nemici(int a[],int n, nemici_dat &nem);

/** Ritorna true se il colore dato per parametro è uguale al colore usato per
 *  identificare la trasparenza di una immagine
 */
static bool alpha_cmp(ALLEGRO_COLOR c);

/** Disegna le sprite dei nemici sulla render matrix. */
static void disegna_sprite( int n ,player_dat &pl, nemici_dat &nem, 
							mappa_dat &map, int zbuffer[],
						 	ALLEGRO_COLOR render[][lschermo]);

/** Conferisce un effetto di distanza ai muri scurendoli	*/						
static ALLEGRO_COLOR effetto_distanza ( ALLEGRO_COLOR col, double dist );


double to_360 (double ang)
{
	ang >= 360? ang=ang-360 : ang;
	ang < 0 ? ang =ang+360 : ang;
	return ang;
}



double rad (double ang)
{	
	return ang* M_PI / 180.0;
}


double grad (double ang)
{	
	return ang / M_PI * 180.0;
}



void raycast (  player_dat &pl, nemici_dat &nem, mappa_dat &map, 
				texture_dat *texture, ALLEGRO_COLOR render[][lschermo]	)
{
	
	//vettore che conterrà tutte le distanze dei muri
	int zbuffer[lschermo];	
	
	//vettore che conterrà tutti i blocchi in cui passeranno i raggi
	bool blocchi_in_vista[map.dim][map.dim];  
	for (int i=0; i<map.dim; i++) for (int j=0; j<map.dim; j++)
		blocchi_in_vista[i][j] = false;

	
	
	for (int i=0; i<lschermo; i++) {
		//calcolo angolo del raggio (in gradi)
		double angray = to_360( pl.ang + pl.fov/2 - (pl.fov/lschermo)*i );
		double tg_ang = tan(rad(angray)); // si usa spesso quindi risparmio   

		//direzione raggio
		bool su, dx;     
		if (angray > 0 && angray < 180)    
			su = true;
		else 
			su = false;    //  !su = giù
			
		if (angray > 90 && angray < 270)
			dx = false;	   //  !dx = sx	
		else 
			dx = true;	
			

		//intersezioni orrizzontali
		double xo, yo;		//punto prima intersezione
		if (su) 
			yo = pl.y - (fmod(pl.y, map.u));
		else 
			yo = pl.y - (fmod(pl.y, map.u)) + map.u;
			
		xo = pl.x + (pl.y - yo) / tg_ang;
		
		double xs,ys; // variabili step
		xs= map.u / tg_ang;
			
		if (su) 
			ys = -map.u; 
		else {
			ys = map.u; 
			xs = -xs;
		}
		
		int tile_o;
		while(	yo/map.u >= 0  && yo/map.u<map.dim  &&
				xo/map.u >= 0  && xo/map.u<map.dim    	) {
				
			tile_o = map.data[int(yo/map.u)-1*su][int(xo/map.u)];
			
			if (tile_o != 0)
				break;
			
			blocchi_in_vista[int(yo/map.u)-1*su][int(xo/map.u)] = true;  
				
			xo += xs;
			yo += ys;
		}

		
		//intersezioni verticali
		double xv, yv;		//punto prima intersezione
		if (dx)
			xv = pl.x - (fmod(pl.x, map.u)) + map.u;
		else
			xv = pl.x - (fmod(pl.x, map.u));

		yv = pl.y + (pl.x - xv) * tg_ang;
		
		
		//variabili step
		ys = map.u * tg_ang;
			
		if (dx) {
			xs = map.u;
			ys = -ys;
		} else
			xs = -map.u;

		int tile_v;		
		while(  yv/map.u >= 0  && yv/map.u<map.dim  &&
				xv/map.u >= 0  && xv/map.u<map.dim	   	) {
		
			tile_v = map.data[int(yv/map.u)][int(xv/map.u)-1*!dx];		
			
			if (tile_v != 0)
				break;
				
			blocchi_in_vista[int(yv/map.u)][int(xv/map.u)-1*!dx] = true;  
				
			xv += xs;
			yv += ys;
		}	 

		
		//calcolo distanze intersezioni
		double d_orr =  pow(pl.x - xo,2) + pow (pl.y - yo,2);
		double d_vert =  pow(pl.x - xv,2) + pow (pl.y - yv,2);
	
		
		double dist;
		bool shade=false; 
		int off; //offset texture
		int ntile;
		 
		//prendo la dist minore
		if (d_orr < d_vert) {   
			dist = d_orr; 
			shade=true;   // colorazione più scura dei muri orriz
			
			ntile = tile_o;
			off = int(fmod(xo, map.u));
		} else {
			dist = d_vert;
			
			ntile = tile_v;
			off = int(fmod(yv, map.u)); 
		}

		dist = sqrt(dist);

		if (i==lschermo/2)
			pl.dist_m = dist; 
		
		
		if (dist==0)
			dist = 0.01;
			
		zbuffer[i] = dist;  //aggiorno zbuffer


		dist = dist * cos(rad(to_360(pl.ang-angray))); //tolgo distorsione
				
		double h_linea = (map.u / dist) * dist_schermo;
		
 
		
   		for (int k=0; k<h_linea; k++) {
   		
   			int yoff =  map.u - map.u*k / h_linea;
   			
   			if (yoff<0 && yoff>=lschermo)
   				continue;
   			
   			int screen_pos = hschermo/2+h_linea/2-k;
   			
   			if (screen_pos < hschermo && screen_pos >= 0) {
   				ALLEGRO_COLOR col = texture[ntile].img[yoff%map.u][off];
   				
				col = effetto_distanza(col, dist);
   				
   				if (shade) {
   		  			col.r /= 1.5;
   					col.g /= 1.5;
   					col.b /= 1.5;
   				}
   				
   				
   				
   				render[screen_pos][i] = col; 
   			}
   		}
   			

		
			
		//rendering pavimento e soffitto   
		for (int j=hschermo/2+h_linea/2-1;j>=0 && j<hschermo; j++) {
	
			double deltaPJ =  j- hschermo/2;
			double p_ydist = pl.h / deltaPJ * dist_schermo; 
			double p_dist = p_ydist / cos(rad(angray-pl.ang));
		
		
					
			int px = pl.x + p_dist*cos(-1*rad(angray));
			int py = pl.y + p_dist*sin(-1*rad(angray));
			
					
			px<0 ? px = -px : px;
			py<0 ? py = -py : py;
			
			
			px = px % map.u; //calcolo il pixel da pescare
			py = py % map.u;
				 
			//pavimento:
			render[j][i] = effetto_distanza(texture[0].img[py][px],p_dist);
			//soffitto:
			render[hschermo-j-1][i] = effetto_distanza(texture[1].img[py][px],p_dist);  
			
		} 
	
	
	}
	
	
	
		
	//disegno sprite


	//calcolo le sprite che sono da disegnare
	int da_disegnare[nem.num_nemici];
	int n_da_disegnare=0;
	for (int n=0; n < nem.num_nemici; n++) {
		double xsprite = nem.nemico[n].x;
		double ysprite = nem.nemico[n].y; 
		nem.nemico[n].vis = false;
		
		if(blocchi_in_vista[int(ysprite/map.u)][int(xsprite/map.u)]) {		
			da_disegnare[n_da_disegnare] = n;
			n_da_disegnare++;
			nem.nemico[n].vis = true;
			nem.nemico[n].insegui = true;	
		}
	}
	//calcolo distanze
	for (int n=0; n < n_da_disegnare; n++) {
		int n_spr = da_disegnare[n];
		nem.nemico[n_spr].dist = 
			sqrt(	pow(pl.x - nem.nemico[n_spr].x,2) + 
					pow (pl.y - nem.nemico[n_spr].y,2)		);

	}	
	
	//metto le sprite in fila in base alla distanza
	sort_nemici(da_disegnare, n_da_disegnare, nem);
	
	//disegno le sprite in fila
	for (int n=0; n < n_da_disegnare; n++) {
		int n_spr = da_disegnare[n];
		disegna_sprite(n_spr, pl, nem, map, zbuffer, render);
    }
   			
	
}



void pulisci_render_matrix(ALLEGRO_COLOR render_matrix[hschermo][lschermo])
{
	for( int i=0; i<hschermo; i++)
		for( int j=0; j<lschermo; j++)	
			render_matrix[i][j] = al_map_rgb(0,0,0);

	
}



void applica_render_matrix( ALLEGRO_COLOR render_matrix[hschermo][lschermo],
							ALLEGRO_BITMAP *rendering)
{
	al_lock_bitmap(rendering, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);

	for (int i=0; i<hschermo; i++) 
		for (int j=0; j<lschermo; j++) {
		
			al_put_pixel(j, i,	render_matrix[i][j] );			
		}

	al_unlock_bitmap(rendering);	
}



static ALLEGRO_COLOR effetto_distanza ( ALLEGRO_COLOR col, double dist )
{
	double blend = dist*2/1000; 
	
	if (blend>1) {
		col.r /= blend;
		col.g /= blend;
		col.b /= blend;		
	}

	return col;
}



static void disegna_sprite( int n ,player_dat &pl, nemici_dat &nem, 
							mappa_dat &map, int zbuffer[],
						 	ALLEGRO_COLOR render[][lschermo])
{	
	double xsprite = nem.nemico[n].x;
	double ysprite = nem.nemico[n].y; 	
	int l = map.u;
			
	double dx = xsprite - pl.x;
	double dy = ysprite - pl.y;


	double spr_ang = to_360(grad(atan2(dy, dx))+pl.ang+pl.fov);	
		
	double dists = nem.nemico[n].dist - fabs(pl.fov-spr_ang)/3 ;
	
	
	double lproj =  (l/ dists) * dist_schermo;
	
	int posx = (spr_ang-pl.fov/2)*lschermo / pl.fov;
	
	double hs = (l / dists) * dist_schermo;
	
	if (lproj<= 0 || lproj>lschermo*2 || hs<=0 || hs>hschermo*2)
		return;

	for(int j=0; j<lproj/2;j++) {
	
	 	for (int k=0; k<hs; k++) {
   		
   			int posy = hschermo/2 - hs/2 +k; 		
   		
   			if ( posy<hschermo && posy>=0) {
   				int xoff = int(j*l/lproj) ;
   				int yoff = int(k*l/hs) %l;
				int frame = nem.nemico[n].frame; 
					
   				if (zbuffer[posx+j]>dists && posx+j<lschermo && posx+j>=0 ) {
   					
   					ALLEGRO_COLOR col = nem.text[frame].img[yoff][32+xoff];	
   					if (alpha_cmp(col))
   						render[posy][posx+j] = effetto_distanza(col,dists); 
   				}
   				if (zbuffer[posx-j]>dists && posx-j>=0 && posx-j <lschermo) {
   					
   					ALLEGRO_COLOR col = nem.text[frame].img[yoff][31-xoff];
					if (alpha_cmp(col))
   						render[posy][posx-j] = effetto_distanza(col,dists); 
   				}
   			}
   		}
   	}


}



static void sort_nemici(int a[],int n, nemici_dat &nem)
{
	bool scambio=true;
	while (scambio)
	{	
		scambio=false;
		for (int i=1; i<n; i++)
		{
			if (nem.nemico[a[i]].dist>nem.nemico[a[i-1]].dist) {   		
				int temp = a[i];
				a[i] = a[i-1];
				a[i-1] = temp;
				scambio=true;
			}	
		}
	}


}

static bool alpha_cmp(ALLEGRO_COLOR c)
{
	if (c.r == 1 && c.g == 0 && c.b == 1)
		return false;
	
	return true;
}




