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
#include <gst/gst.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <gtk/gtk.h>

#include "config.h"

#include "onclick.h"

int previousWidth = 1200;
static GtkWidget *mainWindow;

/* Get the type of source (microphone, audio file or jack) */
void onSource(GtkWidget *pBtn, gpointer data)
{  
	GSList *pList;
    	const gchar *sLabel;
    	pList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));

	while(pList)
	{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pList->data)))
	{
	    sLabel = gtk_button_get_label(GTK_BUTTON(pList->data));
	    pList = NULL;
	}
	else
	{
	    pList = g_slist_next(pList);
	}
	}

	if (strcmp(sLabel, "Jack") == 0) {
		typeSource = JACK;
		}
	else if (strcmp(sLabel, "Mic") == 0) {
		typeSource = MIC;
		}
	else if (strstr(sLabel, "File") != NULL) {
		typeSource = AUDIO_FILE;
		}
}

/* Get the type of view (3D or flat view) */
void on_view(GtkWidget *pBtn, gpointer data)
{  
	GSList *pList;
    	const gchar *sLabel;
    	pList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));

	while(pList) {
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pList->data))) {
		    sLabel = gtk_button_get_label(GTK_BUTTON(pList->data));
		    pList = NULL;
		}
		else {
		    pList = g_slist_next(pList);
		}
	}

	if (strcmp(sLabel, "Flat") == 0) {
		viewType = FLAT;
		/*if (width != 1200 && playing){
			sdlQuit();	
			width = 1200;
			setupSDL();
			setupOpengl();
			PROPORTION = (float)width / 1000;
			x = 1.2 * RESIZE;
			onReset();
			}
		else if (width != 1200 && playing == 0) {
			width = 1200;
			}*/
		changeViewParameter = TRUE;
		change = 1;
		}
	else {
		if (strcmp(sLabel, "3D") == 0){
			viewType = THREE_D;
			}
		else {
			viewType = THREE_D_FLAT;
			}
		/*if (playing && presetWidth != width){
			sdlQuit();
			width = presetWidth;
			setupSDL();
			setupOpengl();
			PROPORTION = (float)width / 1000;
			x = 1.2 * RESIZE;
			onReset();
			}*/
		change = 1;
		}
}

/* Reset position */
void onReset()
{
	X = -0.7 * RESIZE;
	Y = -0.1;
	Z = -1.05;
	AngleH = -16.0;
	AngleV = 10.0;
	AngleZ = 0;

	if (pose == 1) {
		change = 1;
		}
}

/* Front position */
void onFrontView()
{
	X = -0.64 * RESIZE;
	Y = -0.30;
	Z = -0.95;
	AngleH = 0;
	AngleV = 0;
	AngleZ = 0;

	if (pose == 1) {
		change = 1;
		}
}

/* Preset */
void onPreset()
{
	X = presetX ;
	Y = presetY;
	Z = presetZ;
	AngleH = presetAngleH;
	AngleV = presetAngleV;
	AngleZ = presetAngleZ;

	if (pose == 1) {
		change = 1;
		}
}

/* stop playing audio and displaying SDL surface */
void onStop() 
{ 
	if (loop != NULL){
		g_main_loop_quit(loop);
		}
	playing = 0;
	pose = 0;
	setPlayButtonIcon();
}


/* Get starting value of the zoom (every 500 Hz, from 0 to 4000 Hz) */
void cb_zoom_changed(GtkComboBox *combo, gpointer data)
{
  	gchar *string = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
	int spinZoom = strtol(string, NULL, 10);
	zoom = spinZoom / 2;
	g_free( string );

	if (pose == 1) {
		change = 1;
		}
	if (playing) {
		changeViewParameter = TRUE;
		}
}

/* Get the range of the zoom (range : 1000 to 20000 Hz) */
void cb_range_changed(GtkWidget *combo, gpointer data)
{
	zoomFactor = (gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) + 1);
	change_adjust_start();
	if ((bandsNumber * zoomFactor) > ((AUDIOFREQ * 0.453514739) / hzStep)){
		bandsNumber = ((AUDIOFREQ * 0.453514739)/hzStep) / zoomFactor;
		gtk_range_set_value(GTK_RANGE(pScaleBands), (gdouble)bandsNumber);
		} 
    	if (pose == 1) {
		change = 1;
		}
	if (playing) {
		changeViewParameter = TRUE;
		}
	getTextDisplayLabel();
}

void change_start(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	if ((int)iValue <= ((AUDIOFREQ * 0.453514739) - (hzStep * bandsNumber *zoomFactor)) / 2){
		zoom = (int)iValue;
		}
	else {
		zoom = 0;
		}
	if (playing) {
		changeViewParameter = TRUE;
		}
	if (pose == 1) {
		change = 1;
		}
	getTextDisplayLabel();
}

void change_adjust_start() 
{	
	//if ((((AUDIOFREQ * 0.453514739) - (hzStep * bandsNumber *zoomFactor)) / 2) <= (AUDIOFREQ * 0.453514739)) {
		gtk_adjustment_set_upper(GTK_ADJUSTMENT(adjust_start), (gdouble)((AUDIOFREQ * 0.453514739) - (hzStep * bandsNumber *zoomFactor)) / 2);
		//}
	//else {
		//gtk_adjustment_set_upper(GTK_ADJUSTMENT(adjust_start), 0);
		//}
	if (zoom >= (((AUDIOFREQ * 0.453514739) - (hzStep * bandsNumber *zoomFactor)) / 2)){
		gtk_adjustment_set_value (GTK_ADJUSTMENT(adjust_start), gtk_adjustment_get_upper (GTK_ADJUSTMENT(adjust_start)));
		}
}

gchar* format_value_start (GtkScale *scale, gdouble value) 
{
	return g_strdup_printf ("%d", (int)value * hzStep);
 }

/* Change the speed of display */
void cb_speed_changed(GtkComboBox *combo, gpointer data)
{
	gchar *string = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
	intervalTimeout = strtol(string, NULL, 10);
	printf("intervalTimeout= %d\n", intervalTimeout);
	if (playing){
		g_source_remove(displaySpectroTimeout);	
		displaySpectroTimeout = g_timeout_add (intervalTimeout, (GSourceFunc) displaySpectro, pipeline);
		}
}

/* Seek 5 sec forward in audio file while playing */
void timeForward() 
{
	forward = 1;
	nSec = 5;
}

/* Seek 5 sec backward in audio file while playing */
void timeBackward() 
{
	backward = 1;
	nSec = -5;
}

/* Stop seeking (when "Seek" button is released) */
void stopSeek() 
{
	forward = 0;
	backward = 0;
}

/* Change scale */
/*void cb_scale_changed(GtkComboBox *combo, gpointer data) {
	gint index = gtk_combo_box_get_active( combo );
	if (index ==0 ) {
		scale = 0;
	}
	else if (index == 1) {
		scale = 1;
	}
	else if (index == 2) {
		scale = 2;
	}
	else if (index == 3) {
		scale = 3;
	}

	if (playing) {
		changeViewParameter = TRUE;
		}
	if (pose == 1) {
		change = 1;
		}
}*/

void change_adjust(GtkWidget *pWidget, gpointer data)
{
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(adjust_bands), (gdouble)(width - 200));
}

void change_bands(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	bandsNumber = (int)iValue;
	change_adjust_start();
	if ((zoomFactor * bandsNumber) > ((AUDIOFREQ * 0.453514739)/hzStep)) {
		zoomFactor = ((AUDIOFREQ * 0.453514739)/hzStep) / bandsNumber;
		printf("zoomFactor = %d\n", zoomFactor);
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), (zoomFactor - 1));
		}
	if (playing) {
		changeViewParameter = TRUE;
		}
	if (pose == 1) {
		change = 1;
		}
	getTextDisplayLabel();
}

gchar* format_value_bands (GtkScale *scale, gdouble value) 
{
	return g_strdup_printf ("%d", (int)value * hzStep);
 }

/* Enable lines */
void onCheckLineScale(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		lineScale = 1;
	}
	else {
		lineScale = 0;
		}

	if (pose == 1) {
		change = 1;
		}
}

/* Enable text scale */
void onCheckTextScale(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		textScale = 1;
		}
	else {
		textScale = 0;
		}

	if (pose == 1) {
		change = 1;
		}
}

void onCheckPointer(GtkWidget *pToggle, gpointer data)
{
	gboolean bState;
	bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
	if (bState) {
		pointer = TRUE;
		}
	else {
		pointer = FALSE;
		}

	if (pose == 1) {
		change = 1;
		}
}

/* Enable gain change */
gboolean changeGain(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)  
{
	//if (scroll == GTK_SCROLL_END) {
		//printf("scroll type = %d\n", scroll);
		showGain = (gfloat)value;

		if (pose == 1) {
			change = 1;
			}
		if (playing) {
			changeViewParameter = TRUE;
			}
		//}
return FALSE;
}

/* Enable 'Depth' change */
void changeDepth(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	z = iValue * PROPORTION;

	if (pose == 1) {
		change = 1;
		}
}

/* Change 'Width' of main window */
void changeWidth(GtkSpinButton *spinButton, gpointer user_data)
{
	onClickWidth = gtk_spin_button_get_value_as_int(spinButton);	
}

void errorMessageWindowJack(char *message)
{
	GtkWidget *pAbout;
	pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_OK,
		"%s", message);
	gtk_dialog_run(GTK_DIALOG(pAbout));
     	gtk_widget_destroy(pAbout);
	sdlQuit();
#ifdef HAVE_LIBUTOUCH_GEIS
	geisQuit();
#endif
	playing = 0;
}


void errorMessageWindow(char *message){
	GtkWidget *pAbout;
	pAbout = gtk_message_dialog_new (GTK_WINDOW(mainWindow),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_OK,
		"%s", message);
	gtk_dialog_run(GTK_DIALOG(pAbout));
     	gtk_widget_destroy(pAbout);
}





