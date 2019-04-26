/* This file is part of Spectrum3D.

    Spectrum3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Spectrum3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Spectrum3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "config.h"

#include "events.h"

int counter;

/* Detect keyboard and mouse events in the SDL window */
gboolean sdlEvent()
{
	Uint8* keys;
	SDL_Event event;
	int MouseX,MouseY;
	//int result = 0;
 	
/*#ifdef HAVE_LIBUTOUCH_GEIS
if (enableTouch){
	result = geisGesture();	
	}		
#endif*/
	
	/* Keyboard events */
	keys = SDL_GetKeyState(NULL);
		if ( keys [SDLK_ESCAPE] ) {
			onStop();
			}
		else if ( keys[SDLK_s] && keys[SDLK_UP] ) {
			if (zoom < 2500){
				zoom+=5;
				}
			}
		else if ( keys[SDLK_s] && keys[SDLK_DOWN] ) {
			if (zoom > 0){
				zoom-=5;
				}
			}
		else if ( keys[SDLK_x] && keys[SDLK_RIGHT] ) {
			X+=0.004;
			}
		else if ( keys[SDLK_x] && keys[SDLK_LEFT] ) {
			X-=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_UP] ) {
			Y+=0.004;
			}
		else if ( keys[SDLK_y] && keys[SDLK_DOWN] ) {
			Y-=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_UP] ) {
			Z+=0.004;
			}
		else if ( keys[SDLK_z] && keys[SDLK_DOWN] ) {
			Z-=0.004;
			}
		else if ( keys[SDLK_c] && keys[SDLK_UP] ) {
			AngleZ+=0.4;
			}
		else if ( keys[SDLK_c] && keys[SDLK_DOWN] ) {
			AngleZ-=0.4;
			}
		else if ( keys[SDLK_g] && keys[SDLK_UP] ) {
			if (gain < 2){
				gain+=0.001;
				}
			}
		else if ( keys[SDLK_g] && keys[SDLK_DOWN] ) {
			if (gain > 0){
				gain-=0.001;
				}
			}
		else if ( (keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT]) && (keys[SDLK_LCTRL] || keys[SDLK_RCTRL]) && keys[SDLK_RIGHT] ) {
			YscaleX +=  (x / bandsNumber) * 10;
			flatViewY += (flatViewHeight / bandsNumber) * 10;
			changeViewParameter = TRUE;
			}
		else if ( (keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT]) && (keys[SDLK_LCTRL] || keys[SDLK_RCTRL]) && keys[SDLK_LEFT] ) {
			YscaleX -= (x / bandsNumber) * 10;
			flatViewY -= (flatViewHeight / bandsNumber) * 10;
			changeViewParameter = TRUE;
			}
		else if ( (keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT]) && keys[SDLK_RIGHT] ) {
			YscaleX += x / bandsNumber;
			flatViewY += flatViewHeight / bandsNumber;
			changeViewParameter = TRUE;
			}
		else if ( (keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT]) && keys[SDLK_LEFT] ) {
			YscaleX -= x / bandsNumber;
			flatViewY -= flatViewHeight / bandsNumber;
			changeViewParameter = TRUE;
			}
		/*else if ( keys[SDLK_UP] ) {
			AngleV+=0.4;
			}
		else if ( keys[SDLK_DOWN] ) {
			AngleV-=0.4;
			}*/
		else if ( keys[SDLK_RIGHT] && !keys[SDLK_v] ) {
			AngleH+=0.4;
			}
		else if ( keys[SDLK_LEFT] ) {
			AngleH-=0.4;
			}
		else if ( keys[SDLK_SPACE] ) {
			playFromSource();
			}
		else if ( keys[SDLK_p] ) {
			Z-=0.004;
			}
		
if ( keys[SDLK_c] || keys[SDLK_g] || keys[SDLK_s] || keys[SDLK_x] || keys[SDLK_y] || keys[SDLK_z] || keys[SDLK_UP] || keys[SDLK_DOWN] || keys[SDLK_LEFT] || keys[SDLK_RIGHT] || keys[SDLK_SPACE] ) {
			change = 1;
			}
		
	/* Mouse and combined keyboard and mouse events */
	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) {
		change = 1;
		SDL_GetRelativeMouseState(&MouseX,&MouseY);
		if ( keys[SDLK_x] && (MouseX < 100 && MouseX > -100) ) {
			if (MouseX > 0) {
				X+=0.02;
				}
			else if (MouseX < 0) {
				X-=0.02;
				}
			}
		else if ( keys[SDLK_y] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Y-=0.02;
				}
			else if (MouseY < 0) {
				Y+=0.02;
				}
			}
		else if ( keys[SDLK_z] && (MouseY < 100 && MouseY > -100) ) {
			if (MouseY > 0) {
				Z-=0.02;
				}
			else if (MouseY < 0) {
				Z+=0.02;
				}
			}
		else if ( keys[SDLK_c] && ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10))) {
			AngleZ-=MouseY/10;
			}
		else if ((MouseX < 10 && MouseX > -10) || (MouseY < 10 && MouseY > -10)) {
			AngleH+=MouseX/10;
			AngleV+=MouseY/10;
			}
		}

	SDL_PollEvent(&event);
		switch (event.type)
			{
			case SDL_QUIT:
				onStop();
			break;
			case SDL_KEYDOWN:
			    switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					onStop();
					break;
				case SDLK_UP :
					AngleV+=0.4;
					break;
				case SDLK_DOWN && SDLK_t:
					AngleV-=0.4;
					break;
				default:
				break;

				}
			default:
			break;
			}

//return result;
return TRUE;
}



