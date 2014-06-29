
/** Dato un numero lo riporta ad un intervallo fra 0 e 360
 * 
 * Parametri: ang numero
 */
double to_360 (double ang);

/** Dato un angolo in gradi lo converte in radianti
 * 
 * Parametri: ang angolo
 */
double rad (double ang);

/** Dato un angolo in radianti lo converte in gradi
 * 
 * Parametri: ang angolo
 */
double grad (double ang);


/** Funzione principale per quanto concerne la grafica del gioco.
 *  Questa funzione in base alla mappa, costruisce una visuale in falso 3d
 *  dell'ambiente circostante.
 *  Disegna i muri con una tecnica chiamata appunto raycasting, dopodichè 
 *  aggiunge i nemici appoggiandosi a ::disegna_sprite .
 * 
 *  Parametri: pl dati del giocatore
 *  Parametri: nem dati dei nemici
 *  Parametri: map dati della mappa
 *  Parametri: texture texture dei muri
 *  Parametri: render matrice sulla quale verranno applicate tutta la visuale
 *             grafica calcolata nella funzione
 */
void raycast (  player_dat &pl, nemici_dat &nem, mappa_dat &map, 
				texture_dat *texture, ALLEGRO_COLOR render[][lschermo]	);

/** Pulisce la matrice utilizzata per il rendering, preparandola quindi ad 
 *  essere plasmata da ::raycast
 *  
 *  Parametri: render_matrix matrice per il rendering
 */
void pulisci_render_matrix(ALLEGRO_COLOR render_matrix[hschermo][lschermo]);

/** Trasforma la matrice di colori in immagine che verrà poi disegnata sul 
 *  display dal ::main
 *  
 *  Parametri: render_matrix matrice per il rendering
 *  Parametri: rendering immagine finale
 */
void applica_render_matrix( ALLEGRO_COLOR render_matrix[hschermo][lschermo],
							ALLEGRO_BITMAP *rendering);


