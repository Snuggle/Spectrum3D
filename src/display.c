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
#include <math.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <gtk/gtkgl.h>

#include "config.h"
#include "spectrum3d.h"
#include "display.h"

int ii = 0, a = 0, b = 0;
float cr = 0, cb = 1, cg = 0, k = 0;
float i = 0, l = 0, q = 0, YcoordFactor = 1, ZcoordFactor = 0;

/* Initialise several display values */
void init_display_values(){
	lineScale = 1, textScale = 1, pointer = FALSE, showPosition = TRUE;
	colorType = PURPLE;
	viewType = THREE_D;
	showGain = 0.2;
	zoom = 0;
	firstPass3D = 1;
	zoomFactor = 1;
	x = 1.2; // x is the width of the frame in 3D
	z = (float)spectrum3d.frames * spectrum3d.zStep; // z is the depth in 3D, used for the scale; zStep is the distance between 2 frames
	x_2d = 1.4; // x_2d is the width od the display in 2d
	X = -0.73; Y = -0.1; Z = -1; // those are the initial coordinates of the frame in 3D
	spectrum3d.previousFrames = spectrum3d.frames; // previousFrames will be usefull when the 'Front' view will be used, to restore the initial number of frames when coming back to the previous normal 3d view
}

gboolean
configure_SDL_gl_window (int width, int height)
{
	SDL_SetVideoMode(width, height, 24, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);

	glLoadIdentity();
	glViewport (0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gluPerspective(45, (float)2, .1, 200);
	glMatrixMode(GL_MODELVIEW);
		
	return TRUE;
}

gboolean compareGLfloat(GLfloat value1, GLfloat value2, GLfloat precision)
{
	return fabs (value1 - value2) < precision; // function that allows to compare two floats
}

/* Draw the spectrogram */
gboolean display_spectro(){
	if (playing && pose == 0){
		newEvent = TRUE;
		}

	if (newEvent){
		newEvent = FALSE;

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		glLoadIdentity();

		ii = 0, a = 0, b = 0;
		cr = 0, cb = 1, cg = 0, k = 0;
		i = 0, l = 0, q = 0;
		if (viewType == THREE_D){
			YcoordFactor = 1;
			ZcoordFactor = 0;
			}
		else if (viewType == THREE_D_FLAT){
			YcoordFactor = 5;
			ZcoordFactor = 1;
			}

		if (firstPass3D) {
			AngleH = -16.0;
			AngleV = 10.0;
			firstPass3D = 0;
			}

		if ( viewType == THREE_D || viewType == THREE_D_FLAT ){
			glTranslatef(X, Y, Z);

			glRotatef(AngleV, 1, 0, 0);
			glRotatef(AngleH, 0, 1, 0);
			glRotatef(AngleZ, 0, 0, 1);
			}
		else if (viewType == TWO_D){
			glTranslatef(-0.7, -0.4, -1.05);
			}

		/* 2D*/
		if (viewType == TWO_D){
			
			if (magnitudes != NULL) {
				y_2d = 0.8;
				k = 0;
				i = 0;
				q = y_2d / bandsNumber;
	
				l = 0;
				for (b = 0; b < spectrum3d.frames; b ++){
					glBegin(GL_QUAD_STRIP);
					i = 0;
					for (ii = zoom; ii < spect_bands + zoom; ii+=zoomFactor) {
						k = 0;
						for (a = 0; a < zoomFactor; a++) {
							k = k + spec_data[b][ii+a];
							}
						if (b == 0 && compareGLfloat(i, Ypointer, 0.0005)){
							storedFreq = ii;
							}
						k = (k/zoomFactor) * showGain;
						if (i <= y_2d) {
							glColor3f(10 * k, 0, 0); 
							glVertex2f(l, i);
							glVertex2f(l + x_2d/spectrum3d.frames, i);
							} 
						i += q;
						}
					l += x_2d/spectrum3d.frames;
					glEnd();
					}
				}
			}

		/* 3D VIEW */
		else {
			if (magnitudes != NULL) {
				l = 0;
				cr = 1, cg = 0, cb = 0;
				q = x/bandsNumber;
				for (b = 0; b < spectrum3d.frames; b++) {
					i = 0;
					/* set color */
					if (colorType == RAINBOW){
						if (b < (spectrum3d.frames * 0.5)) {
							cr-=(0.02  * 100 / spectrum3d.frames);
							}
						if (b > (spectrum3d.frames * 0.7)) {
							cr+=(0.02 * 100 / spectrum3d.frames);
							}
						if (b > (spectrum3d.frames * 0.25) && b < (spectrum3d.frames * 0.9)) {
							cg-=(0.01538 * 100 / spectrum3d.frames);
									}
						if (b < (spectrum3d.frames * 0.25)) {
							cg+=(0.04 * 100 / spectrum3d.frames);
							} 
						if (b > (spectrum3d.frames * 0.3) && b < (spectrum3d.frames * 0.9)) {
							cb+=(0.01666 * 100 / spectrum3d.frames);
							}
						}
					else if (colorType == PURPLE){
						cr = 1; cg = 0; cb = 1;
						}
					else if (colorType == RED){
						cr = 1; cg = 0; cb = 0;
						} 
				
					glBegin(GL_LINE_STRIP);
					glColor3f(cr , cg, cb); glVertex3f( 0, 0 ,l);
					for (ii = zoom; ii < spect_bands + zoom; ii+=zoomFactor) {
						k = 0;
						for (a = 0; a < zoomFactor; a++) {
							k = k + spec_data[b][ii+a];
							}
						if (b == 0 && compareGLfloat(i, Xpointer, 0.001)){
							storedFreq = ii;
							storedIntensity = (40 * k/zoomFactor) - 80; // this value has to be divided by the zoomFactor first; since the spec_data are multiplied by 40 and 80 added, the oposite should be done now to get the initial intensity value in dB;
							}
						k = (k/zoomFactor) * showGain;
						if (i <= x) {
							if (source == SOUND_FILE && analyse_rt == FALSE){
								int mult = 18;
								glColor3f((mult/2) * k, k * mult, 1/(k * mult)); 
								}
							else {
								glColor3f((10 * k *cr) , (10 * k *cg), (10 * k * cb)); 
								}
							//glVertex3f( i, k ,l);
							glVertex3f( i, k/(YcoordFactor), l - ((k/5) * ZcoordFactor));
							}
						i += q;
						}
					glEnd();
					l-=spectrum3d.zStep;	
					}
				}
			}
	
		/* Get value for the next turn : each value takes the value of 'preceeding' variable (spec_data[200][ii] takes the vaue of spec_data[199][ii], and so on) */
		if (pose == 0 && analyse_rt == TRUE) {
			for (b = spectrum3d.frames; b >= 0; b --){
				for (ii = 0; ii < spect_bands; ii++) {
					spec_data[b+1][ii] = spec_data[b][ii];
					}
				}
			}
		//}

	if (showPosition == 1 && analyse_rt == FALSE && playing) {
		show_playing_position();
		}
	if (pointer) {
		drawPointer();
		drawPointerText();
		}
	if (textScale == 1) {
		RenderText();
		}
	if (lineScale == 1) {
		drawScale();
		}
	
	}

	SDL_GL_SwapBuffers();
	return TRUE;	
}




