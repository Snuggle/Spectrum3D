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

#include "config.h"

#ifdef HAVE_LIBJACK
	#include <jack/jack.h>
#endif

#include "spectrum3d.h"
#include "gstreamer.h"

const GValue *magni, *magnitudes;
gdouble freqTestSound = 440;
GstElement *srcTestSound;
static GstTaskPool *pool;

#define NBANDS 10

void initGstreamer(){
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

void init_audio_values(){
	memset(spec_data, 0, sizeof(spec_data));
	playing = 0;
	pose = 0;
	analyse_rt = TRUE;
	typeSource = NONE;
	spect_bands = 11025; // number of spectral bands analysed by thre 'spectrum' element of gstreamer
	AUDIOFREQ = 44100; // Sampling rate
	bandsNumber = 1000; // number of bands displayed on the screen
	hzStep = (AUDIOFREQ/2) / spect_bands; // number of herz in 1 'band'
	BPlowerFreq = 0; BPupperFreq = 40000; // limit of Band-Pass(BP) filters
	tmpPath = g_build_filename (G_DIR_SEPARATOR_S, g_get_tmp_dir(), "spectrum3d.flac", NULL); // path of the recorded file (in the 'tmp' directory)
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

      gst_message_parse_stream_status (message, &type, &owner);

      val = gst_message_get_stream_status_object (message);

      path = gst_object_get_path_string (GST_MESSAGE_SRC (message));
      g_free (path);
      path = gst_object_get_path_string (GST_OBJECT (owner));
      g_free (path);

      /* see if we know how to deal with this object */
      if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
        task = g_value_get_object (val);
      }

      switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
          if (task) {
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
		const GstStructure *s = gst_message_get_structure (message);
		const gchar *name = gst_structure_get_name (s);
		GstClockTime endtime;
		
                if (strcmp (name, "spectrum") == 0) {

			if (!gst_structure_get_clock_time (s, "endtime", &endtime))
			endtime = GST_CLOCK_TIME_NONE;
			magnitudes = gst_structure_get_value (s, "magnitude");
			for (ii = 0; ii < spect_bands ; ii++) {
				magni = gst_value_list_get_value (magnitudes, ii);
				if (source == SOUND_FILE && analyse_rt == FALSE && frame_number_counter < spectrum3d.frames){
					spec_data[frame_number_counter][ii] = ((80 + (g_value_get_float(magni)))/40);
					}
				else if (source == MICRO || (source == SOUND_FILE && analyse_rt == TRUE)){
					spec_data[0][ii] = ((80 + (g_value_get_float(magni)))/40);
					}
				}
				if (source == SOUND_FILE && analyse_rt == FALSE) {
					// frame_number counter is increased
					frame_number_counter +=1;
					}
				}
			}
		if (message->type == GST_MESSAGE_EOS) {
			g_main_loop_quit (loop);
			}
return TRUE;
}

void on_seek (GtkRange *range, gchar *data)
{
	gdouble value = gtk_range_get_value (range);
	if (playing){
		gint64 seekPos = len * value/100;
		if (!gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
			GST_SEEK_TYPE_SET, seekPos,
			GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
			g_print ("Seek failed!\n");
			}
		}
}

/* Print the total duration and the time position in a terminal when playing an audio file */
static gboolean cb_print_position (GstElement *pipeline){

	gchar positionLabel[50];
	//gchar shortPos[7], shortLen[7];
	GstFormat fmt = GST_FORMAT_TIME;
	if (gst_element_query_position (pipeline, &fmt, &pos)
	&& gst_element_query_duration (pipeline, &fmt, &len)) {
		gdouble setPos = ((gdouble)pos)/((gdouble)len) * 100;
		/* pScaleSeek has to be updated with the new time value, but the callback (that updates the time values when pScaleSeek has been changed) doesn't have to be activated; the callback to the 'on_seek' function while moving pScaleSeek has to be blocked */
		g_signal_handlers_block_by_func(G_OBJECT(scaleSeek), G_CALLBACK(on_seek), NULL);
		gtk_range_set_value(GTK_RANGE(scaleSeek), setPos);
		g_signal_handlers_unblock_by_func(G_OBJECT(scaleSeek), G_CALLBACK(on_seek), NULL);
		
		/* Print the time in stringPos and stringLen variables, then make them shorter (cut the '.' and then 10 numbers behind the dot) */
		gchar stringPos[45];
		g_sprintf(stringPos, "%" GST_TIME_FORMAT "\r", GST_TIME_ARGS (pos));
		stringPos[strlen(stringPos)-11] = 0;
		
		gchar stringLen[45];
		g_sprintf(stringLen, "%" GST_TIME_FORMAT "\r", GST_TIME_ARGS (len));
		stringLen[strlen(stringLen)-11] = 0;
		
		/* Print the result in the main window */
		g_sprintf(positionLabel, "      %s / %s      ", stringPos, stringLen);
		show_position(positionLabel);
		}
	return TRUE;					
}

int checkJackActive(){
	int result = 0;
#ifdef HAVE_LIBJACK
	printf("*** Checking if JACK is running (Jack error messages are normal):\n");
	jack_client_t *src_client;
	//jack_status_t status;
	src_client = jack_client_open ("src_client", JackNoStartServer, NULL); 
	if (src_client != NULL) {
		printf("JACK seems to be running. Please stop JACK and start playing again\n");
		result = 1;
		}
	else if (src_client == NULL) {
		printf("*** --> OK (ERROR MESSAGE IS NORMAL HERE!)\n");
		}
#endif

	return result;

}


void playFromSource(GtkWidget *widget, gpointer *data){

	printf("playFromSource\n");

/* There are many possible combinations of elements, acording to the sourceButtons selected (Microphone, audio file or jack), the 'real-time' analyse of ths sound (i.e. from a file or from the microphone), of the 'loading' status (i.e. a file is beeing analysed without beeing played or displayed, and the 'record' status (i.e. a file is beeing recorded from the microphone; at the end of the recording, it will be loaded as an audio file; all these possibilities are combines to but end up finally in one of the three basic groups : microphone, audio file or jack; a new enumeration 'Source' is created for that; its possible values are the same than the typeSource possible values */

	if (typeSource == AUDIO_FILE){
		if (selected_file == NULL){
			error_message_window("No file was selected.\nPlease select an audio file.");
			return;
			}
		source = SOUND_FILE;
		}
	else if (typeSource == MIC){
		if (analyse_rt == TRUE){
			source = MICRO;
			}
		else if (analyse_rt == FALSE){
			if (recording == TRUE){
				source = MICRO;
				}
			else if (recording == FALSE){
				source = SOUND_FILE;
				}
			}
		}

if (typeSource == NONE){
	error_message_window("You have to select a source (microphone or audio file)"); 
	return;
	}
	
if (playing){
	if (pose == 0) {
	        pose = 1;
		setPlayButtonIcon();
		if (source == MICRO){
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
			}
		else if (source == SOUND_FILE){
			gst_element_set_state (playbin, GST_STATE_PAUSED);
			}			
		}
	 else {
	        pose = 0;
		setPlayButtonIcon();
		if (source == MICRO){
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			}
		else if (source == SOUND_FILE){
			gst_element_set_state (playbin, GST_STATE_PLAYING);
			}
	        }
    	}

/* Play */
else if (playing == 0) {
					
	guint timeoutPrintPosition;
	GstElement *src, *audioconvert, *audioconvert2, *audioconvert3, *spectrum, *flacenc, *sink;
	GstBus *bus, *busRT;
	GstCaps *caps;
	GstPad *audiopad;
	guint64 interval;

	if (loading){
		interval = (guint64)spectrum3d.interval_loaded;
		}
	else {
		interval = (guint64)spectrum3d.interval_rt;
		}
	playing = 1;
	setPlayButtonIcon();

/* Check if Jack is active while MIC or SOUND_FILE is selected as source; in that case, the pipeline is by-passed and a warning message is showed */
	int jackActive = 0;
	if (!jack) {
		jackActive = checkJackActive();
		if (jackActive){
			error_message_window("JACK seems to be running\nStop JACK server and start playing again");
			return;
			}
		}
	/* create jack client if apropriate */
#ifdef HAVE_LIBJACK
	if (jack){
		jack_client_t *src_client, *sink_client;
		jack_status_t status;
		gchar message[10];

		src_client = jack_client_open ("src_client", JackNoStartServer, &status); 
		if (src_client == NULL) {
			if (status & JackServerFailed){
				error_message_window("JACK server not running");
				g_print ("JACK server not running\n");
				}
		    	else {
				g_sprintf (message, "jack_client_open() failed, status = 0x%2.0x\n", status);
				error_message_window(message);
		      		g_print("jack_client_open() failed, status = 0x%2.0x\n", status);
				}
		    	return;
		 	}
		sink_client = jack_client_open ("sink_client", JackNoStartServer, &status);
		if (sink_client == NULL) {
			if (status & JackServerFailed){
				error_message_window("JACK server not running");
				g_print ("JACK server not running\n");
				}
			else {
				g_sprintf (message, "jack_client_open() failed, status = 0x%2.0x\n", status);
				error_message_window(message);
			      	g_print ("jack_client_open() failed, status = 0x%2.0x\n", status);
				}
			}
		}
#endif
	
	if (!jack) {
	  	pool = test_rt_pool_new (); // allows realtime without jack (if this option is selected)
		}

	loop = g_main_loop_new(NULL, FALSE);


/* create pipeline ; create bus if source == MICRO */
  		if (source == MICRO) {
			pipeline = gst_pipeline_new ("pipeline");
			g_assert (pipeline);
			bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
			gst_bus_add_watch(bus, message_handler, NULL);
			gst_object_unref(bus);
			}
		if (source == SOUND_FILE) {
			pipeline = gst_bin_new ("pipeline");
			g_assert (pipeline);
			}

/* create source element */
		if (source == MICRO) {
			if (jack){
				src = gst_element_factory_make ("jackaudiosrc", NULL);
				g_assert (src);
				}
			else {
				src = gst_element_factory_make ("autoaudiosrc", NULL);
				g_assert (src);
				}
			}
		else if (source == SOUND_FILE) {
			playbin = gst_element_factory_make ("playbin2", NULL);
			g_assert (playbin);
			g_object_set (G_OBJECT (playbin), "uri", g_filename_to_uri(selected_file, NULL, NULL), NULL);
			}
		
/* create common elements : audioconvert, spectrum, equalizer and audiochebband */
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
		getBand();
		BP_BRfilter = gst_element_factory_make ("audiochebband", "BP_BRfilter");
		g_assert (BP_BRfilter);
		g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, "upper-frequency", BPupperFreq, NULL);
		flacenc = gst_element_factory_make ("flacenc", NULL);
		g_assert (flacenc);

/* create sink element */
		if (source == MICRO) {
			if (recording) { 
				sink = gst_element_factory_make ("filesink", NULL);
				g_assert (sink);
				g_object_set (G_OBJECT (sink), "location", tmpPath, NULL);
				}
			else {
				if (jack && analyse_rt == FALSE) {
					sink = gst_element_factory_make ("jackaudiosink", PACKAGE);
					}
				else {
					sink = gst_element_factory_make ("fakesink", NULL);
					}
				}
			}
		else if (source == SOUND_FILE) {
			if (analyse_rt == FALSE && loading == TRUE){
				sink = gst_element_factory_make("fakesink", "sink");
				}
			else {
				if (jack) {
					sink = gst_element_factory_make ("jackaudiosink", NULL);
					}
				else {
					sink = gst_element_factory_make("autoaudiosink", "sink");
					}
				}
			}
		g_assert (sink);

/* add elements to pipeline */
		if (source == MICRO){
			gst_bin_add_many (GST_BIN (pipeline), src, NULL);
			}
		else if (source == SOUND_FILE){
			gst_bin_add_many (GST_BIN (pipeline), audioconvert, NULL);
			}
		gst_bin_add_many (GST_BIN (pipeline), equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, flacenc, sink, NULL);

/* link elements */
		caps = gst_caps_new_simple ("audio/x-raw-int", "rate", G_TYPE_INT, AUDIOFREQ, NULL);
		if (source == MICRO){
			if (jack) {
				if (!gst_element_link (src, equalizer)) {
			    fprintf (stderr, "can't link elements 1\n");
				exit (1);
					}
				}
			else {
				if (!gst_element_link_filtered (src, equalizer, caps)) {
				    fprintf (stderr, "can't link elements 1\n");
					exit (1);
				}
			    }
			}
		else if (source == SOUND_FILE){
			if (!gst_element_link_filtered (audioconvert, equalizer, caps)){
			    fprintf (stderr, "can't link elements 2\n");
				exit (1);
			    }
			}
		if (source == MICRO){
			if (!gst_element_link_many (equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, flacenc, sink, NULL)) {
			    fprintf (stderr, "can't link elements\n");
				exit (1);
			    }
			}
		else if (source == SOUND_FILE){
			if(analyse_rt == FALSE && recording == FALSE && loading == FALSE) {
			// if audio_file has been loaded, file should be played without the spectrum element since harmonic analysis has been done previously
				if (!gst_element_link_many (equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, sink, NULL)) {
				    fprintf (stderr, "can't link elements\n");
					exit (1);
				    }
				}
			else {
				if (!gst_element_link_many (equalizer, equalizer2, equalizer3, audioconvert2, BP_BRfilter, audioconvert3, spectrum, sink, NULL)) {
				    fprintf (stderr, "can't link elements\n");
					exit (1);
				    }
				}
			}

/* if source == SOUND_FILE, add the playbin2 element to the pipeline */
		if (source == SOUND_FILE){
			audiopad = gst_element_get_static_pad (audioconvert, "sink");
			gst_element_add_pad (pipeline, gst_ghost_pad_new (NULL, audiopad));
			g_object_set(G_OBJECT(playbin), "audio-sink", pipeline, NULL);
			gst_object_unref (audiopad);
			}

/* create bus if source is SOUND_FILE */
		if (source == SOUND_FILE) {
			bus = gst_pipeline_get_bus (GST_PIPELINE (playbin));
			gst_bus_add_watch(bus, message_handler, NULL);
			gst_object_unref(bus);
			}

/* if realtime is enabled */
		if (spectrum3d.realtime && jack == FALSE) {
			busRT = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
			gst_bus_set_sync_handler (busRT, (GstBusSyncHandler) sync_bus_handler, pipeline);
			gst_object_unref (busRT);
			}

/* set state to PLAYING and start main loop */
		if (source == MICRO) {
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			}
		else if (source == SOUND_FILE) {
			gst_element_set_state (playbin, GST_STATE_PLAYING);
			timeoutPrintPosition = g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
			}
		printf ("Now playing\n");
		g_main_loop_run (loop);

/* stop playing */
		if (source == MICRO) {
			gst_element_set_state (pipeline, GST_STATE_NULL);
			}
		else if (source == SOUND_FILE) {
			g_source_remove(timeoutPrintPosition);
			gst_element_set_state (playbin, GST_STATE_NULL);
			}
		printf ("Stop playing\n");
	//}
	playing = 0; pose = 0;
	setPlayButtonIcon();
	show_position("      0.00 / 0.00      ");
	gtk_range_set_value(GTK_RANGE(scaleSeek), 0);
	//pos = 0;
	//show_playing_position();
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





