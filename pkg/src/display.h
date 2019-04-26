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

#ifndef DEFINE_DISPLAY
#define DEFINE_DISPLAY

gboolean flatView, changeViewParameter, pointer, useCopyPixels;
int pose, firstPass3D, zoom, zoomFactor, textScale, lineScale, width, f, change, result, widthFrame, bandsNumber, storedFreq, flatviewDefinition;
float showGain, z, X, Y, Z, storedIntensity, PROPORTION; 
float AngleH, AngleV, AngleZ;
GLfloat x, flatViewHeight, YscaleX, flatViewY;
GLfloat prec[805][10005];
const GValue *magnitudes;
char fontPreference[100];

typedef enum ColorType ColorType;
enum ColorType
{
	PURPLE, RED, RAINBOW
};
ColorType colorType;

typedef enum ViewType ViewType;
enum ViewType
{
	THREE_D, THREE_D_FLAT, FLAT
	/* THREE_D : the 3D view
	FLAT : the 'FlatView'
	THREE_D_FLAT : a mix between the previous (a 3D that has been somehow 'flatened');*/
};
ViewType viewType;

#define RESIZE width/1200
#define WIDTH_WINDOW 1200 * RESIZE
#define HEIGHT_WINDOW 600

TTF_Font *font;

int sdlEvent();
void drawScale();
void RenderText();
void drawPointer();

#endif
