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
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "config.h"
#include "spectrum3d.h"
#include "events.h"

gboolean sdl_event(Spectrum3dGui *spectrum3dGui)
{
	//printf("sdlEvent\n");
	Uint8 mouseState, *keystate;
	SDL_Event event;
	int MouseX,MouseY;

	//newEvent = TRUE;
	
	while (SDL_PollEvent(&event)){
		switch (event.type)
			{
			case SDL_VIDEORESIZE:
				configure_SDL_gl_window(event.resize.w, event.resize.h);
				spectrum3d.width = event.resize.w; 
				spectrum3d.height = event.resize.h;
				newEvent = TRUE;
				break;
			case SDL_MOUSEMOTION:
				mouseState = SDL_GetMouseState(NULL, NULL);
				if (SDL_GetRelativeMouseState(&MouseX,&MouseY)){
					if (mouseState == 1){
						if ( MouseX < 100 && MouseX > -100 ) {
							if (MouseX > 0) {
								AngleH += 1.2;
								}
							else if (MouseX < 0) {
								AngleH-= 1.2;
								}
							}
						if (MouseY < 100 && MouseY > -100 ) {
							if (MouseY > 0) {
								AngleV += 1.6;
								}
							else if (MouseY < 0) {
								AngleV -= 1.6;
								}
							}
						}
					else if (mouseState == 4){
						if ( MouseX < 100 && MouseX > -100 ) {
							if (MouseX > 0) {
								X+=0.02;
								}
							else if (MouseX < 0) {
								X-=0.02;
								}
							}
						if (MouseY < 100 && MouseY > -100 ) {
							if (MouseY > 0) {
								Y-=0.02;
								}
							else if (MouseY < 0) {
								Y+=0.02;
								}
							}
						}
					newEvent = TRUE;
					}	
							
				break;
			case SDL_BUTTON_WHEELDOWN :
				if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)){
					if (event.button.button == SDL_BUTTON_WHEELUP){
						Z +=0.02;
						}
					if (event.button.button == SDL_BUTTON_WHEELDOWN){
						Z -=0.02;
						}
					newEvent = TRUE;
					}	
				else if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)){
					if (event.button.button == SDL_BUTTON_WHEELUP){
						AngleZ +=0.4;
						}
					if (event.button.button == SDL_BUTTON_WHEELDOWN){
						AngleZ -=0.4;
						}
					newEvent = TRUE;
					}		
				break;
			case SDL_QUIT:
				quit_spectrum3d();
				break;
			case SDL_KEYDOWN:
				keystate = SDL_GetKeyState(NULL);
				if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && (keystate[SDLK_RSHIFT] || keystate[SDLK_LSHIFT]) && keystate[SDLK_UP] ) {
					Z -= 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && (keystate[SDLK_RSHIFT] || keystate[SDLK_LSHIFT]) && keystate[SDLK_DOWN] ) {
					Z += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RSHIFT] || keystate[SDLK_LSHIFT]) && keystate[SDLK_UP] ) {
					AngleZ += 0.4;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RSHIFT] || keystate[SDLK_LSHIFT]) && keystate[SDLK_DOWN] ) {
					AngleZ -= 0.4;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && keystate[SDLK_UP] ) {
					Y += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && keystate[SDLK_DOWN] ) {
					Y -= 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && keystate[SDLK_RIGHT] ) {
					X += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && keystate[SDLK_LEFT] ) {
					X -= 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDLK_RCTRL] || keystate[SDLK_LCTRL]) && keystate[SDLK_DOWN] ) {
					//AngleZ -= 0.4;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_q] && ( keystate[SDLK_RIGHT] || keystate[SDLK_UP]) ) {
					Xpointer +=  (x / bandsNumber);
					Ypointer += (y_2d / bandsNumber);
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_q] && ( keystate[SDLK_LEFT] || keystate[SDLK_DOWN]) ) {
					Xpointer -= (x / bandsNumber);
					Ypointer -= (y_2d / bandsNumber);
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_a] && ( keystate[SDLK_RIGHT] || keystate[SDLK_UP]) ) {
					Xpointer +=  (x / bandsNumber) * 10;
					Ypointer += (y_2d / bandsNumber) * 10;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_a] && ( keystate[SDLK_LEFT] || keystate[SDLK_DOWN]) ) {
					Xpointer -= (x / bandsNumber) * 10;
					Ypointer -= (y_2d / bandsNumber) * 10;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_g] && keystate[SDLK_UP]) {
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) + 0.01) );
					}
				else if ( keystate[SDLK_g] && keystate[SDLK_DOWN]) {
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) - 0.01) );
					}
				else if ( keystate[SDLK_UP] ) {
					AngleV += 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_DOWN] ) {
					AngleV -= 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_RIGHT] ) {
					AngleH += 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_LEFT] ) {
					AngleH -= 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_SPACE] ) {
					playFromSource(NULL, NULL);
					}
				else if ( keystate[SDLK_d] ) {
					if (viewType == THREE_D){
						gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio2D));
						}
					else {
						gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3D));
						}					
					}
				else if ( keystate[SDLK_f] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3Dflat));
					}
				else if ( keystate[SDLK_t] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuText));
					}
				else if ( keystate[SDLK_l] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuLines));
					}
				else if ( keystate[SDLK_p] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuPointer));
					}
				else if ( keystate[SDLK_r] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->reset));
					}
				else if ( keystate[SDLK_o] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->front));
					}
				else if ( keystate[SDLK_ESCAPE] ) {
					on_stop();
					}		
				default:
					break;
				}
			}
	return TRUE;
}




