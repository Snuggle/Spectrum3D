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

gboolean newEvent, jack, loading, analyse_rt;
int AUDIOFREQ, playing, pose, zoom, zoomFactor, bandsNumber, hzStep;
float showGain;
GLfloat x, z, X, Y, Z, AngleH, AngleV, AngleZ;
GLfloat spec_data[405][11030];

typedef enum TypeSource TypeSource;
enum TypeSource
{
	NONE, MIC, AUDIO_FILE
};
TypeSource typeSource;

typedef enum ViewType ViewType;
enum ViewType
{
	THREE_D, THREE_D_FLAT, TWO_D
};
ViewType viewType;

GMainLoop *loop;
GstElement *pipeline;

void setPlayButtonIcon();
void change_adjust_start();

#endif
