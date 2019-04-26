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

#ifndef DEFINE_ONCLICK
#define DEFINE_ONCLICK

gboolean flatView, changeViewParameter, pointer;
guint displaySpectroTimeout;
int AUDIOFREQ, playing, pose, zoom, forward, backward, nSec, scale, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, presetWidth, intervalTimeout, bandsNumber, hzStep;
float showGain, z, X, Y, Z, PROPORTION, AngleH, AngleV, AngleZ, presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ; 
GLfloat x;

typedef enum TypeSource TypeSource;
enum TypeSource
{
	MIC, AUDIO_FILE, JACK
};
TypeSource typeSource;

typedef enum ViewType ViewType;
enum ViewType
{
	THREE_D, THREE_D_FLAT, FLAT
};
ViewType viewType;

GtkObject *adjust_bands, *adjust_start;
GtkWidget *pScaleBands, *pComboRange;

#define RESIZE width/1200

GMainLoop *loop;
GstElement *pipeline;

void setPlayButtonIcon();
void getTextDisplayLabel();
void onReset();
void setupSDL();
void setupOpengl();
void displaySpectro();
void change_adjust_start();

#endif
