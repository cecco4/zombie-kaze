/**Questo file contiene la definizione della Debug console.
 * E' una speciale interfaccia attivabile solo in modalità di debug che  
 * consente di ricavare informazioni in tempo reale su player, mappa e nemici.
 * è possibile inoltre scegliere quale livello giocare.
 *
 * Per attivare questa modilità, è necessario compilare il progetto con 
 * "make debug" e una volta in gioco premere "p". 
 */

#define DEBUG_CONSOLE                                                           \
    al_stop_timer(timer);                                                       \
    cout<<"-------- DEBUG CONSOLE --------"<<endl;                              \
    cout<<"Codice livello attuale: "<<livello<<endl;                            \
    bool menu = true;                                                           \
    while(menu) {                                                               \
    cout<<"\n>>MENU:\n";                                                        \
    cout<<"\t1) Informazioni player\n";                                         \
    cout<<"\t2) Informazioni mappa\n";                                          \
    cout<<"\t3) Informazioni nemici\n";                                         \
    cout<<"\t4) Salta al livello desiderato\n";                                 \
    cout<<"\t5) torna al gioco\n";                                              \
    cout<<"Scelta: ";                                                           \
    int scelta; cin>>scelta;                                                    \
    switch(scelta) {                                                            \
    case 1:                                                                     \
        cout<<"\nPlayer:\n";                                                    \
        cout<<"\tx "<<player.x<<"\ty "<<player.y<<endl;                         \
        cout<<"\tangolo "<<player.ang<<endl;                                    \
        cout<<"\tdistanza muro davanti "<<player.dist_m<<endl;                  \
        break;                                                                  \
    case 2:                                                                     \
        cout<<"\nMappa:\n";                                                     \
        cout<<"\tdimensione mappa "<<map.dim<<endl;                             \
        cout<<"\tdimensione unita' "<<map.u<<endl;                              \
        cout<<"\tcodice pavimento "<<map.pav<<" , soffitto "<<map.soff<<endl;   \
        break;                                                                  \
    case 3:                                                                     \
        cout<<"\nNemici:\n";                                                    \
        cout<<"\tnemici tot "<<nemici.num_nemici<<endl;                         \
        cout<<"\tnemici vivi "<<nemici.nem_vivi<<endl;                          \
        for(int i=0; i<nemici.num_nemici; i++) {                                \
            cout<<"\nNemico "<<i<<": ";                                         \
            if (nemici.nemico[i].vivo)                                          \
                cout<<"vivo\n";                                                 \
            else {                                                              \
                cout<<"morto\n";                                                \
                continue;                                                       \
            }                                                                   \
                                                                                \
            cout<<"\tvita "<<nemici.nemico[i].vita<<endl;                       \
            cout<<"\tx, y "<<nemici.nemico[i].x<<" , ";                         \
            cout<<nemici.nemico[i].y<<endl;                                     \
            cout<<"\tangolo "<<nemici.nemico[i].ang<<endl;                      \
            cout<<"\tx,y future "<<nemici.nemico[i].xarrivo<<" , ";             \
            cout<<nemici.nemico[i].yarrivo;                                     \
            cout<<"  insegui? "<<nemici.nemico[i].insegui<<endl;                \
            cout<<"frame animazione "<<nemici.nemico[i].frame<<endl;            \
        }                                                                       \
        break;                                                                  \
    case 4:                                                                     \
        cout<<"Codice livello (0-9) ? ";                                        \
        char lv_scelto;                                                         \
        cin>>lv_scelto;                                                         \
        if(lv_scelto>='0' && lv_scelto<='9')                                    \
            livello=lv_scelto;                                                  \
        else {                                                                  \
            cout<<"scelta errata\n";                                            \
            break;                                                              \
        }                                                                       \
        al_stop_sample(&audio.m_id);                                            \
        stato_gioco= CARICA;                                                    \
        dealloca_ambiente(map, texture, nemici, pistola);                       \
        caricato = false;                                                       \
        titolo.angle = sfondo.angle = 0;                                        \
        titolo.y = sfondo.y = -hschermo/2;                                      \
    case 5:                                                                     \
        tasto[P]=false;                                                         \
        menu = false;                                                           \
        al_start_timer(timer);                                                  \
        break;                                                                  \
    }                                                                           \
    }
        
        
        
