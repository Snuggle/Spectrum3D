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

#ifndef DEFINE_MENU
#define DEFINE_MENU

gboolean newEvent;
char prefPath[100], policyName[20];
GLfloat z, X, Y, Z, AngleH, AngleV, AngleZ; 
FILE *rcFile;
GMainLoop *loopTestSound;

enum {
    ACTION_COLUMN,
    KEYBOARD_COLUMN,
    MOUSE_COLUMN,
    N_COLUMN
};

typedef enum ColorType ColorType;
enum ColorType
{
	PURPLE, RED, RAINBOW
};
ColorType colorType;

void playTestSound(GtkWidget *pWidget, gpointer data);
void change_freq_test_sound(GtkWidget *widget, gpointer data);
void change_volume_test_sound(GtkWidget *pWidget, gpointer data);
gboolean display_spectro();

#endif
