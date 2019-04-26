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
#include <gtk/gtk.h>

#include "config.h"
#include "spectrum3d.h"
#include "onclick.h"

/* Gstreamer will perform spectrum analysis on the frequencies up to the half of the AUDIOFREQ parameter. Since AUDIOFREQ is 44100Hz and since we want the values displayed untill 20000 Hz, we define this constant 0.453514739, thas has been rounded to 0.453514740 because we will divide it by an integer (for example, if the value of 10000 is not reached, this will rounded to 9999) : this factor multiplied by AUDIOFREQ gives the analysed range ofr frequencies (e.g. : for AUDIOFREQ = 44100, AUDIOFREQ * 0.453514740 = 20000)*/

#define AUDIOFREQFACTOR AUDIOFREQ * 0.453514740

/* Get the type of view (3D, 3D flat or 2D) */
void change_perspective(GtkWidget *widget, gpointer data)
{  
	
	GSList *list;
    	const gchar *label;
	
	list = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM(data));

	while(list) {
		if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
			label = gtk_menu_item_get_label(GTK_MENU_ITEM(widget));
			list = NULL;

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
		else {
			list = g_slist_next(list);
		}
	}	
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
	X = spectrum3d.presetX;
	Y = spectrum3d.presetY;
	Z = spectrum3d.presetZ;
	AngleH = spectrum3d.presetAngleH;
	AngleV = spectrum3d.presetAngleV;
	AngleZ = spectrum3d.presetAngleZ;

	spectrum3d.frames = spectrum3d.previousFrames;
	newEvent = TRUE;
}

/* Reset all spec_data; this is called via a callback; reset will be done if playing is 0 */
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
void cb_range_changed(GtkWidget *combo, Spectrum3dGui *spectrum3dGui)
{
	zoomFactor = (gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) + 1);
	change_adjust_start(NULL, spectrum3dGui);
	if ((bandsNumber * zoomFactor) > ((AUDIOFREQFACTOR) / hzStep)){
		bandsNumber = ((AUDIOFREQFACTOR)/hzStep) / zoomFactor;
		gtk_range_set_value(GTK_RANGE(spectrum3dGui->scaleBands), (gdouble)bandsNumber);
		} 
	newEvent = TRUE;
}

void change_start(GtkRange *range, gpointer data)
{
	gfloat iValue;
	iValue = gtk_range_get_value(range);
	if ((int)iValue <= ((AUDIOFREQFACTOR) - ((hzStep * bandsNumber *zoomFactor)-1)) / 2){
		zoom = (int)iValue;
		}
	else {
		zoom = 0;
		}
	
	newEvent = TRUE;
}

void change_adjust_start(GtkRange *range, Spectrum3dGui *spectrum3dGui) 
{	
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(spectrum3dGui->adjustStart), (gdouble)((AUDIOFREQFACTOR) - (hzStep * bandsNumber *zoomFactor)) / 2);  
	if (zoom >= (((AUDIOFREQFACTOR) - (hzStep * bandsNumber *zoomFactor)) / 2)){
		gtk_adjustment_set_value (GTK_ADJUSTMENT(spectrum3dGui->adjustStart), gtk_adjustment_get_upper (GTK_ADJUSTMENT(spectrum3dGui->adjustStart)));
		}
	newEvent = TRUE;
}

gchar* format_value_start (GtkScale *scale, gdouble value) 
{
	return g_strdup_printf ("%d", (int)value * hzStep);
 }

void change_bands(GtkRange *range, Spectrum3dGui *spectrum3dGui)
{
	gfloat iValue;
	iValue = gtk_range_get_value(range);
	/* The display is done spect_band by spect_band, and spect_band is an integer; the frequency value displayed is calculated with the maximum frequency value that is divided by 10; so we have to calculate the modulo of the scale value divided by 5 (10/hzStep) to make the value displayed by the 'ajust range' scale and the value displayed on the graph to correspond */
	int modulo = (int)iValue % 5;
	gtk_range_set_value(range, iValue - modulo);
	bandsNumber = (int)iValue;
	change_adjust_start(range, spectrum3dGui);
	if ((zoomFactor * bandsNumber) > ((AUDIOFREQFACTOR)/hzStep)) {
		//printf("(zoomFactor * bandsNumber) = %d, (AUDIOFREQFACTOR)/hzStep = %d\n", (zoomFactor * bandsNumber), (int)((AUDIOFREQFACTOR)/hzStep));
		zoomFactor = ((AUDIOFREQFACTOR)/hzStep) / bandsNumber;
		gtk_combo_box_set_active(GTK_COMBO_BOX(spectrum3dGui->cbRange), (zoomFactor - 1));
		}

	newEvent = TRUE;
}

gchar* format_value_bands (GtkScale *scale, gdouble value) 
{
	//int result = ((int)value * hzStep) % 10;
	return g_strdup_printf ("%d", ((int)value * hzStep));// - result);
 }

/* Enable lines */
void check_menu_lines(GtkWidget *widget, Spectrum3dGui *spectrum3dGui)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		spectrum3dGui->lineScale = 1;
		}
	else {
		spectrum3dGui->lineScale = 0;
		}

	newEvent = TRUE;
}

/* Enable text scale */
void check_menu_text(GtkWidget *widget, Spectrum3dGui *spectrum3dGui)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		spectrum3dGui->textScale = 1;
		}
	else {
		spectrum3dGui->textScale = 0;
		}

	newEvent = TRUE;
}

void check_menu_pointer(GtkWidget *widget, Spectrum3dGui *spectrum3dGui)
{
	gboolean state;
	state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	if (state) {
		spectrum3dGui->pointer = 1;
		}
	else {
		spectrum3dGui->pointer = 0;
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





