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
#include "spectrum3d.h"
#include "typesource.h"

int lenght = 0;
GtkWidget *progressWindow;
GMainLoop *load_loop;

/* Quit everything except GTK GUI */
void set_source_to_none(){
	// stop pipeline if running
	on_stop();
	// reinitialize spec_data (spectrum values)	
	memset(spec_data, 0, sizeof(spec_data));
	// set typeSource to NONE
	typeSource = NONE;
}

/* Check button to set if harmonics are displayed in real time or not */
void set_analyse_in_rt(GtkWidget *check, Spectrum3dGui *spectrum3dGui){
	// This can be changed only if there is no playing stream
	if (playing){
		error_message_window("Stop playing first");
		return;
		}
	// If analyse_rt is checked, it shouldn't be possible to record anymore, so the the record button will be set to not sensitive
	gboolean state;
	state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
	if (state) {
		analyse_rt = TRUE;
		gtk_widget_set_sensitive (spectrum3dGui->record, FALSE);
		gtk_widget_set_sensitive (spectrum3dGui->reload, FALSE);
	}
	else {
		analyse_rt = FALSE;
		gtk_widget_set_sensitive (spectrum3dGui->record, TRUE);
		gtk_widget_set_sensitive (spectrum3dGui->reload, TRUE);
		}
}

static gboolean
idle_exit_loop (gpointer data)
{
  g_main_loop_quit ((GMainLoop *) data);

  /* once */
  return FALSE;
}

static void
cb_typefound (GstElement *typefind,
	      guint       probability,
	      GstCaps    *caps,
	      gpointer    data)
{
  GMainLoop *loopTF = data;
  gchar *type;

  type = gst_caps_to_string (caps);
  g_print ("Media type %s found, probability %d%%\n", type, probability);
  g_free (type);

  /* since we connect to a signal in the pipeline thread context, we need
   * to set an idle handler to exit the main loop in the mainloop context.
   * Normally, your app should not need to worry about such things. */
  g_idle_add (idle_exit_loop, loopTF);
}

gint 
typefind_pipeline ()
{
  GMainLoop *loopTF;
  GstElement *pipelineTF, *filesrc, *typefind, *fakesink;
  GstBus *busTF;

  loopTF = g_main_loop_new (NULL, FALSE);

  /* create a new pipeline to hold the elements */
  pipelineTF = gst_pipeline_new ("pipe");

  busTF = gst_pipeline_get_bus (GST_PIPELINE (pipelineTF));
  //gst_bus_add_watch (bus, my_bus_callback, NULL);
  gst_object_unref (busTF);

  /* create file source and typefind element */
  filesrc = gst_element_factory_make ("filesrc", "source");
  g_object_set (G_OBJECT (filesrc), "location", selected_file, NULL);
  typefind = gst_element_factory_make ("typefind", "typefinder");
  g_signal_connect (typefind, "have-type", G_CALLBACK (cb_typefound), loopTF);
  fakesink = gst_element_factory_make ("fakesink", "sink");

  /* setup */
  gst_bin_add_many (GST_BIN (pipelineTF), filesrc, typefind, fakesink, NULL);
  gst_element_link_many (filesrc, typefind, fakesink, NULL);
  gst_element_set_state (GST_ELEMENT (pipelineTF), GST_STATE_PLAYING);
  g_main_loop_run (loopTF);

  /* unset */
  gst_element_set_state (GST_ELEMENT (pipelineTF), GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipelineTF));

  return 0;
}

 /* Select an audio file */
void select_audio_file()
{
	GtkWidget *fileSelection;
	fileSelection = gtk_file_chooser_dialog_new("Select File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_OK,
			NULL);
	gtk_window_set_modal(GTK_WINDOW(fileSelection), TRUE);
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (fileSelection), g_get_home_dir());

	switch(gtk_dialog_run(GTK_DIALOG(fileSelection)))
	{
	case GTK_RESPONSE_OK:
	       	selected_file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fileSelection));
		gtk_widget_destroy(fileSelection);
		printf("Uri of selected_file = %s\n", g_filename_to_uri(selected_file, NULL, NULL));
		//typefind_pipeline();  // this send to a function that finds type; FIXME : this has to be implemented and generate error message if type is not audio
		if (analyse_rt == FALSE){
				load_audio_file();
				}
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		printf("No file was selected...\n");
		gtk_widget_destroy(fileSelection);
	    	break;
	}
}

/* Get the lenght of the pipeline */
gboolean get_pipeline_lenght (GstElement *pipeline)
{
	gboolean result = TRUE;
	GstFormat fmt = GST_FORMAT_TIME; 
	if (gst_element_query_duration (pipeline, &fmt, &len)) {
		lenght = (int)GST_TIME_AS_SECONDS(len);
		printf("lenght = %d seconds\n", lenght);
		if (lenght > 0 && lenght < 36000) {
			g_main_loop_quit (load_loop);
			result = FALSE;
			}
		}
	return result;	
}

/* Print the progression in a progress bar when loading audio file */
gboolean update_progress_bar(GtkWidget *progress) {
	GstFormat fmt = GST_FORMAT_TIME;
	if (gst_element_query_position (pipeline, &fmt, &pos)
	&& gst_element_query_duration (pipeline, &fmt, &len)) {
		gdouble setPos = ((gdouble)pos)/((gdouble)len);
		//printf("setPos = %f\n", setPos);
		if (setPos >= 0 && setPos <= 1) {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), setPos);
			}
		}
	if (loading){	
		return TRUE;	
		}
	else {
		gtk_widget_destroy(progressWindow);
		return FALSE;	
		} 	
}


/* show progression of the loading of an audio file */
void show_progression() {
	GtkWidget *vbox, *button, *progress;

	progressWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_modal (GTK_WINDOW(progressWindow), TRUE);
	gtk_window_set_title(GTK_WINDOW(progressWindow), "Loading file");
	gtk_window_set_default_size(GTK_WINDOW(progressWindow), 320, 180);
	gtk_container_set_border_width(GTK_CONTAINER(progressWindow), 4);
	g_signal_connect(G_OBJECT(progressWindow), "delete-event", G_CALLBACK(on_stop), NULL);

	vbox = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(progressWindow), vbox);

	progress = gtk_progress_bar_new();
#ifdef GTK3
	gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(progress), FALSE);
#elif GTK2
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(progress), GTK_PROGRESS_LEFT_TO_RIGHT);
#endif
	gtk_box_pack_start(GTK_BOX(vbox), progress, TRUE, FALSE, 0);
	button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, FALSE, 0);
	gtk_widget_show_all(progressWindow);

	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_stop), NULL);
	g_timeout_add (200, (GSourceFunc) update_progress_bar, progress);
}

/* Load the audio file : get its lenght, allocate memory for it, analyse the spectrum and display it */
void load_audio_file(){
	GstElement *playbin;
	
	if (selected_file == NULL){
		error_message_window("No file was selected.\nPlease select an audio file.");
		return;
		}

	loading = TRUE; // loading is set to TRUE; it will be set to FALSE again when the audio file hab been loaded;
	// (re)initialize frame_number_counter
	frame_number_counter = 0;
	
	/* get lenght of the file; for that, a pipeline will be run with a "fakesink"; the loop will be terminated when a valid value is obtained */
	load_loop = g_main_loop_new(NULL, FALSE);
	playbin = gst_element_factory_make ("playbin", NULL);
	g_assert (playbin);
	g_object_set (G_OBJECT (playbin), "uri", g_filename_to_uri(selected_file, NULL, NULL), NULL);
	gst_element_set_state (playbin, GST_STATE_PAUSED);
	g_timeout_add (200, (GSourceFunc) get_pipeline_lenght, playbin);
	g_main_loop_run (load_loop);
	gst_element_set_state (playbin, GST_STATE_NULL);

	/* Calculate time interval between frames according to the lenght of the file; we will use a number of 'frames' = 200 to have everything displayed on the same screen */ 

	spectrum3d.interval_loaded = (lenght * 1000) / spectrum3d.frames;

	/* Get spectral data */
	show_progression();
	playFromSource(NULL, NULL);
	
	printf("Audio file is loaded\n");
	loading = FALSE;
	newEvent = TRUE;
}

/* Source buttons that behave almost like radio buttons; the typseSource is chosen; everything is stopped with the call to set_source_to_none, then the typeSource is chosen, even if the typeSource remains the same  */
void change_source_button (GtkWidget *widget, Spectrum3dGui *spectrum3dGui){
	GdkColor color;
	gchar *filename;
	GtkWidget *image;
#ifdef GTK3
	GdkRGBA rgba;
#endif

	// stop everything before changing type source or loading a new audio file
	set_source_to_none();

	// get name of the button
	const gchar *buttonName = gtk_widget_get_name (widget);
	
	// get the name of each button and compare it to the name of the clicked button; if it is different, set the button inactive (i.e. the icon is grey instead of its normal color, and the background is also grey); if it is the same, set the button active (i.e. the icon has its noral color and the background is orange; note that changing background color does not always work due to a bug (see later)); 
	
	// STOP BUTTON : stops everything
	const gchar *stopButton = gtk_widget_get_name (spectrum3dGui->stop);
	if (strcmp(stopButton, buttonName) == 0){
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "stop.png", NULL);
		image = gtk_image_new_from_file(filename);
		gtk_button_set_image(GTK_BUTTON(widget),image);
#ifdef GTK3
		gdk_rgba_parse (&rgba, "orange");
		gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &rgba);
		gtk_widget_override_background_color(widget, GTK_STATE_FLAG_PRELIGHT, &rgba);
		// gtk_widget_override_background_color() is not always honored; this is a known bug in GTK3 (Bug 656461)
#elif defined (GTK2)
		gdk_color_parse ("orange",&color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_PRELIGHT, &color);
#endif
		}
	else {
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "stop_grey.png", NULL);
		image = gtk_image_new_from_file(filename);
		gtk_button_set_image(GTK_BUTTON(spectrum3dGui->stop),image);
#ifdef GTK3
		gdk_rgba_parse (&rgba, "grey");
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->stop), GTK_STATE_FLAG_NORMAL, &rgba);
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->stop), GTK_STATE_PRELIGHT, &rgba);
#elif defined (GTK2)
		gdk_color_parse ("grey",&color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->stop), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->stop), GTK_STATE_PRELIGHT, &color);
#endif
		}

	// MICROPHONE BUTTON
	const gchar *micButton = gtk_widget_get_name (spectrum3dGui->mic);
	if (strcmp(micButton, buttonName) == 0){
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "microphone.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image(GTK_BUTTON(widget),image);
#ifdef GTK3
		gdk_rgba_parse (&rgba, "orange");
		gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &rgba);
		gtk_widget_override_background_color(widget, GTK_STATE_FLAG_PRELIGHT, &rgba);
#elif defined (GTK2)
		gdk_color_parse ("orange",&color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_PRELIGHT, &color);
#endif
		typeSource = MIC;
		}
	else {
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "microphone_grey.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui->mic),image);
#ifdef GTK3
		gdk_rgba_parse (&rgba, "grey");
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->mic), GTK_STATE_FLAG_NORMAL, &rgba);
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->mic), GTK_STATE_PRELIGHT, &rgba);
#elif defined (GTK2)
		gdk_color_parse ("grey",&color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->mic), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->mic), GTK_STATE_PRELIGHT, &color);
#endif
		}

	// FILE BUTTON 
	const gchar *fileButton = gtk_widget_get_name (spectrum3dGui->file);
	if (strcmp(fileButton, buttonName) == 0){
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "file.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image(GTK_BUTTON(widget),image);
#ifdef GTK3
		gdk_rgba_parse (&rgba, "orange");
		gtk_widget_override_background_color(GTK_WIDGET(widget), GTK_STATE_NORMAL, &rgba);
		gtk_widget_override_background_color(GTK_WIDGET(widget), GTK_STATE_PRELIGHT, &rgba);
#elif defined (GTK2)
		gdk_color_parse ("orange",&color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(widget), GTK_STATE_PRELIGHT, &color);
#endif
		typeSource = AUDIO_FILE;
		select_audio_file();
		}
	else {
		filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "file_grey.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui->file),image);
		#ifdef GTK3
		gdk_rgba_parse (&rgba, "grey");
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->file), GTK_STATE_NORMAL, &rgba);
		gtk_widget_override_background_color(GTK_WIDGET(spectrum3dGui->file), GTK_STATE_PRELIGHT, &rgba);
#elif defined (GTK2)
		gdk_color_parse ("grey",&color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->file), GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui->file), GTK_STATE_PRELIGHT, &color);
#endif
		}

}




