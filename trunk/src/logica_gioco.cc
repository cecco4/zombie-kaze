//C++ header
#include <iostream>
#include <math.h>

//allegro header
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>    
#include <allegro5/allegro_image.h>

//header interni
#include "struttura_dati.h"
#include "logica_gioco.h"
#include "raycast.h"
#include "input_output.h"

using namespace std;

/** Struttura contenente i dati dei nodi utilizzati nel ::pathfinding */
struct nodo {
    int x;
    int y;
    nodo *succ;
    nodo *prec;
};

/** Valori utilizzati dall'algoritmo di ::pathfinding per costruire il percorso
 *  del nemico, indicano la posizione in cui si deve spostare
 */
enum path_t { p, boh , su , giu, destra, sinistra, muro };


//Funzioni interne
 
/** Implementa l'algoritmo BFS per il pathfinding dei nemici */
static path_t pathfinding
    (mappa_dat &map, nemici_dat &nem, int xp, int yp, int xa, int ya);
/** Prende il primo elemento in coda */
static void eject(nodo * &testa, nodo* &coda, int &x, int &y);
/** inserisce un elemento in coda */
static void inject(nodo * &testa, nodo* &coda, int x, int y);

/** Setta le coordinate futura alle quali il nemico si dovrà spostare */
static void setta_posizione_arrivo(nemico_dat &nem, path_t dir);



int carica_ambiente(char lv, mappa_dat &map, texture_dat *&texture, 
                    player_dat &player,pistola_dat &pistola, nemici_dat &nemici)
{
    cout<<"CARICAMENTO LIVELLO:    "<<lv<<endl; 
    //carico il necessario
    map = carica_mappa(lv);  //mappa primo lv
    map.u = dimensione_tile;
    if (map.data == NULL) {
        cout<<"errore caricamento della mappa\n";
        return -5;
    }    
    
    bool cerca_player = false;
    //setto posizione player
    for(int i=0; i<map.dim ; i++)
        for(int j=0; j<map.dim ; j++) {
            if (map.data[i][j] == int('P')) {
                map.data[i][j] = int('0');
                cerca_player = true;
                player.x = j*map.u+map.u/2;
                player.y = i*map.u+map.u/2;
            }
        }
    if (!cerca_player) {
        cout<<"Non è stata settata la posizione iniziale del player\n";
        return -6;
    }
    //contenitore nemici
 
    nemici.num_nemici = 0;
    
    int text_list[256];
    int num_tex = controlla_texture_mappa(map, text_list,nemici);
    if (num_tex == -1) 
        return -7;
    
    texture = new texture_dat[num_tex];
    if (!carica_texture_mappa(texture, text_list, num_tex))
        return -8;

    //array nemici
    nemici.nemico = new nemico_dat[nemici.num_nemici];
    
    if (lv == '0') {
        //carico texture nemico
        if (!carica_texture_nemico(nemici.text))
            return -9; 
        
        if (!iniz_pistola(pistola))
            return -10;
    }
    
    
    //inizializzo il player e i nemici
    iniz_player(player);
    iniz_nemici(nemici, map);
    
        
    return 0;
}



void dealloca_ambiente(mappa_dat &map, texture_dat *&texture, nemici_dat &nemici,
                        pistola_dat &pistola )
{
    for (int i=0; i<map.dim; i++) 
            delete [] map.data[i];
    delete [] map.data;
    
    delete [] texture;
    delete [] nemici.nemico;
}



void iniz_player (player_dat &pl) 
{
    pl.h = dimensione_tile/2;
    pl.ang = 0;
    pl.fov = FOV;
    pl.vel_camminata = 4;
    pl.vel_gira = 2;

    pl.raggio_collisione = 30;
    pl.dist_m = 100;

}


bool iniz_pistola (pistola_dat &pist) 
{
    pist.x = lschermo/2 - pist.frame_l/2 - 18;
    pist.y = hschermo - pist.img_h;
    
    pist.t=0;
    pist.tmax = 35; 
    pist.danno = 80;
    
    cout<<"Caricamento: data/sprite/pistola.png    ";
    ALLEGRO_BITMAP *img = al_load_bitmap("data/sprite/pistola.png");
    if (img == NULL) {
        cout<<"errore\n";
        return false;
    } else
        cout<<"OK\n";
        
    pist.img = img;
    pist.img_l = 400;
    pist.img_h = 105;
    pist.frame_l = 80;
    pist.nframe = 5;
    
    pist.stato_anim = CAMMINA;
    pist.FERMO_off =2;
    pist.FERMO_vel =0.05;    
    pist.CAMMINA_off =20;
    pist.CAMMINA_vel = 1;
    pist.t_SPARA = 15;
    

    return true;
}



void muovi_player (player_dat &pl, mappa_dat &map, bool tasto[], audio_dat &audio)
{
    static int tmp_passo =0;
    tmp_passo++;

    //cammina avanti
    double newx = pl.x + cos(rad(pl.ang))*pl.raggio_collisione;
    double newy = pl.y - sin(rad(pl.ang))*pl.raggio_collisione;

    double xx = pl.x + pl.vel_camminata* cos(rad(pl.ang))*tasto[W];
    double yy = pl.y - pl.vel_camminata* sin(rad(pl.ang))*tasto[W];    



    if (map.data[int(pl.y/map.u)][int(newx/map.u)] == 0)
        pl.x = xx;
    if (map.data[int(newy/map.u)][int(pl.x/map.u)] == 0)
        pl.y = yy;


    //cammina indietro
    newx = pl.x - cos(rad(pl.ang))*pl.raggio_collisione;
    newy = pl.y + sin(rad(pl.ang))*pl.raggio_collisione;

    xx = pl.x - pl.vel_camminata* cos(rad(pl.ang))*tasto[S];
    yy = pl.y + pl.vel_camminata* sin(rad(pl.ang))*tasto[S];

    if (map.data[int(pl.y/map.u)][int(newx/map.u)] == 0)
        pl.x = xx;
    if (map.data[int(newy/map.u)][int(pl.x/map.u)] == 0)
        pl.y = yy;

    assert(    pl.x > 0 && pl.x < map.dim*map.u && 
            pl.y > 0 && pl.y < map.dim*map.u    );


    //cammina sinistra
    newx = pl.x + cos(rad(pl.ang+90))*pl.raggio_collisione;
    newy = pl.y - sin(rad(pl.ang+90))*pl.raggio_collisione;

    xx = pl.x + pl.vel_camminata* cos(rad(pl.ang+90))*tasto[A];
    yy = pl.y - pl.vel_camminata* sin(rad(pl.ang+90))*tasto[A];    



    if (map.data[int(pl.y/map.u)][int(newx/map.u)] == 0)
        pl.x = xx;
    if (map.data[int(newy/map.u)][int(pl.x/map.u)] == 0)
        pl.y = yy;


    //cammina destra
    newx = pl.x + cos(rad(pl.ang-90))*pl.raggio_collisione;
    newy = pl.y - sin(rad(pl.ang-90))*pl.raggio_collisione;

    xx = pl.x + pl.vel_camminata* cos(rad(pl.ang-90))*tasto[D];
    yy = pl.y - pl.vel_camminata* sin(rad(pl.ang-90))*tasto[D];    



    if (map.data[int(pl.y/map.u)][int(newx/map.u)] == 0)
        pl.x = xx;
    if (map.data[int(newy/map.u)][int(pl.x/map.u)] == 0)
        pl.y = yy;

    //si gira
    pl.ang += tasto[SX] * pl.vel_gira;
    pl.ang -= tasto[DX] * pl.vel_gira;
    pl.ang = to_360(pl.ang);
    
    assert(pl.ang >= 0 && pl.ang <= 360);
    
    //suono passi
    if (tmp_passo > 20 && (tasto[W] ^ tasto[S]) ) {
        al_play_sample(audio.passo, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        tmp_passo = 0;
    }
    assert(tmp_passo>= 0 );    
}



void gestisci_sparo (    player_dat &pl, nemici_dat &nem, pistola_dat &pist, 
                        bool tasto[], audio_dat &audio                        )
{
    //tempistiche pistola
    pist.t<pist.tmax ? pist.t++ : pist.t;

    if (tasto[SPAZIO] && pist.t >= pist.tmax) {
        pist.stato_anim = SPARA;
        
        al_play_sample(audio.pistola, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        
        //distanze nemici in collisione
        double distanze[nem.num_nemici];
        for (int i=0; i<nem.num_nemici; i++)
            distanze[i] = pl.dist_m*2; //distanza maggiore
        
        //calcolo collisioni
        for (int n=0; n < nem.num_nemici; n++) {
            if(nem.nemico[n].vis) {            
                double sparo_x = pl.x + cos(rad(pl.ang))*nem.nemico[n].dist;    
                double sparo_y = pl.y - sin(rad(pl.ang))*nem.nemico[n].dist;    
                
                if ( computa_collisione(nem.nemico[n],sparo_x,sparo_y) )     
                        distanze[n] = nem.nemico[n].dist;
                            
            }
        }
        
        
        //distanza minima
        int min = 0;
        for (int i=1; i<nem.num_nemici; i++) {
            if (distanze[i] < distanze[min])
                min = i;
        }
    
        //se non c'è il muro davanti
        if (distanze[min] < pl.dist_m) {
            nem.nemico[min].vita -= pist.danno;    
            al_play_sample(audio.zombie, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    
        
        pist.t=0;
    }
    
    assert(pist.t>=0 && pist.t<= pist.tmax);
    
    //animazione pistola
    if (pist.stato_anim != SPARA) {
        if (tasto[W] ^ tasto[S])
            pist.stato_anim = CAMMINA;
        else
            pist.stato_anim = FERMO;
    }
}



void iniz_nemici (nemici_dat &nem, mappa_dat &map)
{
    nem.nem_vivi = nem.num_nemici;
    int n=0;
    for(int i=0; i<map.dim ; i++)
        for(int j=0; j<map.dim ; j++) {
            if (map.data[i][j] == int('N')) {
            
                nem.nemico[n].x = j*map.u + map.u/2;
                nem.nemico[n].y = i*map.u + map.u/2;
                nem.nemico[n].l = map.u/2;
                nem.nemico[n].ang = 0;
                nem.nemico[n].vita = 100;
                nem.nemico[n].frame = 0;
                nem.nemico[n].t_ani = 0;
                nem.nemico[n].dist = 200;
                nem.nemico[n].vis = false;
                nem.nemico[n].simuove = false;
                nem.nemico[n].vivo = true;
                nem.nemico[n].insegui = false;
                
                map.data[i][j] = 0;   //ripristino mappa
                    
                n++;
            }
    }
    
    nem.vel_camminata = 2;

}


bool computa_collisione(nemico_dat &nem, double x, double y) 
{
    //limiti
    int sx = nem.x - nem.l;
    int dx = nem.x + nem.l;
    int alto = nem.y - nem.l;
    int basso = nem.y + nem.l;
    
    //se è dentro i limiti c'è la collisione
    if ( x >= sx && x<= dx && y<= basso && y >= alto )
        return true;
        
    return false;
}



void muovi_nemici(player_dat &pl, mappa_dat &map, nemici_dat &nem)
{

    for (int i=0; i < nem.num_nemici; i++) {
        if (nem.nemico[i].vita <=0 || !nem.nemico[i].insegui)
            continue;
            
        if(!nem.nemico[i].simuove) {
            //se ha finito il movimento precedente, calcola il prossimo 
            int xnem = nem.nemico[i].x/map.u;
            int ynem = nem.nemico[i].y/map.u;
            int xpl = pl.x/map.u;
            int ypl = pl.y/map.u;
            path_t dir = pathfinding(map, nem, xpl, ypl, xnem, ynem); 

            setta_posizione_arrivo(nem.nemico[i], dir);
            nem.nemico[i].simuove = true;
        } else {
            //muovi alla posizione successiva
            if (nem.nemico[i].x < nem.nemico[i].xarrivo)
                nem.nemico[i].x += nem.vel_camminata;
            else if (nem.nemico[i].x > nem.nemico[i].xarrivo)
                nem.nemico[i].x -= nem.vel_camminata;        
        
            if (nem.nemico[i].y < nem.nemico[i].yarrivo)
                nem.nemico[i].y += nem.vel_camminata;
            else if (nem.nemico[i].y > nem.nemico[i].yarrivo)
                nem.nemico[i].y -= nem.vel_camminata;    
                
            double epsy = 0.01;
            if (nem.nemico[i].x > nem.nemico[i].xarrivo-epsy &&
                nem.nemico[i].x < nem.nemico[i].xarrivo+epsy &&
                nem.nemico[i].y < nem.nemico[i].yarrivo+epsy &&
                nem.nemico[i].y > nem.nemico[i].yarrivo-epsy    )
                //se ha raggiunto la posizione è fermo e richiede istruzioni
                nem.nemico[i].simuove = false;
        }
        
        assert(    nem.nemico[i].x >0 && nem.nemico[i].x <map.dim*map.u &&
                nem.nemico[i].y >0 && nem.nemico[i].y <map.dim*map.u );
        
    }

    

}

STATO controlla_stato_nemici (nemici_dat &nemici, audio_dat &audio) 
{
    for (int n=0; n < nemici.num_nemici; n++) {
        if(nemici.nemico[n].vivo && nemici.nemico[n].vita <= 0) {
            nemici.nemico[n].vivo = false;            
            nemici.nemico[n].t_ani = 0;
            nemici.nemico[n].frame = 4; //inizio fase esplosione
            al_play_sample(audio.bomba, 2.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
        
        //controllo distanza, se espode vicino al player è GAME OVER
        if(nemici.nemico[n].dist<dimensione_tile*3/2) {
             nemici.nemico[n].vita = 0;
             //esplosione finita
             if ( nemici.nemico[n].frame >= 9) {
                 al_stop_sample(&audio.m_id);
                 al_play_sample(audio.gameover, 2.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                 return GAME_OVER;    
             }
         }
    }

    
    if (nemici.nem_vivi<=0) 
        return CARICA;    //avanza al prossimo livello

    return PLAY;

}

static void setta_posizione_arrivo(nemico_dat &nem, path_t dir)
{
    switch (dir) {
        case su:
            nem.xarrivo = nem.x;
            nem.yarrivo = nem.y - dimensione_tile;    
            break;
        case giu:
            nem.xarrivo = nem.x;
            nem.yarrivo = nem.y + dimensione_tile;        
            break;
        case destra:
            nem.xarrivo = nem.x + dimensione_tile;
            nem.yarrivo = nem.y;
            break;
        case sinistra:
            nem.xarrivo = nem.x - dimensione_tile;
            nem.yarrivo = nem.y;
            break;
        default:
            nem.xarrivo = nem.x;
            nem.yarrivo = nem.y;
    }
}


static void eject(nodo * &testa, nodo* &coda, int &x, int &y)
{
    x = testa->x;
    y = testa->y;

    nodo *tmp = testa->succ;
    delete [] testa;
    if (coda == testa)
        coda = testa = tmp;
    else
        testa = tmp;
}

static void inject(nodo * &testa, nodo* &coda, int x, int y)
{
    nodo *n = new nodo;

    n->x = x;
    n->y = y;
    if (testa != NULL) {
        n->prec = coda;
        n->succ = NULL;
        coda->succ = n;
        coda = n;
    } else {
        testa = coda = n;        
        n->prec = NULL;
        n->succ = NULL;    
    }
    


}



static path_t pathfinding
    (mappa_dat &map, nemici_dat &nem, int xp, int yp, int xa, int ya)
{
    path_t path[map.dim][map.dim];

    //setto la matrice iniziale 
    for (int i=0; i<map.dim; i++) 
        for (int j=0; j<map.dim; j++)    
            if (map.data[i][j] == 0)
                path[i][j] = boh;  //non so che direzione prendere
            else
                path[i][j] = muro; //non posso passare di qui


    for (int i=0; i < nem.num_nemici; i++) {
        int y = int(nem.nemico[i].y/map.u);
        int x = int(nem.nemico[i].x/map.u);
        //gli altri nemici non sono oltrepassabili
        if (x != xa || y != ya)
            path[y][x] = muro;  
    }
    
    //Algoritmo BFS:
    
    nodo *testa = NULL;
    nodo *coda = NULL;

    inject(testa, coda, xp, yp);
    
    path[xp][yp] = p;    //partenza
    path[ya][xa] = boh; //arrivo

    
    while(testa != NULL) {
             
        int x, y;
        eject(testa, coda, x, y);
        
        if (x==xa && y==ya){
            break;    
        }
                
        if (path[y][x+1] == boh ) {
            path[y][x+1] = sinistra;
            inject(testa, coda, x+1, y);
        }
        
        if (path[y][x-1] == boh ) {
            path[y][x-1] = destra;
            inject(testa, coda, x-1, y);
        }
        
        if (path[y-1][x] == boh ) {
            path[y-1][x] = giu;
            inject(testa, coda, x, y-1);
        }

        if (path[y+1][x] == boh ) {
            path[y+1][x] = su;
            inject(testa, coda, x, y+1);
        }
        
    
    }



    path[yp][xp] = muro; //impedisce al nemico di attraversarti

    //a questo punto path è una matrice in cui ogni casella punta alla 
    //casella successiva in cui il nemico deve andare
    
    path_t dir = path[ya][xa]; 

    assert(xa>=0 && xa<map.dim && ya >= 0 && ya<map.dim); 

    //controllo la nuova posizione
    int x=xa,y=ya;
    switch(dir) {
        case su:    y--;        break;
        case giu:    y++;        break;
        case destra:    x++;    break;
        case sinistra:    x--;    break;
        default: ;
    }
        
    if (path[y][x] == muro)
        return boh;  //non ci sono possibilità di movimento
    else
        return dir;
    
}




void anima_nemici (nemici_dat &nem) 
{

    for (int i=0; i < nem.num_nemici; i++) {
        nem.nemico[i].t_ani++;
        
        if (nem.nemico[i].frame >= 4) {
            
            if (nem.nemico[i].t_ani >= 5) {
                if (nem.nemico[i].frame<9) {
                    nem.nemico[i].frame++;
                    nem.nemico[i].t_ani=0;
                } else {
                    //elimino e posiziono il nemico fuori dalla mappa
                    nem.nemico[i].x = 0;
                    nem.nemico[i].y = 0;
                    nem.nemico[i].frame = 0;
                    nem.nemico[i].t_ani=0;
                    nem.nem_vivi--;  // aggiorno numero nemici vivi
                }
            }
            continue;
        }
        
        if (nem.nemico[i].t_ani >= 20) {
            if (nem.nemico[i].simuove) {
                nem.nemico[i].frame < 3? 
                    nem.nemico[i].frame++ :nem.nemico[i].frame =0;
                nem.nemico[i].t_ani = 0;
            } else {
                nem.nemico[i].frame = 0;
                nem.nemico[i].t_ani = 0;
            }
        }
        
        assert(nem.nemico[i].frame>=0 && nem.nemico[i].frame<9);
    }

    
}

