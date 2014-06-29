
/** Dato un livello, carica tutto il necessario affinchè si possa giocare su
 *  quel livello.  
 *  Viene chiamata all'inizio di ogni livello dal ::main .
 *
 *	si appoggia alle seguenti funzioni di input_output:
 *  ::carica_mappa , ::controlla_texture_mappa , ::carica_texture_mappa e
 *  ::carica_texture_nemico .
 *
 *  inoltre inizializza anche le strutture dati necessarie a far partire 
 *  correttamente il livello, appoggiandosi alle seguenti funzioni di logica_gioco:
 *  ::iniz_player , ::iniz_pistola e ::iniz_nemici .  
 *
 *  Parametri: lv livello da caricare
 *  Parametri: le strutture da aggiornare
 */
int carica_ambiente(char lv, mappa_dat &map, texture_dat *&texture, 
					player_dat &player,pistola_dat &pistola, nemici_dat &nemici);

/** dealloca la memoria dinamica delle strutture che devono essere cambiate.
 *
 *  Parametri: tutte le strutture che contengono elementi da eliminare
 */
void dealloca_ambiente(mappa_dat &map, texture_dat *&texture, nemici_dat &nemici,
						pistola_dat &pistola );

/** Inizializza i campi della struttura del player con oppurtuni dati.
 *
 *  Parametri: pl struttura del player
 */
void iniz_player (player_dat &pl);

/** Inizializza i campi della struttura della pistola con oppurtuni dati.
 *  Ritorna falso in caso di errore nel caricamento della sprite
 *
 *  Parametri: pist struttura della pistola
 */
bool iniz_pistola (pistola_dat &pist);

/** Aggiorna la posizione del giocatore in base agli input
 *
 *  Parametri: tasto input della tastiera
 *  Parametri: tutte le strutture da aggiornare
 */
void muovi_player (player_dat &pl, mappa_dat &map, bool tasto[], audio_dat &audio);

/** Inizializza i campi della struttura dei nemici con oppurtuni dati.
 *
 *  Parametri: nem struttura dei nemici 
 *  Parametri: map struttura della mappa
 */
void iniz_nemici (nemici_dat &nem, mappa_dat &map);

/** Dato un nemico e la posizione di un punto dice se tale punto è dentro 
 *  o no al raggio di collisione del nemico
 *
 * Parametri: nem struttura dei nemici  
 * Parametri: x,y coordinate del punto
 */
bool computa_collisione(nemico_dat &nem, double x, double y);

/** Gestisce l'animazione della pistola e la possibilità di sparare in base 
 *  alle tempistiche, inoltre appoggiandosi a ::computa collisione calcola
 *  quale nemico lo sparo può aver colpito.
 *
 *  Parametri: tutte le strutture da aggiornare  
 */
void gestisci_sparo (	player_dat &pl, nemici_dat &nem, pistola_dat &pist, 
						bool tasto[], audio_dat &audio						);

/** Aggiorna la posizione dei nemici.
 *  Per calcolare la posizione futura si appoggia all'algoritmo di ::pathfinding.
 *
 *  Parametri: tutte le strutture da aggiornare
 */
void muovi_nemici(player_dat &pl, mappa_dat &map, nemici_dat &nem);

/** Gestisce l'animazione dei nemici.
 *  
 *  Parametri: tutte le strutture da aggiornare  
 */
void anima_nemici (nemici_dat &nem);

/** Controlla lo stato del gioco.
 *  Controlla se ci sono nemici vivi, e appena ne muore uno fa partire 
 *  l'animazione di esplosione. Se non ci sono più nemici vivi ritorna
 *  lo stato CARICA che fa capire al ::main che deve caricare un'altro 
 *  livello.
 *  Se invece il nemico esplode abbastanza vicino da ucciere il giocatore
 *  ritorna GAME_OVER.
 *
 *  Parametri: tutte le strutture da aggiornare
 */
STATO controlla_stato_nemici (nemici_dat &nemici, audio_dat &audio);
