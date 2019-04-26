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

#include "config.h"

#include "equalizer.h"

#define NBANDS 10

gboolean activateFilter;
GtkWidget *widgetF[10], *widgetB[10], *widgetG[10], *spinBPlowerValue, *spinBPupperValue;
GstObject *band, *band2, *band3;
gdouble freq;
gdouble bw;
gdouble gain;

/* Quit main window and eveything */
static void on_window_destroy (GObject * object, GtkWidget *effectsWindow)
{	
	showEqualizerWindow = FALSE;
	gtk_widget_destroy(effectsWindow);
}

static void 
on_gain_changed (GtkRange * range, gpointer user_data) // change gain of equalizer
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "gain", value, NULL);
}

static void
on_gain_changed2 (GtkRange * range, gpointer user_data) // change gain of equalizer2
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "gain", value, NULL);
}

static void
on_gain_changed3 (GtkRange * range, gpointer user_data) // change gain of equalizer3
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "gain", value, NULL);
}

static void
on_bandwidth_changed (GtkRange * range, gpointer user_data) // change bandwidth of equalizer
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "bandwidth", value, NULL);
}

static void
on_bandwidth_changed2 (GtkRange * range, gpointer user_data) // change bandwidth of equalizer2
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "bandwidth", value, NULL);
}

static void
on_bandwidth_changed3 (GtkRange * range, gpointer user_data) // change bandwidth of equalizer3
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);

  g_object_set (band, "bandwidth", value, NULL);
}

/* control frequency */
static void
on_freq_changed (GtkRange * range, gpointer user_data) // change frequency range of equalizer
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);
  GtkWidget *parent = gtk_widget_get_parent (GTK_WIDGET (range));
  GtkWidget *parent_parent = gtk_widget_get_parent (parent);
  gchar *label = g_strdup_printf ("%d Hz", (int) (value + 0.5));
  gtk_frame_set_label (GTK_FRAME (parent_parent), label);
  g_free (label);
  g_object_set (band, "freq", value, NULL);
}

static void
on_freq_changed2 (GtkRange * range, gpointer user_data) // change frequency range of equalizer2
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);
  GtkWidget *parent = gtk_widget_get_parent (GTK_WIDGET (range));
  GtkWidget *parent_parent = gtk_widget_get_parent (parent);
  gchar *label = g_strdup_printf ("%d Hz", (int) (value + 0.5));
  gtk_frame_set_label (GTK_FRAME (parent_parent), label);
  g_free (label);
  g_object_set (band, "freq", value, NULL);
}

static void
on_freq_changed3 (GtkRange * range, gpointer user_data) // change frequency range of equalizer3
{
  GstObject *band = GST_OBJECT (user_data);
  gdouble value = gtk_range_get_value (range);
  GtkWidget *parent = gtk_widget_get_parent (GTK_WIDGET (range));
  GtkWidget *parent_parent = gtk_widget_get_parent (parent);
  gchar *label = g_strdup_printf ("%d Hz", (int) (value + 0.5));
  gtk_frame_set_label (GTK_FRAME (parent_parent), label);
  g_free (label);
  g_object_set (band, "freq", value, NULL);
}

void getBand(){
	int i = 0;
	for (i = 0; i < NBANDS; i++) {
		band = gst_child_proxy_get_child_by_index (GST_CHILD_PROXY (equalizer), i);
		g_assert (band != NULL);
		//g_object_get (G_OBJECT (band), "freq", &freq, NULL);
		//g_object_get (G_OBJECT (band), "bandwidth", &bw, NULL);
		//g_object_get (G_OBJECT (band), "gain", &gain, NULL);
		gain = gtk_range_get_value (GTK_RANGE (widgetG[i]));
		on_gain_changed(GTK_RANGE (widgetG[i]), (gpointer) band);
		g_signal_connect (G_OBJECT (widgetG[i]), "value-changed", G_CALLBACK (on_gain_changed), (gpointer) band);
		}
}

/* reset all gain values of equalizers to 0 */
void reset_equalizer(){
	int i = 0;
	for (i = 0; i < NBANDS; i++) {
		gtk_range_set_value (GTK_RANGE (widgetG[i]), 0);
		}
}

gchar* format_gain_eq_value (GtkScale *scale, gdouble value) // adapt the displayed values of 'gain' : since there are 3 equalizers placed in series and commanded by the same scale together, the displayed gain value has to be multiplied by 3;
{
	return g_strdup_printf ("%.1f", value * 3);
 }

/* Active or deactivate the BP/BR filter */
void onCheckBandPass(GtkWidget *pToggle, gpointer data)
{
	if (pToggle != NULL){
		activateFilter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pToggle));
		}
	if (activateFilter) {
		BPlowerFreq = (gfloat)(gtk_spin_button_get_value (GTK_SPIN_BUTTON(spinBPlowerValue)));
		BPupperFreq = (gfloat)(gtk_spin_button_get_value (GTK_SPIN_BUTTON(spinBPupperValue)));
		}
	else {
		if (gtk_combo_box_get_active(GTK_COMBO_BOX(comboFilter))){
		    
			BPlowerFreq = 0; BPupperFreq = 0; // turns off band-pass
			}
		else { 
			BPlowerFreq = 0; BPupperFreq = 40000; // turns off band-reject
			}
		}
	g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, NULL);
	g_object_set (G_OBJECT (BP_BRfilter), "upper-frequency", BPupperFreq, NULL);
}

void change_filter_lower_value(GtkWidget *widget, gpointer data){
	if (activateFilter) {
		BPlowerFreq = gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
		g_object_set (G_OBJECT (BP_BRfilter), "lower-frequency", BPlowerFreq, NULL);
		}
}

void change_filter_upper_value(GtkWidget *widget, gpointer data){
	if (activateFilter){
		BPupperFreq = gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
		g_object_set (G_OBJECT (BP_BRfilter), "upper-frequency", BPupperFreq, NULL);
		}
}

void cb_filter_type_changed(GtkComboBox *combo, gpointer data)
{
// audiochebband's "mode" property : 0 is band-reject, 1 is band-pass filter
	g_object_set (G_OBJECT (BP_BRfilter), "mode", (gint)gtk_combo_box_get_active(GTK_COMBO_BOX(combo)), NULL);
	onCheckBandPass(NULL, NULL);
}

void change_filter_poles(GtkWidget *widget, gpointer data){
	int poles = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget));
	g_object_set (G_OBJECT (BP_BRfilter), "poles", (gint)poles, NULL);
}

void change_filter_ripple(GtkWidget *widget, gpointer data){
	gfloat ripple = (gfloat)gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
	g_object_set (G_OBJECT (BP_BRfilter), "ripple", ripple, NULL);
}

/* Filter and equalizer window */
void effects_window()
{
if (showEqualizerWindow == FALSE){ // show window if window is hidden
	int i = 0;
	GtkWidget *vbox[4], *hbox[11], *button, *checkBandPass, *frame, *spinFilterRipple, *spinFilterPoles;
	GtkObject *adjustment;
			
	effectsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(effectsWindow), "Spectrum3d : BP/BR filter and equalizer");
	g_signal_connect (G_OBJECT (effectsWindow), "destroy", G_CALLBACK (on_window_destroy), effectsWindow);
	vbox[0] = gtk_vbox_new (FALSE, 6);
	hbox[0] = gtk_hbox_new(TRUE, 0);
	hbox[1] = gtk_hbox_new(TRUE, 0);
	hbox[2] = gtk_hbox_new(FALSE, 0);			

	gtk_container_add(GTK_CONTAINER(effectsWindow), vbox[0]);
	gtk_box_pack_start(GTK_BOX(vbox[0]), hbox[0], TRUE, TRUE, 0);

/* Combo box to choose the type of filter : band-pass (BP) or band-reject (BR) */
	frame = gtk_frame_new("Type of filter");
	comboFilter = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(frame), comboFilter);
	gtk_box_pack_start(GTK_BOX(hbox[0]), frame, FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboFilter), "BAND PASS");
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboFilter), "BAND REJECT");
	gtk_combo_box_set_active(GTK_COMBO_BOX(comboFilter), 0);
	g_signal_connect( G_OBJECT(comboFilter), "changed", G_CALLBACK( cb_filter_type_changed ), NULL );

/* Check button to activate the BP/BR filter */
	checkBandPass = gtk_check_button_new_with_label("Activate Band\n Pass Filter");
	gtk_box_pack_start(GTK_BOX(hbox[0]), checkBandPass, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(checkBandPass), "toggled", G_CALLBACK(onCheckBandPass), NULL);

/* Spon buttons to set lower and upper values of the BP/BR filter  */
	frame = gtk_frame_new("Lower value of BP filter (hz)");
	adjustment = gtk_adjustment_new(200, 0, 40000, 1, 100, 0);
	spinBPlowerValue = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_container_add(GTK_CONTAINER(frame), spinBPlowerValue);
	gtk_box_pack_start(GTK_BOX(hbox[0]), frame, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (spinBPlowerValue), "value_changed", G_CALLBACK (change_filter_lower_value),
NULL);

	frame = gtk_frame_new("Upper value of BP filter (hz)");
	adjustment = gtk_adjustment_new(400, 0, 40000, 1, 100, 0);
	spinBPupperValue = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_container_add(GTK_CONTAINER(frame), spinBPupperValue);
	gtk_box_pack_start(GTK_BOX(hbox[0]), frame, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (spinBPupperValue), "value_changed", G_CALLBACK (change_filter_upper_value),
NULL);

	/*frame = gtk_frame_new("Poles");
	adjustment = gtk_adjustment_new(4, 4, 32, 1, 10, 0);
	spinFilterPoles = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_container_add(GTK_CONTAINER(frame), spinFilterPoles);
	gtk_box_pack_start(GTK_BOX(hbox[0]), frame, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (spinFilterPoles), "value_changed", G_CALLBACK (change_filter_poles), NULL);

	frame = gtk_frame_new("Ripple");
	adjustment = gtk_adjustment_new(0.25, 0, 200, 0.05, 1, 0);
	spinFilterRipple = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 2);
	gtk_container_add(GTK_CONTAINER(frame), spinFilterRipple);
	gtk_box_pack_start(GTK_BOX(hbox[0]), frame, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (spinFilterRipple), "value_changed", G_CALLBACK (change_filter_ripple), NULL);*/
	
	for (i = 0; i < NBANDS; i++) {
			
		gchar *label;
		GtkWidget *frame, *scales_hbox;
		/*  Set up 3 'bands' corresponding to the 3 equalizers placed in serie */
		band = gst_child_proxy_get_child_by_index (GST_CHILD_PROXY (equalizer), i);
		g_assert (band != NULL);
		g_object_get (G_OBJECT (band), "freq", &freq, NULL);
		g_object_get (G_OBJECT (band), "bandwidth", &bw, NULL);
		g_object_get (G_OBJECT (band), "gain", &gain, NULL);

		band2 = gst_child_proxy_get_child_by_index (GST_CHILD_PROXY (equalizer2), i);
		g_assert (band2 != NULL);
		g_object_get (G_OBJECT (band2), "freq", &freq, NULL);
		g_object_get (G_OBJECT (band2), "bandwidth", &bw, NULL);
		g_object_get (G_OBJECT (band2), "gain", &gain, NULL);

		band3 = gst_child_proxy_get_child_by_index (GST_CHILD_PROXY (equalizer3), i);
		g_assert (band3 != NULL);
		g_object_get (G_OBJECT (band3), "freq", &freq, NULL);
		g_object_get (G_OBJECT (band3), "bandwidth", &bw, NULL);
		g_object_get (G_OBJECT (band3), "gain", &gain, NULL);

		label = g_strdup_printf ("%d Hz", (int) (freq + 0.5));
		frame = gtk_frame_new (label);
		g_free (label);

		scales_hbox = gtk_hbox_new (TRUE, 6);

		/* Draw the scales for gain of each band; the displayed value will be multiplied by 3 since there 3 equalizers placed in serie controlled by the same Scale */
		widgetG[i] = gtk_vscale_new_with_range (-24.0, 12.0, 0.5);
		gtk_widget_set_tooltip_text (widgetG[i], "Gain in dB");
		gtk_scale_set_draw_value (GTK_SCALE (widgetG[i]), TRUE);
		gtk_scale_set_value_pos (GTK_SCALE (widgetG[i]), GTK_POS_TOP);
		gtk_range_set_value (GTK_RANGE (widgetG[i]), gain);
		gtk_scale_set_digits (GTK_SCALE(widgetG[i]), 1);
		gtk_widget_set_size_request (widgetG[i], 25, 150);
		gtk_range_set_inverted (GTK_RANGE (widgetG[i]), TRUE);
		g_signal_connect (G_OBJECT (widgetG[i]), "value-changed",
		G_CALLBACK (on_gain_changed), (gpointer) band);
		g_signal_connect (G_OBJECT (widgetG[i]), "value-changed",
		G_CALLBACK (on_gain_changed2), (gpointer) band2);
		g_signal_connect (G_OBJECT (widgetG[i]), "value-changed",
		G_CALLBACK (on_gain_changed3), (gpointer) band3);
		g_signal_connect(G_OBJECT(widgetG[i]), "format-value", G_CALLBACK(format_gain_eq_value), NULL);
		gtk_box_pack_start (GTK_BOX (scales_hbox), widgetG[i], TRUE, TRUE, 0);

		/* Draw the scales for bandwith of each band, from 0 to 20000 Hz */
		widgetB[i] = gtk_vscale_new_with_range (0.0, 20000.0, 5.0);
		gtk_widget_set_tooltip_text (widgetB[i], "Bandwidth of this band in Herz; finer adjustment can be done with the Up/Down or PgUp/PgDn Keys");
		gtk_scale_set_draw_value (GTK_SCALE (widgetB[i]), TRUE);
		gtk_scale_set_value_pos (GTK_SCALE (widgetB[i]), GTK_POS_TOP);
		gtk_range_set_value (GTK_RANGE (widgetB[i]), bw);
		gtk_widget_set_size_request (widgetB[i], 25, 150);
		g_signal_connect (G_OBJECT (widgetB[i]), "value-changed",
		G_CALLBACK (on_bandwidth_changed), (gpointer) band);
		g_signal_connect (G_OBJECT (widgetB[i]), "value-changed",
		G_CALLBACK (on_bandwidth_changed2), (gpointer) band2);
		g_signal_connect (G_OBJECT (widgetB[i]), "value-changed",
		G_CALLBACK (on_bandwidth_changed3), (gpointer) band3);
		gtk_box_pack_start (GTK_BOX (scales_hbox), widgetB[i], TRUE, TRUE, 0);
		
		/* Draw the scales to ajust frequency of each band, from 20 to 20000 Hz */
		widgetF[i] = gtk_vscale_new_with_range (20.0, 20000.0, 5.0);
		gtk_widget_set_tooltip_text (widgetF[i], "Frequency of this band in Herz; finer adjustment can be done with the Up/Down or PgUp/PgDn Keys");
		gtk_scale_set_draw_value (GTK_SCALE (widgetF[i]), TRUE);
		gtk_scale_set_value_pos (GTK_SCALE (widgetF[i]), GTK_POS_TOP);
		gtk_range_set_value (GTK_RANGE (widgetF[i]), freq);
		gtk_widget_set_size_request (widgetF[i], 25, 150);
		g_signal_connect (G_OBJECT (widgetF[i]), "value-changed",
		G_CALLBACK (on_freq_changed), (gpointer) band);
		g_signal_connect (G_OBJECT (widgetF[i]), "value-changed",
		G_CALLBACK (on_freq_changed2), (gpointer) band2);
		g_signal_connect (G_OBJECT (widgetF[i]), "value-changed",
		G_CALLBACK (on_freq_changed3), (gpointer) band3);
		gtk_box_pack_start (GTK_BOX (scales_hbox), widgetF[i], TRUE, TRUE, 0);
		
		gtk_container_add (GTK_CONTAINER (frame), scales_hbox);

		gtk_box_pack_start (GTK_BOX (hbox[1]), frame, TRUE, TRUE, 0);
  }

  gtk_box_pack_start (GTK_BOX (vbox[0]), hbox[1], TRUE, TRUE, 0);
	
/* Button that reset all gain values to 0 */
	button = gtk_button_new_with_label("Reset\nequalizer");
	gtk_widget_set_tooltip_text (button, "Reset all gain values to 0");
	gtk_box_pack_start (GTK_BOX (hbox[2]), button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox[0]), hbox[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_equalizer), NULL);
	
	showEqualizerWindow = TRUE;
  	gtk_widget_show_all (effectsWindow);
}
else { // hide window if window is showed
	showEqualizerWindow = FALSE;
	gtk_widget_hide(effectsWindow);
	}
}



