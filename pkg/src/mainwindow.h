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

#ifndef DEF_MAINWINDOW
#define DEF_MAINWINDOW

gboolean showEqualizerWindow, flatView, enableTouch;
char *sFile;
gchar *uri, file[200];
GSList *filenames;
guint displaySpectroTimeout;
gint64 pos;
guint64 interval;
int AUDIOFREQ, pose, spect_bands, playing, zoom, stringInt, range, counterNumber, forward, backward, nSec, scale, firstPass3D, zoomFactor, textScale, showPanels, lineScale, change, f, onClickWidth, width, presetWidth, intervalTimeout, widthFrame, hzStep, columnNumber, previousWidth;
gboolean realtime;
float z, X, Y, Z, PROPORTION; 
gfloat BPlowerFreq, BPupperFreq;
GLfloat x;
GLfloat prec[805][10005];
GstElement *pipeline;
GMainLoop *loop;
const GValue *magnitudes;

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

GtkWidget *pScaleDepth;

#define RESIZE width/1200
GstElement *pipeline, *equalizer, *equalizer2, *equalizer3, *BP_BRfilter;

int displayPausedSpectro();
gboolean playSlowly();
void getFileName();
void setPlayButtonIcon ();
void displaySpectro();
gboolean sdlEvent();
void setupSDL();
void setupOpengl();
void setupGeis();
void sdlQuit();
void geisQuit();
void effects_window();
void errorMessageWindow(char *message);
void getBand();
gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data);
GstTaskPool *
test_rt_pool_new ();
//#ifdef GEIS
gboolean geisGesture();
//#endif

#endif



