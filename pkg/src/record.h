/* This file is part of PitchTempoPlayer (PTPlayer).

    PitchTempoPlayer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PitchTempoPlayer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PitchTempoPlayer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEFINE_RECORD
#define DEFINE_RECORD

gboolean recording;
gchar *encodePath, *tmpPath, *selected_file, analyse_rt;
int playing, pose;

GMainLoop *loop;

typedef enum TypeSource TypeSource;
enum TypeSource
{
	NONE,
	MIC, 			// source is the microphone
	AUDIO_FILE, 		// source is an audio file 
};
TypeSource typeSource;

void playFromSource(GtkWidget *widget, gpointer *data);
void on_stop();
void setPlayButtonIcon (GtkWidget *widget, gpointer data);
void error_message_window(gchar *message);
void load_audio_file(gchar *fileType);

#endif



