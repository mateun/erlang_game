#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <fcntl.h>



#define BUF 15

#define UDS_FILE "/tmp/graphsock.uds"

int main(int argc, char** args) {
int create_socket, new_socket;

	
	// All the network stuff...
	FILE* logf = fopen("./graphics_server.log", "w");
	fprintf(logf, "start entry\n");
	fflush(logf);
  socklen_t addrlen;
  char *buffer = (char*) malloc (BUF);
  ssize_t size;
  struct sockaddr_un address;
  const int y = 1;
  //printf ("\e[2J");
  if((create_socket=socket (AF_LOCAL, SOCK_STREAM, 0)) > 0)
    fprintf (logf, "Socket wurde angelegt\n");
	fflush(logf);

	
  unlink(UDS_FILE);
  address.sun_family = AF_LOCAL;
  strcpy(address.sun_path, UDS_FILE);
  if (bind ( create_socket,
             (struct sockaddr *) &address,
             sizeof (address)) != 0) {
    fprintf(logf, "Der Port ist nicht frei – belegt!\n");
  }
  listen (create_socket, 5);
  addrlen = sizeof (struct sockaddr_in);


		 printf("accepting..\n");
     new_socket = accept ( create_socket,
                           (struct sockaddr *) &address,
                           &addrlen );
     if (new_socket > 0) {
     	printf ("Ein Client ist verbunden ...\n");
     	fprintf (logf, "Ein Client ist verbunden ...\n");
			fflush(logf);
		 }

			printf("Opening render window\n");		 

			// All the graphics stuff
			if (SDL_Init(SDL_INIT_VIDEO) != 0) {
				SDL_Log("error %s", SDL_GetError());
				exit(1);
			}


			SDL_Window* window;
			SDL_Renderer* renderer;
			if (SDL_CreateWindowAndRenderer(640, 480,
				 									SDL_WINDOW_RESIZABLE, 
													&window, &renderer)) {
  			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
						"Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
			}

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

			char* outBuffer = (char*) malloc(512);	
		 	int counter = 0;

		 	fcntl(new_socket, F_SETFL, O_NONBLOCK);  // set client socket to non-blocking;
			bool shouldRun = true;
     	do {
				counter++;

				
        // check if we received a new command from the game client
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size > 0) {
        	buffer[size] = '\0';
					printf("received %l\n", size);
					printf ("Nachricht empfangen: %s\n", buffer);
        	fprintf (logf, "Nachricht empfangen: %s\n", buffer);
					fflush(logf);
					int len = sprintf(outBuffer, "c: %d %s", counter, "drawing_ok"); 
					//sleep(5);
					send(new_socket, outBuffer, len+1, 0);
					fprintf(logf, "_sent data: %s\n", outBuffer);
					fflush(logf);

				}
				
				// Poll for window event
				SDL_Event event;
				SDL_PollEvent(&event);
				// TODO should maybe retrieve all pending events here
				// with while loop
    		if (event.type == SDL_QUIT) {
					shouldRun = false;
    		}

				// Clear the screen and flip the backbuffer
    		SDL_RenderClear(renderer);
    		SDL_RenderPresent(renderer);


     } while (shouldRun);

  	close (new_socket);

  	close (create_socket);
		fclose (logf);

	  SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


		return EXIT_SUCCESS;


}
