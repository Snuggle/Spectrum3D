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

#ifndef DEF_GSTREAMER
#define DEF_GSTREAMER

gboolean showEqualizerWindow, analyse_rt, jack, loading, recording;
gchar *selected_file, *tmpPath;
gint64 pos, len;
int AUDIOFREQ, pose, spect_bands, playing, hzStep, frame_number_counter, bandsNumber; 
gfloat BPlowerFreq, BPupperFreq;
GLfloat spec_data[405][11030];
GMainLoop *loop, *loopTestSound;

GtkWidget *scaleSeek;

typedef enum TypeSource TypeSource;
enum TypeSource
{
	NONE, MIC, AUDIO_FILE
};
TypeSource typeSource;

typedef enum Source Source;
enum Source
{
	NO, MICRO, SOUND_FILE
};
Source source; 

GstElement *pipeline, *equalizer, *equalizer2, *equalizer3, *BP_BRfilter, *playbin;

void show_position(gchar *positionLabel);
void setPlayButtonIcon ();
void error_message_window(gchar *message);
void getBand();
gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data);
GstTaskPool * test_rt_pool_new ();

#endif



