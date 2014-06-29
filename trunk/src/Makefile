allegrolib = `pkg-config --cflags --libs allegro-5.0\
 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0\
 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0\
 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`
  
OBJ = main.o input_output.o logica_gioco.o raycast.o hud.o
CC = main.cc input_output.cc logica_gioco.cc raycast.cc hud.cc
CXXFLAGS= -Wall -std=c++0x

Zombie-kaze: $(OBJ)
	g++ -Wall -std=c++0x -o Zombie-kaze  $(OBJ) $(allegrolib)

-include dependencies

.PHONY: updatepath depend clean cleanall debug

depend:
	g++ -MM *.cc > dependencies
clean:
	rm *.o
cleanall:
	rm *.o *~ Zombie-kaze

debug: 	
	g++ -Wall -g -D DEBUG_MODE $(CC) $(allegrolib) -o Zombie-kaze
	
