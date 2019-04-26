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
#include "spectrum3d.h"
#include "onclick.h"

#define AUDIOFREQFACTOR AUDIOFREQ * 0.453514739

/* Get the type of view (3D, 3D flat or 2D) */
void change_perspective(GtkWidget *widget, gpointer data)
{  
	GSList *list;
    	const gchar *label;
	
	list = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM(data));

	while(list) {
		if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
			label = gtk_label_get_label(GTK_LABEL(GTK_BIN(widget)->child));
			list = NULL;
		}
		else {
			list = g_slist_next(list);
		}
	}

	if (strcmp(label, "2D (D)") == 0) {
		viewType = TWO_D;
		}
	else {
		if (strcmp(label, "3D (D)") == 0){
			viewType = THREE_D;
			}
		else {
			viewType = THREE_D_FLAT;
			}
		}
	newEvent = TRUE;
}

/* Check button to set if JACK is used or not */
void use_jack(GtkWidget *check, gpointer data){
	gboolean state;
	state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
	if (state) {
		jack = TRUE;
	}
	else {
		jack = FALSE;
		}
}

/* Reset position */
void reset_view()
{
	X = -0.7;
	Y = -0.1;
	Z = -1.05;
	AngleH = -16.0;
	AngleV = 10.0;
	AngleZ = 0;

	spectrum3d.frames = spectrum3d.previousFrames;
	newEvent = TRUE;
}

/* Front position */
void front_view()
{
	X = -0.64;
	Y = -0.25;
	Z = -0.95;
	AngleH = 0;
	AngleV = 0;
	AngleZ = 0;

	spectrum3d.frames = 1;

	newEvent = TRUE;
}

/* Preset */
void set_view_from_preset()
{
	X = spectrum3d.presetX ;
	Y = spectrum3d.presetY;
	Z = spectrum3d.presetZ;
	AngleH = spectrum3d.presetAngleH;
	AngleV = spectrum3d.presetAngleV;
	AngleZ = spectrum3d.presetAngleZ;

	spectrum3d.frames = spectrum3d.previousFrames;
	newEvent = TRUE;
}

gboolean memset_spec_data(gpointer *data){
	//printf("resetting spec_data\n");
	if (playing){
		return TRUE;
		}
	else {
		memset(spec_data, 0, sizeof(spec_data));
		newEvent = TRUE;
		return FALSE;
		}
}

/* stop playing audio */
void on_stop() 
{	
	if (loop != NULL){
		g_main_loop_quit(loop);
		}
	/* spec_data will be reset; this is placed in a timeout because it can take some time for the loop to finish, hence for the playing state to go to 0; so when playing is 0, spec_data will be reset and the timeout will be stoped */
	if (analyse_rt || loading){ 
		g_timeout_add (200, (GSourceFunc) memset_spec_data, NULL);
		}
	
	newEvent = TRUE;
}

/* Get the factor that multiplies the frequency range, to make it larger (= 'zoomFactor')*/
void cb_range_changed(GtkWidget *combo, gpointer data)
{
	zoomFactor = (gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) + 1);
	change_adjust_start();
	if ((bandsNumber * zoomFactor) > ((AUDIOFREQFACTOR) / hzStep)){
		bandsNumber = ((AUDIOFREQFACTOR)/hzStep) / zoomFactor;
		gtk_range_set_value(GTK_RANGE(pScaleBands), (gdouble)bandsNumber);
		} 
	newEvent = TRUE;
	getTextDisplayLabel();
}

void change_start(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	if ((int)iValue <= ((AUDIOFREQFACTOR) - (hzStep * bandsNumber *zoomFactor)) / 2){
		zoom = (int)iValue;
		}
	else {
		zoom = 0;
		}
	
	newEvent = TRUE;
	getTextDisplayLabel();
}

void change_adjust_start() 
{	
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(adjust_start), (gdouble)((AUDIOFREQFACTOR) - (hzStep * bandsNumber *zoomFactor)) / 2);
	if (zoom >= (((AUDIOFREQFACTOR) - (hzStep * bandsNumber *zoomFactor)) / 2)){
		gtk_adjustment_set_value (GTK_ADJUSTMENT(adjust_start), gtk_adjustment_get_upper (GTK_ADJUSTMENT(adjust_start)));
		}
}

gchar* format_value_start (GtkScale *scale, gdouble value) 
{
	return g_strdup_printf ("%d", (int)value * hzStep);
 }

void change_bands(GtkWidget *pWidget, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(GTK_RANGE(pWidget));
	bandsNumber = (int)iValue;
	change_adjust_start();
	if ((zoomFactor * bandsNumber) > ((AUDIOFREQFACTOR)/hzStep)) {
		zoomFactor = ((AUDIOFREQFACTOR)/hzStep) / bandsNumber;
		//printf("zoomFactor = %d\n", zoomFactor);
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), (zoomFactor - 1));
		}

	newEvent = TRUE;
	getTextDisplayLabel();
}

gchar* format_value_bands (GtkScale *scale, gdouble value) 
{
	return g_strdup_printf ("%d", (int)value * hzStep);
 }

/* Enable lines */
void check_menu_lines(GtkWidget *widget, gpointer data)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		lineScale = 1;
		}
	else {
		lineScale = 0;
		}

	newEvent = TRUE;
}

/* Enable text scale */
void check_menu_text(GtkWidget *widget, gpointer data)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		textScale = 1;
		}
	else {
		textScale = 0;
		}

	newEvent = TRUE;
}

void check_menu_pointer(GtkWidget *widget, gpointer data)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		pointer = 1;
		}
	else {
		pointer = 0;
		}

	newEvent = TRUE;
}

/* Enable gain change */
gboolean change_gain(GtkScaleButton *scaleButton, gdouble value, gpointer user_data)  
{
	showGain = (gfloat)value;
	newEvent = TRUE;
return FALSE;
}

void error_message_window(gchar *message){
	GtkWidget *pAbout;
	pAbout = gtk_message_dialog_new (NULL,
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_OK,
		"%s", message);
	gtk_dialog_run(GTK_DIALOG(pAbout));
     	gtk_widget_destroy(pAbout);
}





