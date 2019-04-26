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
#include <gdk/gdkkeysyms.h>

#include "config.h"

#if defined HAVE_LIBSDL2
    #include <SDL2/SDL.h>
#elif defined HAVE_LIBSDL
    #include <SDL/SDL.h>
#endif

#include "spectrum3d.h"
#include "events.h"


float previousX, previousY; 
Uint32 previousTimeStamp, previousFingerDownTimeStamp, previousFingerUpId;
GtkWidget *pScaleStart;


#ifdef HAVE_LIBSDL2 
/* Events for SDL2 */
gboolean sdl_event(Spectrum3dGui *spectrum3dGui)
{
	//printf("SDL_event\n");
	Uint8 mouseState;
	const Uint8 *keystate;
	SDL_Event event;
	int MouseX,MouseY;
	gdouble step_increment;
	gboolean multiTouch=FALSE;
	
    while (SDL_PollEvent(&event)){
	//printf("SDL_event\n");
    	switch(event.type){
        	case SDL_WINDOWEVENT: // Événement de la fenêtre
            		if ( event.window.event == SDL_WINDOWEVENT_CLOSE ){ // Fermeture de la fenêtre
            			quit_spectrum3d();
            			}
			else if ( event.window.event == SDL_WINDOWEVENT_RESIZED ){ // Fermeture de la fenêtre
            			configure_SDL_gl_window(event.window.data1, event.window.data2);
				spectrum3d.width = event.window.data1; 
				spectrum3d.height = event.window.data2;
				newEvent = TRUE;
            			}
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
			
			/*case SDL_FINGERMOTION:
				//if (event.mgesture.numFingers < 2){
          			SDL_Log("Finger: %"SDL_PRIs64",x: %f, y: %f",event.tfinger.fingerId,
               			event.tfinger.dx,event.tfinger.dy);
				//}
         			break;*/
        		case SDL_FINGERDOWN:
				//SDL_Log("Finger: %"SDL_PRIs64" down - x: %f, y: %f, timestamp = %i", event.tfinger.fingerId,event.tfinger.x,event.tfinger.y, event.tfinger.timestamp);
				if (((event.tfinger.timestamp-previousFingerDownTimeStamp) < 250) && (event.tfinger.fingerId == previousFingerUpId + 1)){
					g_idle_add ((GSourceFunc)playFromSource, NULL);
					}
// PreviousFingerUpId is used to be sure that the first finger has been released (= a dubble tap with one finger); other wise a double touch can also trigger this if the 2 fingers don't touch the screen exactly at the same time
				previousFingerDownTimeStamp = event.tfinger.timestamp;
				break;
			case SDL_FINGERUP:
				previousFingerUpId = event.tfinger.fingerId;
			        //SDL_Log("Finger: %"SDL_PRIs64" up - x: %f, y: %f",
               			//event.tfinger.fingerId,event.tfinger.x,event.tfinger.y);
				break;
			case SDL_MULTIGESTURE:
				multiTouch = TRUE;
				/*SDL_Log("Multi Gesture: x = %f, y = %f, dAng = %f, dR = %f",
				event.mgesture.x,
				event.mgesture.y,
				event.mgesture.dTheta,
				event.mgesture.dDist);*/
				//printf("timestamp = %i\n", event.mgesture.timestamp);
			    if (event.mgesture.numFingers == 2){
				Z+=(event.mgesture.dDist/200); //Pinch
				AngleZ-=(event.mgesture.dTheta*10);  //Rotate
				if ((event.mgesture.timestamp-previousTimeStamp) < 10){ //Drag
					X+=((event.mgesture.x-previousX)/300);
					Y-=((event.mgesture.y-previousY)/300);
					}
				newEvent = TRUE;
				}
			   
				/*if ((event.mgesture.timestamp-previousTimeStamp) < 100){
				    //step_increment = gtk_adjustment_get_step_increment (GTK_ADJUSTMENT(spectrum3dGui->adjustStart));
				    gtk_range_set_value (GTK_RANGE(spectrum3dGui->scaleBands),
                     //((gtk_range_get_value (GTK_RANGE(spectrum3dGui->scaleBands))) + (event.mgesture.y-previousY)));
				    ((gtk_range_get_value (GTK_RANGE(spectrum3dGui->scaleBands))) + (event.mgesture.dDist)));
				    //printf("pinch\n");
				    newEvent = TRUE;
				    }*/


				//SDL_Log("MG: numDownTouch = %i",event.mgesture.numFingers);
			    
				previousX=event.mgesture.x;
				previousY=event.mgesture.y;
				previousTimeStamp=event.mgesture.timestamp;	
				break;

			case SDL_FINGERMOTION:
				if (!multiTouch){
          			//SDL_Log("Finger: %"SDL_PRIs64",x: %f, y: %f",event.tfinger.fingerId, event.tfinger.dx,event.tfinger.dy);
				
				AngleH+=(event.tfinger.dx/10);
				AngleV+=(event.tfinger.dy/10);
				newEvent = TRUE;

				/*if ((event.tfinger.timestamp-previousTimeStamp) > 400){
				    gtk_range_set_value (GTK_RANGE(pScaleStart),                   		((gtk_range_get_value (GTK_RANGE(pScaleStart))) +((gdouble)(event.tfinger.dx))));
				printf("drag\n");
				    }*/	
				}
				multiTouch=FALSE;
         			break;

			case SDL_KEYDOWN:
				keystate = SDL_GetKeyboardState(NULL);
								
				if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && (keystate[SDL_SCANCODE_RSHIFT] || keystate[SDL_SCANCODE_LSHIFT]) && keystate[SDL_SCANCODE_UP] ) {
					Z -= 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && (keystate[SDL_SCANCODE_RSHIFT] || keystate[SDL_SCANCODE_LSHIFT]) && keystate[SDL_SCANCODE_DOWN] ) {
					Z += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RSHIFT] || keystate[SDL_SCANCODE_LSHIFT]) && keystate[SDL_SCANCODE_UP] ) {
					AngleZ += 0.4;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RSHIFT] || keystate[SDL_SCANCODE_LSHIFT]) && keystate[SDL_SCANCODE_DOWN] ) {
					AngleZ -= 0.4;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_UP] ) {
					Y += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_DOWN] ) {
					Y -= 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_RIGHT] ) {
					X += 0.02;
					newEvent = TRUE;
					}
				else if ( (keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_LEFT] ) {
					X -= 0.02;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_Q] && ( keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_UP]) ) {
					Xpointer +=  (x / bandsNumber);
					Ypointer += (y_2d / bandsNumber);
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_Q] && ( keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_DOWN]) ) {
					Xpointer -= (x / bandsNumber);
					Ypointer -= (y_2d / bandsNumber);
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_A] && ( keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_UP]) ) {
					Xpointer +=  (x / bandsNumber) * 10;
					Ypointer += (y_2d / bandsNumber) * 10;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_A] && ( keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_DOWN]) ) {
					Xpointer -= (x / bandsNumber) * 10;
					Ypointer -= (y_2d / bandsNumber) * 10;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_G] && keystate[SDL_SCANCODE_UP]) {
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) + 0.01) );
					}
				else if ( keystate[SDL_SCANCODE_G] && keystate[SDL_SCANCODE_DOWN]) {
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) - 0.01) );
					}
				else if ( keystate[SDL_SCANCODE_UP] ) {
					AngleV -= 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_DOWN] ) {
					AngleV += 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_RIGHT] ) {
					AngleH += 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDL_SCANCODE_LEFT] ) {
					AngleH -= 0.8;
					newEvent = TRUE;
					}
				
				else if ( keystate[SDL_SCANCODE_SPACE] ) {
					//gtk_button_clicked (GTK_BUTTON(playButton));
					//newEvent = TRUE;
					//playFromSource(NULL, NULL);
					g_idle_add ((GSourceFunc)playFromSource, NULL);
					}
				else if ( keystate[SDL_SCANCODE_D] ) {
					if (viewType == THREE_D){
						gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio2D));
						}
					else {
						gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3D));
						}					
					}
				else if ( keystate[SDL_SCANCODE_F] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3Dflat));
					}
				else if ( keystate[SDL_SCANCODE_T] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuText));
					}
				else if ( keystate[SDL_SCANCODE_L] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuLines));
					}
				else if ( keystate[SDL_SCANCODE_P] ) {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuPointer));
					}
				else if ( keystate[SDL_SCANCODE_R] ) {
					reset_view();
					//gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->reset));
					}
				else if ( keystate[SDL_SCANCODE_O] ) {
					front_view();
					//gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->front));
					}
				else if ( keystate[SDL_SCANCODE_ESCAPE] ) {
					on_stop();
					}		
				default:
					break;
		}
	}
	return TRUE;
}
#endif

#ifdef HAVE_LIBSDL 
/* Events for SDL1.2 */
gboolean sdl_event(Spectrum3dGui *spectrum3dGui)
{
	//printf("SDL_event\n");
	Uint8 mouseState, *keystate;
	SDL_Event event;
	int MouseX,MouseY;

	while (SDL_PollEvent(&event)){
		//printf("pollEvent\n");
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
					AngleV -= 0.8;
					newEvent = TRUE;
					}
				else if ( keystate[SDLK_DOWN] ) {
					AngleV += 0.8;
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
					g_idle_add ((GSourceFunc)playFromSource, NULL);
					//playFromSource(NULL, NULL);
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
					reset_view();
					//gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->reset));
					}
				else if ( keystate[SDLK_o] ) {
					front_view();
					//gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->front));
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
#endif





