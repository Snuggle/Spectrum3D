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
#include <gst/gst.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "config.h"

#include "display.h"

int ii = 0, a = 0, b = 0;
float cr = 0, cb = 1, cg = 0, k = 0;
float i = 0, l = 0, q = 0, YcoordFactor = 1, ZcoordFactor = 0;

void setupSDL()
{
	printf("Init SDL\n");
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
		}
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
		}
	SDL_SetVideoMode(WIDTH_WINDOW, HEIGHT_WINDOW, 24, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
	SDL_WM_SetCaption(PACKAGE_NAME, NULL);
	font = TTF_OpenFont(fontPreference, 16 * RESIZE);
}

void sdlQuit()
{
	TTF_CloseFont(font);
        SDL_Quit();
	TTF_Quit();
}

void setupOpengl() {
	printf("Init OpenGL\n");
	glViewport(0, 0, WIDTH_WINDOW, HEIGHT_WINDOW);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gluPerspective(45, (float)WIDTH_WINDOW/HEIGHT_WINDOW, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
}

void initDisplay()
{
	setupSDL();
	setupOpengl();
	YscaleX = 0;
}

gboolean compareGLfloat(GLfloat value1, GLfloat value2, GLfloat precision)
{
	return fabs (value1 - value2) < precision; // function that allows to compare two floats
}

/* Draw the spectrogram while playing */
gboolean displaySpectro(){
	
	flatViewHeight = 0.7;
	int factor = 2;

	if (viewType == THREE_D){
			YcoordFactor = 1;
			ZcoordFactor = 0;
			}
	else if (viewType == THREE_D_FLAT){
			YcoordFactor = 5;
			ZcoordFactor = 1;
			}

/* FLATVIEW*/
	if (viewType == FLAT){
		if (width < 800 || width > 1300 ){
			useCopyPixels = FALSE;
			}
		float XX = 0.5, YY = -0.35, ZZ = -1;
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		glLoadIdentity();
		glTranslatef(XX * RESIZE, YY, ZZ);
		k = 0;
		i = 0;
		q = flatViewHeight / bandsNumber;
	
		if (changeViewParameter || useCopyPixels == FALSE || (useCopyPixels && flatviewDefinition != 400)) {
			l = -1.1 * RESIZE;
			for (b = flatviewDefinition * RESIZE; b >= 0; b--){
				glBegin(GL_QUAD_STRIP);
				i = 0;
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					k = 0;
/* In any case, all the 10000 frequencies values are stored in an array; in case we want to zoom, we have to calculate the displayed value, that is the mean of several values; we could let the 'spectrum' element of Gstreamer do that (by changing the "bands" property), but the advantage is that display can be changed, 'zoomed' and explored when it is in the paused state */
					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[b][ii+a];
						prec[b+1][ii+a] = prec[b][ii+a];
						}
						k = (k/zoomFactor) * showGain;
						if (i <= flatViewHeight) {
							glColor3f(10 * k, 0, 0); 
							glVertex2f( l, i);
							glVertex2f( l + (1.1/flatviewDefinition), i);
							} 
						i += q;
					}
				l += 1.1/flatviewDefinition;
				glEnd();
				}
			//glEnd();
			changeViewParameter = FALSE;
			}
		else {
			for (b = flatviewDefinition * RESIZE; b >= 0; b--){
				for (ii = 0; ii < 10000; ii++) {
					prec[b+1][ii] = prec[b][ii];
					}
				}
			glReadBuffer(GL_FRONT);
			glDrawBuffer(GL_BACK);
			// glBlitFrameBuffer had strange behavior sometimes
			// these values work for width from 800 to 1300 (the very best fit is at 1200): 
			glRasterPos2f((-1.1 * RESIZE), -0.009);
			glCopyPixels(167 * RESIZE, 40, 795 * RESIZE, 590, GL_COLOR);			
			}

		glBegin(GL_QUAD_STRIP);
		k = 0;
		i = 0;	
		for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
			for (a = 0; a < zoomFactor; a++) {
				k = k + prec[0][ii+a];
				prec[1][ii+a] = prec[0][ii+a];
				}
				k = (k/zoomFactor) * showGain;
			if (i <= flatViewHeight) {
				glColor3f(k * 10, 0, 0);
				glVertex2f(0, i);
				glVertex2f(-0.0025, i);
				if (compareGLfloat(i, flatViewY, 0.00001)){
					storedFreq = ii;
					storedIntensity = (k * 40) - 80;
					}	
			i += q;
			}
		}
	}
/* 3D VIEW */
	else {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		glLoadIdentity();

		ii = 0, a = 0, b = 0;
		cr = 0, cb = 1, cg = 0, k = 0;
		i = 0, l = 0, q = 0;

		if (magnitudes != NULL) {
				if (firstPass3D) {
					AngleH = -16.0;
					AngleV = 10.0;
					firstPass3D = 0;
					}
		
			glTranslatef(X, Y, Z);

			glRotatef(AngleV, 1, 0, 0);
			glRotatef(AngleH, 0, 1, 0);
			glRotatef(AngleZ, 0, 0, 1);

			l = -z;
			cr = 0.6, cb = 1, cg = 0;
			q = x/bandsNumber;
			for (b = (100 * factor)/f * z; b >= 0; b--) {
				i = 0;
				if (colorType == RAINBOW){
					if (b > ((25 * factor)/f * z) && b < ((90 * factor)/f * z)) {
								cg+=f * 0.01538 / factor * z;
								}
					if (b < ((25 * factor)/f * z)) {
						cg-=f * 0.04 / factor * z;
						} 
					if (b < ((50 * factor)/f * z)) {
						cr+=f * 0.02 / factor * z;
						} 
					if (b > ((70 * factor)/f * z)) {
						cr-=f * 0.02 / factor * z;
						} 
					if (b > ((30 * factor)/f * z) && b < (90 * factor/f * z)) {
						cb-=f * 0.01666 / factor * z;
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
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					k = 0;
					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[b][ii+a];
						prec[b+1][ii+a] = prec[b][ii+a];
						}
						k = (k/zoomFactor) * showGain;
						if (i <= x) {
							glColor3f((10 * k *cr) , (10 * k *cg), (10 * k * cb)); 
							//glVertex3f( i, k ,l);
							glVertex3f( i, k/(YcoordFactor), l - ((k/5) * ZcoordFactor));
							}
						i += q;
					}
				glEnd();
				l+=(0.01 / factor) * f;	
				}
	
				k = 0;
				i = 0;
				glBegin(GL_LINE_STRIP);
				glColor3f(cr , cg, cb); glVertex3f( 0, 0 ,l);
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[0][ii+a];
						prec[1][ii+a] = prec[0][ii+a];
						}
						k = (k/zoomFactor) * showGain;
					if (i <= x) {
						glColor3f(1, 0.3, 0.5);
						//glVertex3f( i, k ,0);
						glVertex3f( i, k/(YcoordFactor), 0 - ((k/5) * ZcoordFactor));
						if (compareGLfloat(i, YscaleX, 0.00001)){
							storedFreq = ii;
							storedIntensity = (k * 40) - 80;
							}
					i += q;
					}
				}
			}
	}

glEnd();

if (lineScale == 1) {
	drawScale();
	}
if (pointer) {
	drawPointer();
	}
if (textScale == 1) {
	RenderText();
	}
if (pointer) {
	drawPointerText();
	}
SDL_GL_SwapBuffers();
if (pose == 0) {
	return TRUE;
	}
else {
	return FALSE;
	}
}

/* Draw the spectrogram while paused (while image is frozen) */
gboolean displayPausedSpectro()
{
if (change) {
	
	flatViewHeight = 0.7;
	int factor = 2;
	GLfloat w = 1;
	
	if (viewType == THREE_D){
			YcoordFactor = 1;
			ZcoordFactor = 0;
			}
	else if (viewType == THREE_D_FLAT){
			YcoordFactor = 5;
			ZcoordFactor = 1;
			}

/* FLAT VIEW */
	if (viewType == FLAT){
		if (width < 800 || width > 1300 ){
			useCopyPixels = FALSE;
			}
		float XX = 0.5, YY = -0.35, ZZ = -1;
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		glLoadIdentity();
		glTranslatef(XX * RESIZE, YY, ZZ);
		k = 0;
		i = 0;
		q = flatViewHeight / bandsNumber;
	
			l = -1.1 * RESIZE;
			for (b = flatviewDefinition * RESIZE; b >= 0; b--){
				glBegin(GL_QUAD_STRIP);
				i = 0;
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					k = 0;

					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[b][ii+a];
						}
						k = (k/zoomFactor) * showGain;
						if (i <= flatViewHeight) {
							glColor3f(10 * k, 0, 0); 
							glVertex2f( l, i);
							glVertex2f( l + (1.1/flatviewDefinition), i);
							} 
						i += q;
					}
				l += 1.1/flatviewDefinition;
				glEnd();
				}
			changeViewParameter = FALSE;
			
		glBegin(GL_QUAD_STRIP);
		k = 0;
		i = 0;	
		for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
			for (a = 0; a < zoomFactor; a++) {
				k = k + prec[0][ii+a];
				}
				k = (k/zoomFactor) * showGain;
			if (i <= flatViewHeight) {
				glColor3f(k * 10, 0, 0);
				glVertex2f(0, i);
				glVertex2f(-0.0025, i);
				if (compareGLfloat(i, flatViewY, 0.00001)){
					storedFreq = ii;
					storedIntensity = (k * 40) - 80;
					}	
			i += q;
			}
		}
	}
		
/* 3D VIEW */
	else {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
		glLoadIdentity();

		ii = 0, a = 0, b = 0;
		cr = 0, cb = 1, cg = 0, k = 0;
		i = 0, l = 0, q = 0;

		if (magnitudes != NULL) {
				if (firstPass3D) {
					AngleH = -16.0;
					AngleV = 10.0;
					firstPass3D = 0;
					}
	
			glTranslatef(X, Y, Z);

			glRotatef(AngleV, 1, 0, 0);
			glRotatef(AngleH, 0, 1, 0);
			glRotatef(AngleZ, 0, 0, 1);

			l = -z;
			cr = 0.6, cb = 1, cg = 0;
			q = x/bandsNumber;
			for (b = (100 * factor)/f * z; b >= 0; b--) {
				i = 0;
				if (b > ((25 * factor)/f * z) && b < ((90 * factor)/f * z)) {
							cg+=f * 0.01538/z * factor;
							}
				if (b < ((25 * factor)/f * z)) {
					cg-=f * 0.04/z * factor;
					} 
				if (b < ((50 * factor)/f * z)) {
					cr+=f * 0.02/z * factor;
					} 
				if (b > ((70 * factor)/f * z)) {
					cr-=f * 0.02/z * factor;
					} 
				if (b > ((30 * factor)/f * z) && b < (90 * factor/f * z)) {
					cb-=f * 0.01666/z * factor;
					} 
				if (colorType == 0) {
					cr = 1; cg = 0, cb = 1;
					}
				glBegin(GL_LINE_STRIP);
				glColor3f(cr , cg, cb); glVertex3f( 0, 0 ,l);
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					k = 0;
					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[b][ii+a];
						}
						k = (k/zoomFactor) * showGain;
						if (i <= x) {
							glColor3f((10 * k * cr) , (10 * k * cg), (10 * k * cb)); 
							//glVertex3f( i, k ,l);
							glVertex3f( i, k/(YcoordFactor), l - ((k/5) * ZcoordFactor));
							}
						i += q;
					}
				glEnd();
				l+=(0.01 / factor) * f;	
				}

				k = 0;
				i = 0;
				glBegin(GL_LINE_STRIP);
				glColor3f(cr , cg, cb); glVertex3f( 0, 0 ,l);
				for (ii = zoom; ii < 10000 + zoom; ii+=zoomFactor) {
					for (a = 0; a < zoomFactor; a++) {
						k = k + prec[0][ii+a];
						}
						k = (k/zoomFactor) * showGain;
					if (i <= x) {
						glColor3f(1, 0.3, 0.5);
						//glVertex3f( i, k ,0);
						glVertex3f( i, k/(YcoordFactor), 0 - ((k/5) * ZcoordFactor));
					if (compareGLfloat(i, YscaleX, 0.00001)){
						storedFreq = ii;
						storedIntensity = (k * 40) - 80;
						}
					i += q;	
					}
				}
			}
	}
	glEnd();

	if (lineScale == 1) {
		drawScale();
		}
	if (pointer) {
		drawPointer();
		}
	if (textScale == 1) {
		RenderText();
		}
	if (pointer) {
		drawPointerText();
	}
	SDL_GL_SwapBuffers();
	change = 0;
	}
if (pose == 0) {
	return FALSE;
	}
else {
	return TRUE;
	}

}




