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

#ifndef DEFINE_EVENTS
#define DEFINE_EVENTS

gboolean newEvent;
int bandsNumber;
GLfloat x, X, Y, Z, AngleH, AngleV, AngleZ, y_2d, Xpointer, Ypointer;

typedef enum ViewType ViewType;
enum ViewType
{
	THREE_D, THREE_D_FLAT, TWO_D
	/* 
	THREE_D : the 3D view
	TWO_D : the 2D view
	THREE_D_FLAT : a mix between the previous (a 3D that has been somehow 'flatened');
	*/
};
ViewType viewType;

void on_stop();
void playFromSource(GtkWidget *widget, gpointer *data);
gboolean configure_SDL_gl_window (int width, int height);
void quit_spectrum3d();
void reset_view();
void front_view();

#endif
