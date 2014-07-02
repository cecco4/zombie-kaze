
/** Carica la mappa da un file .map in "data/mappe" in una matrice statica,
  *	ritornando il tutto in uno struct mappa_dat.
  *	il nome del file da aprire viene composto ricevendo un carattere passato
  *	alla funzione, quindi se la chiamata è:
  *	carica_mappa('4');
  *	apre il file "lv4.map", se non lo trova il puntatore alla matrice è NULL,
  *  e sarà compito di ::carica_ambiente rilevare l'errore. 
  *
  * I file .map contengono tutte le informazioni necessarie per posizionare
  * le texture nell'ambiente pseudo-3d.
  * Essi sono strutturati secondo il seguente schema:
  * prima riga: dimensione lato mappa, spazio, carattere pavimento,
  * 			carattere soffitto, newline.
  * segue poi dalla seconda riga in poi la rappresenzazione della mappa,
  * ovvero una sequenza di caratteri a ognuno dei quali corrisponde una 
  * texture, fatta eccezione per i seguenti caratteri:
  * 0: nessuna texture
  * P: posizione iniziale del player
  * N: posizione di un nemico
  * 
  * regole: 
  * - Le mappe devono essere quadrate con dimensione del lato uguale al 
  * 	numero scritto nella prima riga.
  * - Le texture possono avere tutti i caratteri possibili a parte 0, P, N.
  * - Lasciare dei "buchi" verso la fine della mappa potrebbe creare effetti
  * 	indesiderati.
  * - I livelli vanno in ordine da 0 a 9.
  * - I livelli devono contenere solo una P e almeno una N.
  * 
  * ESEMPIO lv3.map:
  * 5 2E
  * 11111
  * 2P00r
  * r0001
  * e00Nb
  * bbb12
  * 
  * 
  * Parametri: n_mappa nome mappa da caricare
  */
mappa_dat carica_mappa(char n_mappa);

/** Setta i bool nel array tasto (inizializzato nel ::main), in base al tasto 
 *  premuto o rilasciato.    
 *
 *  Parametri: ev evento Allegro
 *  Parametri: tasto array contenente lo stato dei tasti premuti e non
 *  Parametri: set valore da attribuire al campo dell'array 
 */  
void aggiorna_tasti(ALLEGRO_EVENT &ev, bool tasto[], bool set);

/** controlla se ci sono tutte le texture della mappa, provando 
  *	ad accedere ai file. 
  *	se manca una texture ritorna -1, se ci sono tutte ritorna il numero di 
  *	texture.  
  * Inoltre , per riferimento, ritorna anche un array in cui sono salvati i 
  * codici delle relative texture da disegnare nella matrice della mappa.
  * Inserisce poi nella matrice della mappa un numero temporaneo nella 
  * posizione in cui andranno messi i nemici, tale numero varrà poi letto 
  *	ed eliminato da ::iniz_nemici
  *
  * Parametri: map struttura dati mappa
  * Parametri: text_list lista texture presenti sulla mappa
  * Parametri: nem struttura dati nemici
  */	
int controlla_texture_mappa(mappa_dat &map, int *text_list, nemici_dat &nem);

/** Carica le texture necessarie al livello su un array di texture_dat 
 *  ritorna true se tutto è andato bene, false se non è riuscito a caricare
 *
 * Parametri: texture array di texture
 * Parametri: lista texture da caricare
 * Parametri: nem struttura dati nemici
 */
bool carica_texture_mappa(texture_dat texture[], int *text_list, int num_tex);

/** Carica le texture del nemico su un array di texture dat
 *  ritorna true se tutto è andato bene, false se non è riuscito a caricare.
 *
 * Parametri: texture array di texture
 */
bool carica_texture_nemico(texture_dat text[]);

/** carica un immagine generica e la salva su un immagine_dat  
 *  ritorna la struttura con l'immagine salvata.
 *  se il campo img della struttura ritornata è NULL c'è stato un errore nel
 *  caricamento.
 *
 * Parametri: img struttura su cui salvare
 * Parametri: path percorso immagine da caricare
 */
immagine_dat carica_immagine ( immagine_dat img, const char *path);

/** Carica l'audio del gioco, ritornando true se è andato tutto bene,
 *  false in caso contrario.
 *
 * Parametri: audio struttura audio_dat su cui salvare
 */
bool carica_audio(audio_dat &audio);
