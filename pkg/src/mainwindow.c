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
#include <gst/gst.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "config.h"

#ifdef HAVE_LIBJACK
	#include <jack/jack.h>
#endif

#include "mainwindow.h"

const GValue *magni;
GMainLoop *loopTestSound;
gdouble freqTestSound = 440;
GstElement *srcTestSound;
#define NBANDS 10

#ifdef HAVE_LIBJACK
	static GtkWidget *mainWindow;
#endif

static GstTaskPool *pool;

void initGstreamer(){
	gst_init (NULL, NULL);	
	equalizer = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands");
	g_assert (equalizer);
	g_object_set (G_OBJECT (equalizer), "num-bands", NBANDS, NULL);
	equalizer2 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands2");
	g_assert (equalizer2);
	g_object_set (G_OBJECT (equalizer2), "num-bands", NBANDS, NULL);
	equalizer3 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands3");
	g_assert (equalizer3);
	g_object_set (G_OBJECT (equalizer3), "num-bands", NBANDS, NULL);
	BP_BRfilter = gst_element_factory_make ("audiochebband", "BP_BRfilter");
	g_assert (BP_BRfilter);
}

static GstBusSyncReply
sync_bus_handler (GstBus * bus, GstMessage * message, GstElement * bin)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_STREAM_STATUS:
    {
      GstStreamStatusType type;
      GstElement *owner;
      const GValue *val;
      gchar *path;
      GstTask *task = NULL;

      //g_message ("received STREAM_STATUS");
      gst_message_parse_stream_status (message, &type, &owner);

      val = gst_message_get_stream_status_object (message);

      //g_message ("type:   %d", type);
      path = gst_object_get_path_string (GST_MESSAGE_SRC (message));
      //g_message ("source: %s", path);
      g_free (path);
      path = gst_object_get_path_string (GST_OBJECT (owner));
      //g_message ("owner:  %s", path);
      g_free (path);

      if (G_VALUE_HOLDS_OBJECT (val)) {
        //g_message ("object: type %s, value %p", G_VALUE_TYPE_NAME (val),
            //g_value_get_object (val));
      } else if (G_VALUE_HOLDS_POINTER (val)) {
        //g_message ("object: type %s, value %p", G_VALUE_TYPE_NAME (val),
            //g_value_get_pointer (val));
      } else if (G_IS_VALUE (val)) {
        //g_message ("object: type %s", G_VALUE_TYPE_NAME (val));
      } else {
        //g_message ("object: (null)");
        break;
      }

      /* see if we know how to deal with this object */
      if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
        task = g_value_get_object (val);
      }

      switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
          if (task) {
            //g_message ("created task %p, setting pool", task);
            gst_task_set_pool (task, pool);
          }
          break;
        case GST_STREAM_STATUS_TYPE_ENTER:
          break;
        case GST_STREAM_STATUS_TYPE_LEAVE:
          break;
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  /* pass all messages on the async queue */
  return GST_BUS_PASS;
}

/* Gstreamer message handler */
gboolean message_handler (GstBus * bus, GstMessage * message, gpointer data)
{	
	int ii = 0;
			
        if (message->type == GST_MESSAGE_ELEMENT) {
		//printf("message = element\n");
		const GstStructure *s = gst_message_get_structure (message);
		const gchar *name = gst_structure_get_name (s);
		GstClockTime endtime;
		//printf("name = %s\n", name);
		

                if (strcmp (name, "spectrum") == 0) {
			//printf("message = element\n");
			if (!gst_structure_get_clock_time (s, "endtime", &endtime))
			endtime = GST_CLOCK_TIME_NONE;
			magnitudes = gst_structure_get_value (s, "magnitude");
			for (ii = 0; ii < 10000 ; ii++) {
				magni = gst_value_list_get_value (magnitudes, ii);
				prec[0][ii] = ((80 + (g_value_get_float(magni)))/40);
				/*if (ii > 100 && ii < 120){
					printf("%f ",prec[0][ii]);
					} */
				}
			//displaySpectro();
			}
		}
	if (message->type == GST_MESSAGE_EOS) {
		g_main_loop_quit (loop);
		}
return TRUE;
}

/* Play  a very small part of an audio file every time this is called */
gboolean playSlowly()
{
	SDL_Event event;
	Uint8* keys;
	SDL_PollEvent(&event);
		
	keys = SDL_GetKeyState(NULL);
		if ( keys[SDLK_v] && keys[SDLK_RIGHT] ) {
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			SDL_Delay(50);
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
			}
if (pose == 0) {
	return FALSE;
	}
else {
	return TRUE;
	}
}

/* Print the total duration and the time position in a terminal when playing an audio file */
static gboolean cb_print_position (GstElement *pipeline)
{

		GstFormat fmt = GST_FORMAT_TIME;
		gint64 len; 
		if (gst_element_query_position (pipeline, &fmt, &pos)
		&& gst_element_query_duration (pipeline, &fmt, &len)) {
		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
		GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
		}
	
	return TRUE;
			
}

/* Seek backward or forward when playing an audio file */
static gboolean seek_to_time (GstElement *pipeline)
{
if (forward == 1 || backward == 1)
	{ 
	if (!gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
	GST_SEEK_TYPE_SET, nSec * GST_SECOND + pos,
	GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
	g_print ("Seek failed!\n");
	}
	}
return TRUE;
}

int checkJackActive(){

#ifdef HAVE_LIBJACK
	printf("*** Checking if JACK is running (Jack error messages are normal):\n");
	jack_client_t *src_client;
	jack_status_t status;
	src_client = jack_client_open ("src_client", JackNoStartServer, &status); 
	if (src_client != NULL) {
		printf("JACK seems to be running. Please stop JACK and start playing again\n");
		return 1;
		}
	else if (src_client == NULL) {
		printf("*** --> OK, JACK is not running\n");
		return 0;
		}
#endif
#ifdef HAVE_LIBJACK
	return 0;
#endif
}

getFileUri(){
	sprintf(file, "file://%s", sFile); 
	uri = g_strdup (file);
	}

void playFromSource(GtkWidget *pWidget, gpointer data)
{
	guint displayPausedSpectroTimeout; 
	interval = 100;
if (playing == 1)
    	{
		if (pose == 0) {
		        pose = 1;
			setPlayButtonIcon();
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
			displayPausedSpectroTimeout = g_timeout_add (80, (GSourceFunc) displayPausedSpectro, pipeline);
			if (typeSource == AUDIO_FILE) {
				g_timeout_add (80, (GSourceFunc) playSlowly, pipeline);
				}
			}
		 else {
		        pose = 0;
			setPlayButtonIcon();
			displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
		        }
    	}

/* Play */
else if (playing == 0) {
	playing = 1;
	setPlayButtonIcon();
	//if (viewType == FLAT) {
	//	width = 1200;
	//	}
	//else {
		width = presetWidth;
	//	}
	hzStep = (AUDIOFREQ/2) / spect_bands;
	PROPORTION = (float)width / 1000;
	x = 1.2 * RESIZE;
	changeDepth(pScaleDepth, NULL);
	X = -0.7 * RESIZE;
	Y = -0.1 * RESIZE;
	Z = -1.05;
	BPlowerFreq = 0; BPupperFreq = 40000;
		
/* init everything */
	setupSDL();	
	setupOpengl();
#ifdef HAVE_LIBUTOUCH_GEIS
	setupGeis();
#endif
	guint timeoutEvent, timeoutPrintPosition, timeoutSeekToTime, timeoutTouch;
	GstElement *alsasrc, *audioconvert, *audioconvert2, *audioconvert3, *spectrum, *alsasink, *playbin, *sink;
	GstBus *bus, *busMH, *busrt;
	GstStateChangeReturn ret;
	GstCaps *caps;
	GstPad *audiopad;

	displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
	timeoutEvent = g_timeout_add (50, (GSourceFunc) sdlEvent, pipeline);
#ifdef HAVE_LIBUTOUCH_GEIS
	if (enableTouch){
		timeoutTouch = g_timeout_add (50, (GSourceFunc) geisGesture, NULL);
		}		
#endif

/* SOURCE IS MICROPHONE */
if (typeSource == MIC) {
	gst_init (NULL, NULL);
	int jackActive = checkJackActive();
	if (jackActive){
		errorMessageWindowJack("JACK seems to be running\nStop JACK server and start playing again");
		}
	else {
	  	pool = test_rt_pool_new ();

		loop = g_main_loop_new(NULL, FALSE);
		playing = 1;
	  
		pipeline = gst_pipeline_new ("pipeline");
		g_assert (pipeline);
		alsasrc = gst_element_factory_make ("alsasrc", "alsasrc");
		g_assert (alsasrc);
		audioconvert = gst_element_factory_make ("audioconvert", NULL);
		g_assert (audioconvert);
		audioconvert2 = gst_element_factory_make ("audioconvert", NULL);
		g_assert (audioconvert2);
		audioconvert3 = gst_element_factory_make ("audioconvert", NULL);
		g_assert (audioconvert3);
		spectrum = gst_element_factory_make ("spectrum", "spectrum");
		g_assert (spectrum);	
		g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", interval * 1000000, NULL);
		equalizer = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands");
		g_assert (equalizer);
		g_object_set (G_OBJECT (equalizer), "num-bands", NBANDS, NULL);
		equalizer2 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands2");
		g_assert (equalizer2);
		g_object_set (G_OBJECT (equalizer2), "num-bands", NBANDS, NULL);
		equalizer3 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands3");
		g_assert (equalizer3);
		g_object_set (G_OBJECT (equalizer3), "num-bands", NBANDS, NULL);
		if (showEqualizerWindow){
			getBand();
			}
		BP_BRfilter = gst_element_factory_make ("audiochebband", "BP_BRfilter");
		g_assert (BP_BRfilter);
		g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, "upper-frequency", BPupperFreq, NULL);
		alsasink = gst_element_factory_make ("fakesink", "alsasink");
		g_assert (alsasink);

		gst_bin_add_many (GST_BIN (pipeline), alsasrc, audioconvert, equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, alsasink, NULL);
		caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);
		if (!gst_element_link (alsasrc, audioconvert) ||
		    !gst_element_link_filtered (audioconvert, equalizer, caps) ||
		    !gst_element_link_many (equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, alsasink, NULL)) {
		    fprintf (stderr, "can't link elements\n");
			exit (1);
		    }
		if (realtime) {
			bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
			gst_bus_set_sync_handler (bus, (GstBusSyncHandler) sync_bus_handler, pipeline);
			gst_object_unref (bus);
			}

		//timeoutEvent = g_timeout_add (50, (GSourceFunc) sdlEvent, pipeline);
		//displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
		ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
		if (ret == GST_STATE_CHANGE_FAILURE){
			g_print ("Failed to start up pipeline!\n");
			}
		busMH = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
		gst_bus_add_watch(busMH, message_handler, NULL);
		printf ("Now playing\n");
		g_main_loop_run (loop);
		//g_source_remove(timeoutEvent);
		//g_source_remove(displaySpectroTimeout);
		gst_element_set_state (pipeline, GST_STATE_NULL);
		printf ("Stop playing\n");
		gst_object_unref (busMH);
		//sdlQuit();
	#ifdef HAVE_LIBUTOUCH_GEIS
		//geisQuit();
	#endif
		}
    }

/* SOURCE IS AUDIO FILE */
else if (typeSource == AUDIO_FILE) {
	gst_init (NULL, NULL);
	int jackActive = checkJackActive();
    if (jackActive){
		errorMessageWindowJack("JACK seems to be running\nStop JACK server and start playing again");
		}
    else {
	pool = test_rt_pool_new ();

	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;
while (filenames != NULL) {
	getFileUri();
            
	pipeline = gst_bin_new ("pipeline");
	playbin = gst_element_factory_make ("playbin2", "source");
	g_assert (playbin);
	g_object_set (G_OBJECT (playbin), "uri", uri, NULL);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_assert (spectrum);
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80,
	    "message", TRUE, "interval", interval * 1000000, NULL);
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert);
	audioconvert2 = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert2);
	audioconvert3 = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert3);
	equalizer = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands");
	g_assert (equalizer);
	g_object_set (G_OBJECT (equalizer), "num-bands", NBANDS, NULL);
	equalizer2 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands2");
	g_assert (equalizer2);
	g_object_set (G_OBJECT (equalizer2), "num-bands", NBANDS, NULL);
	equalizer3 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands3");
	g_assert (equalizer3);
	g_object_set (G_OBJECT (equalizer3), "num-bands", NBANDS, NULL);
	if (showEqualizerWindow){
		getBand();
		}
	BP_BRfilter = gst_element_factory_make ("audiochebband", "BP_BRfilter");
	g_assert (BP_BRfilter);
	g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, "upper-frequency", BPupperFreq, NULL);
	sink = gst_element_factory_make("autoaudiosink", "sink");
	g_assert(sink);
	gst_bin_add_many (GST_BIN (pipeline), audioconvert, equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, sink, NULL);
	caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);

	if (!gst_element_link_filtered (audioconvert, equalizer, caps) ||
	    !gst_element_link_many (equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, sink, NULL)){
	    fprintf (stderr, "can't link elements\n");
		exit (1);
	    }
	gst_caps_unref (caps);
		
	audiopad = gst_element_get_static_pad (audioconvert, "sink");
	gst_element_add_pad (pipeline, gst_ghost_pad_new (NULL, audiopad));
	g_object_set(G_OBJECT(playbin), "audio-sink", pipeline, NULL);
	gst_object_unref (audiopad);
	bus = gst_pipeline_get_bus (GST_PIPELINE (playbin));
	gst_bus_add_watch (bus, message_handler, NULL);
	gst_object_unref (bus);
if (realtime) {
	busrt = gst_pipeline_get_bus (GST_PIPELINE (playbin));
  	gst_bus_set_sync_handler (busrt, (GstBusSyncHandler) sync_bus_handler, playbin);
	gst_object_unref (busrt);
	}

	//timeoutEvent = g_timeout_add (50, (GSourceFunc) sdlEvent, pipeline);
	//displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
	timeoutPrintPosition = g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
	gst_element_set_state (playbin, GST_STATE_PLAYING);
	timeoutSeekToTime = g_timeout_add (101, (GSourceFunc) seek_to_time, pipeline);
	printf ("Now playing\n");
	g_main_loop_run (loop);
	//g_source_remove(timeoutEvent);
	//g_source_remove(displaySpectroTimeout);
	g_source_remove(timeoutPrintPosition);
	g_source_remove(timeoutSeekToTime);
	gst_element_set_state (playbin, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));
	filenames = filenames->next;
	getFileName();
	}
	printf ("Stop playing\n");
	//sdlQuit();
#ifdef HAVE_LIBUTOUCH_GEIS
	//geisQuit();
#endif
	}
    }

/* SOURCE IS JACK */
#ifdef HAVE_LIBJACK
else if (typeSource == JACK){
	GstElement *src, *spectrum, *sink;
	GstBus *bus;
	
	gst_init (NULL, NULL);
	loop = g_main_loop_new(NULL, FALSE);
	playing = 1;

	pipeline = gst_pipeline_new ("pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, message_handler, NULL);
	gst_object_unref(bus);
	jack_client_t *src_client;
	jack_status_t status;

	src_client = jack_client_open ("src_client", JackNoStartServer, &status); // create jack clients
	if (src_client == NULL) {
		if (status & JackServerFailed){
			GtkWidget *pAbout;
			pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				"JACK server not running");
			gtk_dialog_run(GTK_DIALOG(pAbout));
		     	gtk_widget_destroy(pAbout);
			SDL_Quit();
			playing = 0;
			}
		else {
			GtkWidget *pAbout;
			pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				"jack_client_open() failed");
			gtk_dialog_run(GTK_DIALOG(pAbout));
		     	gtk_widget_destroy(pAbout);
			SDL_Quit();
			exit(1);
			}		      
		}

	src = gst_element_factory_make ("jackaudiosrc", NULL);
	g_assert(src);
	g_object_set (G_OBJECT (src), "name", PACKAGE, NULL);
	sink = gst_element_factory_make ("fakesink", NULL);
	g_assert(sink);
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert);
	audioconvert2 = gst_element_factory_make ("audioconvert", NULL);
	g_assert (audioconvert2);
	spectrum = gst_element_factory_make ("spectrum", "spectrum");
	g_assert(spectrum);
	g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80, "interval", interval * 1000000, NULL);
	equalizer = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands");
	g_assert (equalizer);
	equalizer2 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands2");
	g_assert (equalizer2);
	g_object_set (G_OBJECT (equalizer2), "num-bands", NBANDS, NULL);
	equalizer3 = gst_element_factory_make ("equalizer-nbands", "equalizer-nbands3");
	g_assert (equalizer3);
	g_object_set (G_OBJECT (equalizer3), "num-bands", NBANDS, NULL);
	BP_BRfilter = gst_element_factory_make ("audiochebband", "BP_BRfilter");
	g_assert (BP_BRfilter);
	g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, "upper-frequency", BPupperFreq, NULL);
	gst_bin_add_many (GST_BIN (pipeline), src, equalizer, equalizer2, equalizer3, audioconvert, BP_BRfilter, audioconvert2, spectrum, sink, NULL);

		if (!gst_element_link_many (src, equalizer, equalizer2, equalizer3, audioconvert, BP_BRfilter, audioconvert2, spectrum, sink, NULL)) { 
		fprintf (stderr, "can't link elements\n");
		exit (1);
		}
	//timeoutEvent = g_timeout_add (50, (GSourceFunc) sdlEvent, pipeline);
	//displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	printf ("Now playing\n");
	g_main_loop_run (loop);
	//g_source_remove(timeoutEvent);
	//g_source_remove(displaySpectroTimeout);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	printf ("Stop playing\n");
	gst_object_unref(pipeline);

	//sdlQuit();
#ifdef HAVE_LIBUTOUCH_GEIS
	//geisQuit();
#endif
	}
#endif
playing = 0;
g_source_remove(timeoutEvent);
g_source_remove(displaySpectroTimeout);
sdlQuit();
#ifdef HAVE_LIBUTOUCH_GEIS
g_source_remove(timeoutTouch);
geisQuit();
#endif
}

}

void change_freq_test_sound( GtkWidget *widget, gpointer data )
{
	freqTestSound = gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
	g_object_set (G_OBJECT (srcTestSound), "freq", freqTestSound, NULL);
}

void change_volume_test_sound(GtkWidget *pWidget, gpointer data)
{
	gdouble value;
	value = gtk_scale_button_get_value(GTK_SCALE_BUTTON(pWidget));
	g_object_set (G_OBJECT (srcTestSound), "volume", value, NULL);
}

void playTestSound(GtkWidget *pWidget, gpointer data){
	GstElement *bin;
	GstElement *audioconvert, *sink;
	
	if (loopTestSound == NULL){
		gst_init (NULL, NULL);
		bin = gst_pipeline_new ("bin");
		srcTestSound = gst_element_factory_make ("audiotestsrc", "src");
		g_object_set (G_OBJECT (srcTestSound), "wave", 0, "freq", freqTestSound, NULL);
		audioconvert = gst_element_factory_make ("audioconvert", NULL);
		g_assert (audioconvert);
		sink = gst_element_factory_make ("autoaudiosink", "sink");
		gst_bin_add_many (GST_BIN (bin), srcTestSound, audioconvert, sink, NULL);
		if (!gst_element_link_many (srcTestSound, audioconvert, sink, NULL)){ 
			fprintf (stderr, "can't link elements\n");
			exit (1);
			}
		gst_element_set_state (bin, GST_STATE_PLAYING);
		printf("Play test sound \n");
		loopTestSound = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loopTestSound);
		gst_element_set_state (bin, GST_STATE_NULL);
		gst_object_unref (bin);
		}
	else {
		g_main_loop_quit(loopTestSound);
		printf("Stop test sound \n");
		loopTestSound = NULL;
		}
}





