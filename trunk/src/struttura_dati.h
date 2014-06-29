//C++ header
#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES

//allegro header
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

using namespace std;


const int lschermo = 320;	/**< Larghezza dello schermo */
const int hschermo = 200;	/**< Altezza dello schermo   */  

const int dimensione_tile = 64; /**< Dimensione unità della mappa */
const int FOV = 60;				/**< Field of view della camera   */
const int FPS = 60;		   		/**< Framerate del gioco          */



/** distanza del piano di proiezione dallo schermo */
const double dist_schermo = (lschermo/2) /  tan(FOV/2* M_PI / 180.0);

/** Stato del gioco, serve al gameloop (nel ::main) per eseguire diverse
 *  operazioni a seconda della fase in cui si trova il gioco
 */ 
enum STATO { CARICA, PLAY, GAME_OVER };

/** Valori utilizzati dall'array tasto (nel ::main) */
enum TASTI{ SU, GIU, SX, DX, SPAZIO, INVIO, P};

/** Stati dell'animazione della pistola */
enum ANIM_PIST { FERMO , CAMMINA, SPARA };


/** Struttura contenente i dati del giocatore */
struct player_dat {
	double x;	/**< Coordinata x    */
	double y;	/**< Coordinata y    */ 
	double ang;	/**< Angolo in gradi */
	
	double vel_camminata; /**< Valore di incremento per l'avanzamento */
	double vel_gira;  /**< Valore di incremento per la modifica dell'angolo */

	int raggio_collisione; /**< Valore che indica la distanza alla quale il 
							*   giocatore può intercettare delle collisioni 
						 	*   con i muri.
						 	*/
	double dist_m;	/**< Distanza dal muro più vicino */

	int h;      /**< Altezza del giocatore       */
	double fov; /**< Field of view del giocatore */


};

/** Struttura contenente i dati della mappa */
struct mappa_dat {
	/** matrice dinamica che contiene le informazioni sulla locazione dei muri*/
	int **data;     

	int dim;	/**< Numero blocchi per lato della mappa */
	int u;		/**< dimensione blocchi                  */
	
	char pav;	/**< Carattere pavimento */
	char soff;	/**< Carattero soffitto  */
};

/** Struttura contenente i dati di una immagine generica */
struct immagine_dat {
	ALLEGRO_BITMAP *img;	/**< Puntatore all'immagine */
	int l;	/**< Larghezza immagine */
	int h;	/**< Altezza immagine */
	
	float x;	/**< Coordinata x */
	float y;	/**< Coordinata y */
	float sizex;	/**< Larghezza attuale */
	float sizey;	/**< Altezza attuale   */
	int alpha;		/**< Trasparenza       */
	float angle;	/**< Angolo 		   */
};


/** Struttura contenente i dati di una texture */
struct texture_dat {
	/** Matrice di colori, ovvero l' immagine */
	ALLEGRO_COLOR img [dimensione_tile][dimensione_tile];
};


/** Struttura contenente i dati di un nemico */
struct nemico_dat {
	double x;		/**< Coordinata x 			*/
	double y;		/**< Coordinata y 			*/
	double ang;		/**< Angolo 	  			*/
	double dist;    /**< Distanza dal giocatore */
	int l;			/**< Larghezza				*/
	
	
	double xarrivo;	/**< Coordinata x futura 	*/
	double yarrivo; /**< Coordinata y futura 	*/
	

	bool vis;		/**< true se è stato visto        */
	bool insegui;	/**< se true insegue il giocatore */
	bool simuove;	/**< true se si sta muovendo	  */
	bool vivo;		/**< true se è vivo				  */
	
	int vita;	/**< vita attuale */
	
	int frame;	/**< Frame attuale dell'animazione */
	int t_ani;	/**< Tempo dell'animazione 		   */
};

/** Struttura contenente i dati di tutti i nemici sulla mappa */
struct nemici_dat {
	nemico_dat *nemico;   /**< Puntatore ad array di nemici */ 
	int num_nemici;		  /**< Numero nemici				*/
	int nem_vivi;		  /**< Numero nemici vivi 			*/
	texture_dat text[10]; /**< Sprite dei nemici			*/
	double vel_camminata; /**< Velocità camminata nemici    */
};

/** Struttura contenente i dati della pistola */
struct pistola_dat {
	float x; /**< Coordinata x	*/
	float y; /**< Coordinata y	*/
	
	ALLEGRO_BITMAP *img; /**< Puntatore immagine pistola */
	int img_l;			 /**< Larghezza immagine         */
	int img_h;			 /**< Altezza immagine           */
	int frame_l;		 /**< Larghezza frame            */
	

	int t;     /**< Tempo attuale dello sparo            */
	int tmax;  /**< Tempo minimo tra uno sparo e l'altro */
	int danno; /**< Danno di un colpo inflitto al nemico */
	
	ANIM_PIST stato_anim; /**< Stato attuale animazione pistola            */
	float FERMO_off;      /**< Oscillazione pistola da fermo               */
	float FERMO_vel;	  /**< Velocità oscillazione pistola da fermo      */
	float CAMMINA_off;    /**< Oscillazione pistola camminando             */
	float CAMMINA_vel;	  /**< Velocità oscillazione pistola camminando    */
	int frame;		/**< Frame attuale pistola        */
	int nframe;		/**< Numero frame animazione      */
	float t_SPARA;  /**< Tempo animazione dello sparo */
	
};

/** Struttura contenente i dati dell' audio */
struct audio_dat {
	ALLEGRO_SAMPLE_ID m_id; /**< Identificatore audio in riproduzione */ 

	ALLEGRO_SAMPLE *atmosfera; 
	ALLEGRO_SAMPLE *titolo;
	ALLEGRO_SAMPLE *vittoria;
	ALLEGRO_SAMPLE *gameover;
	ALLEGRO_SAMPLE *morte;
	ALLEGRO_SAMPLE *passo;
	ALLEGRO_SAMPLE *pistola;
	ALLEGRO_SAMPLE *zombie;
	ALLEGRO_SAMPLE *bomba;

};






