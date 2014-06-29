//C++ header
#include <iostream>
#include <fstream>

//allgro header
#include <allegro5/allegro.h>

//header interni
#include "struttura_dati.h"

using namespace std;

//Funzioni interne al modulo
/** Ordina un array di interi dall'elemento p a r */
static void quick_sort(int a[], int p, int r);
/** Funzione necessaria a ::quick_sort */
static int partition(int a[], int p, int r);


mappa_dat carica_mappa(char n_mappa)
{
	assert(n_mappa >= '0' && n_mappa<= '9');
	
	char nome[20] = "data/mappe/lv .map";
	nome[13] = n_mappa;

	ifstream f(nome);
	
	mappa_dat map; 
	
	map.data = NULL;
	 
	if (!f)	return map; 
	

	f>>map.dim;

	map.data = new int * [map.dim]; 
	f.get();
	
	f>>map.pav>>map.soff;
	
	f.get();

	for (int i=0; i<map.dim; i++) {
		map.data[i] = new int[map.dim];
		
		for (int j=0; j<map.dim; j++) {
			char c;
			f.get(c);
			map.data[i][j] = int(c);
		}
		f.get();
	}	 
	 
	return map;	 
}


			
int controlla_texture_mappa(mappa_dat &map, int *text_list, nemici_dat &nem)
{

	const int d = map.dim;
	int tmp [d*d];
	
	
	//procedura per ottenere quanti simboli divesi ci sono sulla mappa
	for(int i=0; i<d ; i++)
		for(int j=0; j<d ; j++) {
			int c = map.data[i][j];
			
			//salta le posizioni dei nemici
			if (c == int('N')) {
				nem.num_nemici++;
				c=int('0');  //mette la casella passabile
			}
			
			tmp[j+i*d] = c;
		}
		
	quick_sort(tmp,0,d*d-1);


	int n_text=2;
	
	//inizializzo hashing 
	int hashing[256];	
	for (int i=0; i<256; i++)
		hashing[i] = 0;
	
	text_list[0] = map.pav;	 //pavimento e soffitto
	text_list[1] = map.soff;	
	
	for(int i=1; i<d*d ; i++) {
		if (tmp[i] == 0)
			continue;

		if (tmp[i] != tmp[i-1]) {
			text_list[n_text] = tmp[i];
			hashing[int(tmp[i])] = n_text++;
		}
	}
				
		
	char nome[20] = "data/texture/ .png";			
	for(int i=0; i<n_text ; i++) {	
	
		nome[13] = text_list[i];
		cout<<"Check : "<<nome<<"    ";
		
		ifstream f(nome);

			if (!f) {
				cout<<"errore\n";
				return -1;	
			} else
				cout<<"OK\n";
				
		f.close();
	
	}

	
	
	//setto la mappa con l'hashing in modo che ogni campo punti alla
	//corrispettiva texture
	
	for(int i=0; i<map.dim ; i++)
		for(int j=0; j<map.dim ; j++) {
			if (map.data[i][j] != 0 && map.data[i][j] != int('N')) {
				map.data[i][j] = hashing[map.data[i][j]];
			}
	}
	
	assert(n_text>2);
	
	return n_text;
}





bool carica_texture_mappa(texture_dat texture[], int *text_list, int num_tex) 
{
	char nome[20] = "data/texture/ .png";	

	for (int i=0; i<num_tex; i++) {
	
		nome[13] = text_list[i];
		
		cout<<"Caricamento: "<<nome<<"    ";
		
		ALLEGRO_BITMAP *tile = al_load_bitmap(nome);
		
		if (tile == NULL) {
			cout<<"errore\n";
			return false;
		} 
				
			
		al_lock_bitmap(tile, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
		
		for(int k=0; k<dimensione_tile; k++)
			for(int j=0; j<dimensione_tile; j++)
				texture[i].img[k][j] = al_get_pixel(tile, j, k);
		
		al_unlock_bitmap(tile);
		
		al_destroy_bitmap(tile);
		
		cout<<"OK\n"; //caricato correttamente
	} 

	

	return true;

}



bool carica_texture_nemico(texture_dat text[]) 
{
	char nome[25] = "data/sprite/nemico.png";	
	
	cout<<"Caricamento: "<<nome<<"    ";

	ALLEGRO_BITMAP *sprite = al_load_bitmap(nome);
	
	if (sprite == NULL) {
			cout<<"errore\n";
			return false;
	} 
			
	al_lock_bitmap(sprite, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	
	for (int n=0; n<10; n++) 	
		for(int k=0; k<dimensione_tile; k++)
			for(int j=0; j<dimensione_tile; j++) 
				text[n].img[k][j] = al_get_pixel(sprite, j+n*dimensione_tile, k);
		
	al_unlock_bitmap(sprite);
	
	al_destroy_bitmap(sprite);
	
	cout<<"OK\n";	//caricato correttamente
	
	return true;

}


immagine_dat carica_immagine (immagine_dat img, const char *path)
{
	cout<<"Caricamento: "<<path<<"    ";
	
	img.img = al_load_bitmap(path);
	if (img.img == NULL)
		cout<<"errore\n";
	else {	
		img.l = al_get_bitmap_width(img.img);
		img.h = al_get_bitmap_height(img.img);
		img.angle = 0;
		img.alpha = 255;
		
		cout<<"OK\n";
	}
	return img;
}



void aggiorna_tasti(ALLEGRO_EVENT &ev, bool tasto[], bool set)
{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					tasto[SU] = set;
					break;
				case ALLEGRO_KEY_DOWN:
					tasto[GIU] = set;
					break;
				case ALLEGRO_KEY_RIGHT:
					tasto[DX] = set;
					break;
				case ALLEGRO_KEY_LEFT:
					tasto[SX] = set;
					break;
				case ALLEGRO_KEY_SPACE:
					tasto[SPAZIO] = set;
					break;
				case ALLEGRO_KEY_ENTER:
					tasto[INVIO] = set;
					break;
				case ALLEGRO_KEY_P:
					tasto[P] = set;
					break;
			}
}			
		
		


static void quick_sort(int a[], int p, int r)
{
	if (p<r) {
		int i = partition(a,p,r); 
		quick_sort(a,p,i-1);
		quick_sort(a,i+1,r);
	}
}
	

static int partition(int a[], int p, int r)
{
    while (p<r) {
	
		while(a[p]<a[r])
			r--;
	
		if (p<r) {
			int tmp = a[p];
			a[p] = a[r];
			a[r] = tmp;
			p++; 
		}

		while(a[r]>a[p])
			p++;

		if (p<r) {
			int tmp = a[p];
			a[p] = a[r];
			a[r] = tmp;
			r--; 
		}
	}
	return p;
		
}


static bool carica_sample(ALLEGRO_SAMPLE *&smp , const char *path)
{
	cout<<"Caricamento: "<<path<<"    ";
		
	smp = al_load_sample(path);

	if (smp == NULL) {
		cout<<"errore\n";
		return false;
	} else {
		cout<<"OK\n";
		return true;
	}
}

bool carica_audio(audio_dat &audio)
{
	bool ret = true;
	ret &= carica_sample(audio.atmosfera, "data/audio/atmosfera.flac");
	ret &= carica_sample(audio.titolo, "data/audio/titolo.wav");
	ret &= carica_sample(audio.vittoria, "data/audio/vittoria.wav");
	ret &= carica_sample(audio.gameover, "data/audio/gameover.wav");
	ret &= carica_sample(audio.morte, "data/audio/morte.wav");
	ret &= carica_sample(audio.passo, "data/audio/passo.wav");
	ret &= carica_sample(audio.pistola, "data/audio/pistola.wav");
	ret &= carica_sample(audio.zombie, "data/audio/zombie.wav");
	ret &= carica_sample(audio.bomba, "data/audio/bomba.wav");
	
	return ret;

}
		
			
			
			
