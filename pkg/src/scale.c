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

#include "config.h"
#include "spectrum3d.h"
#include "scale.h"

char textToRender[200];
float xX = 0, yY = 0, zZ = 0;

/* Draw line scale and panels */
void drawScale() {

	if (viewType == TWO_D){
		GLfloat i = 0, s = -0.02, y = y_2d;
		glBegin(GL_LINES);
		glColor3f(0.9, 0.9, 0.9);
		glVertex2f( s, 0); 
		glVertex2f( -0.001, 0);
		glVertex2f( s, 0.5 * y); 
		glVertex2f( -0.001, 0.5 * y);
		glVertex2f( s, 1 * y); 
		glVertex2f( -0.001, 1 * y);
		for (i = 0; i < 1; i+=0.1) {
			glColor3f(0.5, 0.5, 0.5);
			glVertex2f( s/2, i * y);
			glVertex2f( 0.001, i * y);
			}
		/*for (i = 0; i < 1; i+=0.02) {
			glColor3f(0.2, 0.2, 0.2);
			glVertex2f( s, i * y);
			glVertex2f( 0, i * y);
			}*/
		glEnd();
	
	}

	else{	
		GLfloat i = 0, alpha = 0;
		if (spectrum3d.frames == 1){
			glRotatef(90, 1, 0, 0);
			}
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f( 0 * x, 0, -z); 
		glVertex3f( 0 * x, 0, 0.05);
		glVertex3f( 0.5 * x, 0, -z);
		glVertex3f( 0.5 * x, 0, 0.05);
		glVertex3f( 1 * x, 0, -z);
		glVertex3f( 1 * x, 0, 0.05);
		for (i = 0; i < 1; i+=0.1) {
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f( i * x, 0, -z);
			glVertex3f( i * x, 0, 0.03);
			}
		for (i = 0; i < 1; i+=0.02) {
			glColor3f(0.2, 0.2, 0.2);
			glVertex3f( i * x, 0, -z);
			glVertex3f( i * x, 0, 0.02);
			}
	glEnd();
}

}

/* Print text scale */
void RenderText() {
	int i = 0;
	cairo_surface_t *surface;
        cairo_t *cr;
	GLuint textureId;
	const unsigned char* surfaceData;

	if (viewType == TWO_D){
		xX = -0.1, yY = 0.05, zZ = 0;
		}
	else {
		xX = -0.02, yY = 0, zZ = 0;
		}
	
	glColor3f(1, 1, 1);
	
	for (i = 0 + (zoom * hzStep); i <= ((hzStep * bandsNumber *zoomFactor) + (zoom * hzStep)); i+= ((hzStep * bandsNumber * zoomFactor)/10)) {
			
		sprintf(textToRender, "%d", i);
		
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 32, 32);
		cr = cairo_create (surface);
		//cairo_select_font_face (cr, "serif", CAIRO_FONT_SLANT_NORMAL,  CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
		if (viewType == TWO_D){
			cairo_set_font_size (cr, 8.0);
			cairo_move_to (cr, 0, 12);
			}
		else {
			cairo_set_font_size (cr, 10.0);
			cairo_move_to (cr, 0, 18);
			}		
		cairo_show_text (cr, textToRender);
		cairo_surface_flush(surface);
		surfaceData = cairo_image_surface_get_data (surface);
	
		unsigned Texture = 0;

		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cairo_image_surface_get_width(surface), cairo_image_surface_get_height(surface), 0, GL_BGRA, GL_UNSIGNED_BYTE, surfaceData);
		
		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 1);
		glTexCoord2d(0, 0); glVertex3d(xX, yY, 0.01);
		glTexCoord2d(1, 0); glVertex3d(xX+0.1, yY, 0.01);
		glTexCoord2d(1, 1); glVertex3d(xX+0.1, yY-0.2, 0.01);
		glTexCoord2d(0, 1); glVertex3d(xX, yY-0.2, 0.01);
		glEnd();
		
		glDeleteTextures(1, &Texture);
		cairo_destroy (cr);
                cairo_surface_destroy (surface);
		
		if (viewType == TWO_D){
			yY += y_2d/10;
			}
		else {
			xX += x/10;
			}
	}
}

/* Draw pointer when playing an audio file to show location */
void show_playing_position(){
	if (viewType == TWO_D){
		float xlocation = x_2d * (float)pos/len;
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f( xlocation, 0, 0.001);
		glVertex3f( xlocation, y_2d, 0.001);
		glEnd();
		}
	else {
		float zlocation = z * -(float)pos/len;
		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 0.5);
		glVertex3f( 0, 0 , zlocation);
		glVertex3f( x, 0, zlocation);
		glVertex3f( x, 0.3 ,zlocation);
		glVertex3f( 0, 0.3, zlocation);
		glEnd();
		}
}

void drawPointer(){
	if (viewType == TWO_D){
		glBegin(GL_LINES);
		glColor3f(0.9, 0.9, 0.9);
		glVertex3f( 1.4, Ypointer, 0.001); 
		glVertex3f( 0, Ypointer, 0.001);
		glEnd();
		}
	else {
		glBegin(GL_LINES);
		glColor3f(0.9, 0.9, 0.9);
		glVertex3f( Xpointer, 0.5, 0); 
		glVertex3f( Xpointer, 0, 0);
		glEnd();
		}
}

void drawPointerText(){
	int i = 0;
	cairo_surface_t *surface;
        cairo_t *cr;
	GLuint textureId;
	const unsigned char* surfaceData;

	if (viewType == TWO_D){
		xX = 1.4, yY = Ypointer + 0.1, zZ = 0;
		}
	else {
		xX = Xpointer - 0.1, yY = 0.45, zZ = 0;
		}
	
	if (viewType == TWO_D){
		sprintf(textToRender, " %d Hz", (int)(storedFreq * hzStep));
		}
	else {
		sprintf(textToRender, "%d Hz      %f dB", (int)(storedFreq * hzStep), storedIntensity);
		}
		
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 128, 32);
	cr = cairo_create (surface);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_set_font_size (cr, 10.0);
	cairo_move_to (cr, 0, 16);
	cairo_show_text (cr, textToRender);
	cairo_surface_flush(surface);
	surfaceData = cairo_image_surface_get_data (surface);

	unsigned Texture = 0;

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cairo_image_surface_get_width(surface), cairo_image_surface_get_height(surface), 0, GL_BGRA, GL_UNSIGNED_BYTE, surfaceData);
	
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2d(0, 0); glVertex3d(xX, yY, 0.01);
	glTexCoord2d(1, 0); glVertex3d(xX+0.3, yY, 0.01);
	glTexCoord2d(1, 1); glVertex3d(xX+0.3, yY-0.15, 0.01);
	glTexCoord2d(0, 1); glVertex3d(xX, yY-0.15, 0.01);
	glEnd();
	
	glDeleteTextures(1, &Texture);
	cairo_destroy (cr);
        cairo_surface_destroy (surface);
	
	if (viewType == TWO_D){
		yY += y_2d/10;
		}
	else {
		xX += x/10;
		}
}



