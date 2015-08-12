#include "FPS_counter.h"
#include "stdio.h"


void FPS_counter::init( double time)
{
	double lastTime = time;
	int nbFrames = 0;
}

#ifdef __GLFW__
/**@brief Funkcja, którą należy wywołać w każdym obiegu pętli renderingu.

Funkcja sama pobiera czas z systemu.
Ponieważ czas jest pobierany za pomocą biblioteki GLFW, musi zostać
ona użytai należy ustawić makro __GLFW__.*/
void FPS_counter::count( )
{
	double currentTime = glfwGetTime( );
	count( currentTime );
}
#endif

/**@brief Funkcja, którą należy wywołać w każdym obiegu pętli renderingu.

@param[in] time Aktualny czas pobrany z systemu.*/
void FPS_counter::count( double time )
{
	nbFrames++;
	if ( time - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
		// printf and reset timer
		frame_time = 1000.0 / double( nbFrames );
		nbFrames = 0;
		lastTime += 1.0;
		printf( "%E\n", frame_time );
	}
}