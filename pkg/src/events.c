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

#ifdef HAVE_LIBSDL
    #include <SDL/SDL.h>
#endif

#include "spectrum3d.h"
#include "events.h"

gboolean a_keyPressed = FALSE, g_keyPressed = FALSE, q_keyPressed = FALSE, shift_keyPressed = FALSE, control_keyPressed = FALSE;
int counter;
gdouble oldXPos = 0, oldYPos = 0;

gboolean on_mouse_scroll (GtkWidget *window, GdkEventScroll *event, gpointer data){
	//printf("state = %d\n", event->state);
	if (event->type == GDK_SCROLL){
		if (event->state == GDK_BUTTON1_MASK){
			//printf("mouse motion\n");
			if (event->direction == GDK_SCROLL_UP){
				AngleZ+=0.4;
				}
			if (event->direction == GDK_SCROLL_DOWN){
				AngleZ-=0.4;
				}
		newEvent = TRUE;
			}
	  	else if (event->state == GDK_BUTTON3_MASK){
			//printf("mouse motion\n");
			if (event->direction == GDK_SCROLL_UP){
				Z += 0.02;
				}
			if (event->direction == GDK_SCROLL_DOWN){
				Z -= 0.02;
				}
		newEvent = TRUE;
			}
		}
	return FALSE;
}


gboolean on_mouse_motion (GtkWidget *window, GdkEventMotion *event, gpointer data){
	//printf("state = %d\n", event->state);
	if (event->type == GDK_MOTION_NOTIFY){
	  	if (event->state == GDK_BUTTON1_MASK){
			//printf("mouse motion\n");
			if (event->x > oldXPos){
				AngleH+=0.4;
				}
			if (event->x < oldXPos){
				AngleH-=0.4;
				}
			oldXPos = event->x;
			if (event->y > oldYPos){
				AngleV+=0.4;
				}
			if (event->y < oldYPos){
				AngleV-=0.4;
				}
			oldYPos = event->y;
		newEvent = TRUE;
			}
		else if (event->state == GDK_BUTTON3_MASK){
			//printf("mouse motion\n");
			if (event->x > oldXPos){
				X+=0.004;
				}
			if (event->x < oldXPos){
				X-=0.004;
				}
			oldXPos = event->x;
			if (event->y > oldYPos){
				Y-=0.006;
				}
			if (event->y < oldYPos){
				Y+=0.006;
				}
			oldYPos = event->y;
		newEvent = TRUE;
			}
		}
	return FALSE;
}

/* Manage key press events with GDK */  
gboolean on_key_press (GtkWidget * window, GdkEventKey*	event, Spectrum3dGui *spectrum3dGui){
	//printf("shift_keyPressed = %d, control_keyPressed = %d\n", shift_keyPressed, control_keyPressed);
	if (event->type == GDK_KEY_PRESS){
		  switch (event->keyval){
			case GDK_KEY_Escape :
				on_stop ();
				break;
			case GDK_KEY_Left :
				if (event->state == GDK_CONTROL_MASK){
					X -= 0.02;
					}
				else if (a_keyPressed){
					Xpointer -=  (x / bandsNumber) * 10;
					Ypointer -= (y_2d / bandsNumber) * 10;
					}
				else if (q_keyPressed){
					Xpointer -=  (x / bandsNumber);
					Ypointer -= (y_2d / bandsNumber);
					}
				else {
					AngleH-=0.4;
					}
				break;
			case GDK_KEY_Right :
				if (event->state == GDK_CONTROL_MASK){
					X += 0.02;
					}
				else if (a_keyPressed){
					Xpointer +=  (x / bandsNumber) * 10;
					Ypointer += (y_2d / bandsNumber) * 10;
					}
				else if (q_keyPressed){
					Xpointer +=  (x / bandsNumber);
					Ypointer += (y_2d / bandsNumber);
					}
				else {
					AngleH+=0.4;
					}
				break;
			case GDK_KEY_Up :
				if (control_keyPressed && shift_keyPressed){
					printf("ctrl + shift\n");
					Z -= 0.02;
					}
				else if (event->state == GDK_CONTROL_MASK){
					Y += 0.02;
					}
				else if (event->state == GDK_SHIFT_MASK){
					AngleZ += 0.4;
					}
				else if (a_keyPressed){
					Xpointer +=  (x / bandsNumber) * 10;
					Ypointer += (y_2d / bandsNumber) * 10;
					}
				else if (q_keyPressed){
					Xpointer +=  (x / bandsNumber);
					Ypointer += (y_2d / bandsNumber);
					}
				else if (g_keyPressed){
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) + 0.01) );
					}
				else {
					AngleV+=0.4;
					}
				break;
			case GDK_KEY_Down :
				if (control_keyPressed && shift_keyPressed){
					Z += 0.02;
					}
				else if (event->state == GDK_CONTROL_MASK){
					Y -= 0.02;
					}
				else if (event->state == GDK_SHIFT_MASK){
					AngleZ -= 0.4;
					}
				else if (a_keyPressed){
					Xpointer -=  (x / bandsNumber) * 10;
					Ypointer -= (y_2d / bandsNumber) * 10;
					}
				else if (q_keyPressed){
					Xpointer -=  (x / bandsNumber);
					Ypointer -= (y_2d / bandsNumber);
					}
				else if (g_keyPressed){
					gtk_scale_button_set_value( GTK_SCALE_BUTTON(spectrum3dGui->scaleGain), (gtk_scale_button_get_value(GTK_SCALE_BUTTON(spectrum3dGui->scaleGain)) - 0.01) );
					}
				else {
					AngleV-=0.4;
					}
				break;
			case GDK_KEY_space :
				playFromSource(NULL, NULL);
				break;
			case GDK_KEY_t :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuText));
				break;
			case GDK_KEY_l :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuLines));
				break;
			case GDK_KEY_p :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->checkMenuPointer));
				break;
			case GDK_KEY_r :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->reset));
				break;
			case GDK_KEY_o :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->front));
				break;
			case GDK_KEY_d :
				if (viewType == THREE_D){
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio2D));
					}
				else {
					gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3D));
					}
				break;
			case GDK_KEY_f :
				gtk_menu_item_activate(GTK_MENU_ITEM(spectrum3dGui->radio3Dflat));					
				break;
			case GDK_KEY_a :
				a_keyPressed = TRUE;
				break;
			case GDK_KEY_q :
				q_keyPressed = TRUE;
				break;
			case GDK_KEY_g :
				g_keyPressed = TRUE;
				break;
			case GDK_KEY_Shift_L :
				shift_keyPressed = TRUE;
				break;
			case GDK_KEY_Shift_R :
				shift_keyPressed = TRUE;
				break;
			case GDK_KEY_Control_L :
				control_keyPressed = TRUE;
				break;
			case GDK_KEY_Control_R :
				control_keyPressed = TRUE;
				break;
		}
	newEvent = TRUE;
	}

	return TRUE;  // return TRUE instead of FALSE allows default key bindings not to be aplied to the GtkScale widgets used in the GUI
}

/* Manage key release events with GDK; this is used for key combinations */ 
gboolean on_key_release (GtkWidget *window, GdkEventKey *event, Spectrum3dGui *spectrum3dGui){
	if (event->type == GDK_KEY_RELEASE){
		//printf("key release\n");
       		switch (event->keyval){
			case GDK_KEY_a :
				a_keyPressed = FALSE;
				break;
			case GDK_KEY_q :
				q_keyPressed = FALSE;
				break;
			case GDK_KEY_Shift_L :
				shift_keyPressed = FALSE;
				break;
			case GDK_KEY_Shift_R :
				shift_keyPressed = FALSE;
				break;
			case GDK_KEY_g :
				g_keyPressed = FALSE;
				break;
			case GDK_KEY_Control_L :
				control_keyPressed = FALSE;
				break;
			case GDK_KEY_Control_R :
				control_keyPressed = FALSE;
				break;
			}
		}

	return FALSE;
}

#ifdef HAVE_LIBSDL
gboolean sdl_event(Spectrum3dGui *spectrum3dGui)
{
	Uint8 mouseState, *keystate;
	SDL_Event event;
	int MouseX,MouseY;
	
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

#endif


