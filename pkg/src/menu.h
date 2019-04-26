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

gboolean useCopyPixels;
char fontPreference[100], prefPath[100];
char policyName[20];
int width, presetWidth, priority, interval, flatviewDefinition;
gboolean realtime, enableTouch;
float X, Y, Z, AngleH, AngleV, AngleZ, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 
static GtkWidget *mainWindow;
FILE* pref, *rcFile;

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


void change_adjust(GtkWidget *pWidget, gpointer data);

#endif
