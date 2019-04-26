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
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "config.h"
#include "spectrum3d.h"
#include "main.h"

char prefPath[100];
GtkWidget *playButton, *pScaleStart, *displayLabel;

/* Sets the icon of 'playButton' as 'play' or 'pause' */
void setPlayButtonIcon (){
	GtkWidget *playImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_SMALL_TOOLBAR);
	GtkWidget *pauseImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_SMALL_TOOLBAR);
	if (playing){
		if (pose){
			gtk_button_set_image(GTK_BUTTON(playButton),playImage);
			}
		else {
			gtk_button_set_image(GTK_BUTTON(playButton),pauseImage);
			}
		}
	else {
		gtk_button_set_image(GTK_BUTTON(playButton),playImage);
		}	
}

/* Display label with the first and last displayed frequency values, and the displayed frequency range*/
void getTextDisplayLabel(){
	char textDisplayLabel[80];
	int startDisplay = (int)gtk_range_get_value(GTK_RANGE(pScaleStart)) * hzStep;
	int displayRange = (int)gtk_range_get_value(GTK_RANGE(pScaleBands)) * hzStep * zoomFactor;
	sprintf(textDisplayLabel, "From %d to %d Hz\n Range = %d Hz", startDisplay, startDisplay + displayRange, displayRange);
	gtk_label_set_text (GTK_LABEL(displayLabel), textDisplayLabel);
}

void show_position(gchar *positionLabel){
	gchar *sUtf8;
	sUtf8 = g_locale_to_utf8(positionLabel, -1, NULL, NULL, NULL);
	gtk_label_set_label(GTK_LABEL(timeLabel), sUtf8);
	g_free(sUtf8);
	gtk_widget_show_all(timeLabel);

}

void quit_spectrum3d(){
	on_stop();
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	printf("%s \nPlease report any bug to %s\n", PACKAGE_STRING, PACKAGE_BUGREPORT);	
	
	typeSource = NONE; 
	spectrum3d.interval_rt = 100;
	analyse_rt = TRUE;
					
	get_saved_values();

	gchar *filename;
	int i = 0;
	guint timeoutEvent, intervalDisplaySpectro;
	GtkWidget *pVBox[4], *pHBox[12], *menuBar, *menu, *submenu, *menuItem, *submenuItem, *button, *pButton[7], *pButtonSelect, *pComboZoom, *pComboSpeed, *pSpinSpeed, *frame, *pScaleGain, *separator, *image, *label, *widget, *da;
	GtkToolItem *toolItem;
	GtkObject *adjustment;
	GdkColor color;
	Spectrum3dGui spectrum3dGui;
	GSList *radio_menu_group;

	newEvent = TRUE; // set newEvent to TRUE allows to have a first empty scale at the beginning
	intervalDisplaySpectro = (guint)spectrum3d.interval_display;
	
	gst_init (NULL, NULL);
	gtk_init (&argc, &argv);

/* SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
		}
	configure_SDL_gl_window (spectrum3d.width, spectrum3d.height);
	SDL_WM_SetCaption(PACKAGE_NAME, NULL);
	SDL_EnableKeyRepeat(10, 10);
//////////
	
	initGstreamer();
	init_audio_values();
	init_display_values();
	
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (mainWindow, 700, 170);
	gtk_window_set_title(GTK_WINDOW(mainWindow), PACKAGE_NAME);
	g_signal_connect (G_OBJECT (mainWindow), "destroy", G_CALLBACK (quit_spectrum3d), NULL);

	for (i = 0; i < 4; i++) {
		pVBox[i] = gtk_vbox_new(FALSE, 0);
		}
	pHBox[0] = gtk_hbox_new(TRUE, 0);
	for (i = 1; i < 12; i++) {
		pHBox[i] = gtk_hbox_new(FALSE, 0);
		}
	
	gtk_container_add(GTK_CONTAINER(mainWindow), pVBox[1]); 
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[0], FALSE, FALSE, 0);
		
/* Menu */
	menuBar = gtk_menu_bar_new();

	menu = gtk_menu_new(); // 'Quit' submenu
	menuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);    
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(gtk_main_quit), NULL); 
	menuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

	menu = gtk_menu_new(); // 'Edit' submenu
        menuItem = gtk_menu_item_new_with_label("Preferences");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onPreferences), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

	menu = gtk_menu_new(); // 'Sound' submenu
	menuItem = gtk_menu_item_new_with_label("Sound");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

		submenu = gtk_menu_new();// 'Play test sound' sub-submenu
		menuItem = gtk_menu_item_new_with_label("Play test sound");
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
		g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(test_sound_window), NULL);

	menu = gtk_menu_new(); // 'View' submenu
	menuItem = gtk_menu_item_new_with_label("View");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);
	
		submenu = gtk_menu_new();// 'viewType' sub-submenu
		menuItem = gtk_menu_item_new_with_label("Perspective");
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuItem), submenu);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);	
			
			spectrum3dGui.radio3D = gtk_radio_menu_item_new_with_label(NULL, "3D (D)");
			radio_menu_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(spectrum3dGui.radio3D));
			g_signal_connect(G_OBJECT(spectrum3dGui.radio3D), "toggled", G_CALLBACK(change_perspective), spectrum3dGui.radio3D);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.radio3D);

			spectrum3dGui.radio3Dflat = gtk_radio_menu_item_new_with_label(radio_menu_group, "3D flat (F)");
			radio_menu_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(spectrum3dGui.radio3Dflat));
			g_signal_connect(G_OBJECT(spectrum3dGui.radio3Dflat), "toggled", G_CALLBACK(change_perspective), spectrum3dGui.radio3Dflat);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.radio3Dflat);

			spectrum3dGui.radio2D = gtk_radio_menu_item_new_with_label(radio_menu_group, "2D (D)");
			radio_menu_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(spectrum3dGui.radio2D));
			g_signal_connect(G_OBJECT(spectrum3dGui.radio2D), "toggled", G_CALLBACK(change_perspective), spectrum3dGui.radio2D);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.radio2D);

		submenu = gtk_menu_new();// 'Scale' sub-submenu
		menuItem = gtk_menu_item_new_with_label("Scale");
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuItem), submenu);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);	
	
			spectrum3dGui.checkMenuText = gtk_check_menu_item_new_with_label("Text (T)");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spectrum3dGui.checkMenuText), TRUE);
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuText), "activate", G_CALLBACK(check_menu_text), NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuText);
	
			spectrum3dGui.checkMenuLines = gtk_check_menu_item_new_with_label("Lines (L)");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spectrum3dGui.checkMenuLines), TRUE);
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuLines), "activate", G_CALLBACK(check_menu_lines), NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuLines);

			spectrum3dGui.checkMenuPointer = gtk_check_menu_item_new_with_label("Pointer (P)");
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuPointer), "activate", G_CALLBACK(check_menu_pointer), NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuPointer);

	submenu = gtk_menu_new();// 'Change/reset view' sub-submenu
		menuItem = gtk_menu_item_new_with_label("Change/reset view");
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuItem), submenu);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);	
			
			spectrum3dGui.reset = gtk_menu_item_new_with_label("Reset view (R)");
			gtk_widget_set_tooltip_text (spectrum3dGui.reset, "Reset the view as if Spectrum3d was just started");
			g_signal_connect(G_OBJECT(spectrum3dGui.reset), "activate", G_CALLBACK(reset_view), spectrum3dGui.reset);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.reset);

			spectrum3dGui.front = gtk_menu_item_new_with_label("Front view (O)");
			gtk_widget_set_tooltip_text (spectrum3dGui.front, "2D view showing frequency versus intensity of the sound; shows a snapshot of the harmonics at a given time");
			g_signal_connect(G_OBJECT(spectrum3dGui.front), "activate", G_CALLBACK(front_view), spectrum3dGui.front);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.front);

			submenuItem = gtk_menu_item_new_with_label("Preset view");
			gtk_widget_set_tooltip_text (submenuItem, "Set the view with the chosen preset values");
			g_signal_connect(G_OBJECT(submenuItem), "activate", G_CALLBACK(set_view_from_preset), submenuItem);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenuItem);

	menu = gtk_menu_new(); // 'Help...' submenu
	menuItem = gtk_menu_item_new_with_label("Shortcuts"); 
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#ifdef HAVE_LIBUTOUCH_GEIS
	menuItem = gtk_menu_item_new_with_label("Gestures Shortcuts");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onGesturesShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#endif 
	menuItem = gtk_menu_item_new_with_label("About...");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onAbout), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Quick start");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onQuickStart), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);
	gtk_box_pack_start(GTK_BOX(pHBox[0]), menuBar, FALSE, TRUE, 0);

/* SourceButtons to set type of source (none, audio file, microphone) */
	spectrum3dGui.stop = gtk_button_new();
	image = gtk_image_new_from_stock (GTK_STOCK_STOP, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.stop),image);
	gdk_color_parse ("gold",&color);
	gtk_widget_set_name(spectrum3dGui.stop, "stop");
	gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui.stop), GTK_STATE_NORMAL, &color);
	gtk_widget_modify_bg(GTK_WIDGET(spectrum3dGui.stop), GTK_STATE_PRELIGHT, &color);
	gtk_widget_set_tooltip_text (spectrum3dGui.stop, "Stop playing everything; no source of playing");
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[1], FALSE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.stop, FALSE, TRUE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.stop), "clicked", G_CALLBACK(change_source_button), &spectrum3dGui);

	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "microphone.png", NULL);
	image = gtk_image_new_from_file(filename);
	spectrum3dGui.mic = gtk_button_new();
	gtk_button_set_image (GTK_BUTTON(spectrum3dGui.mic), image);
	gtk_widget_set_name(spectrum3dGui.mic, "mic");
	gtk_widget_set_tooltip_text (spectrum3dGui.mic, "Source is microphone; select this to record something and then load it");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.mic, FALSE, TRUE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.mic), "clicked", G_CALLBACK(change_source_button), &spectrum3dGui);

	spectrum3dGui.file = gtk_button_new();
	image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.file),image);
	gtk_widget_set_name(spectrum3dGui.file, "file");
	gtk_widget_set_tooltip_text (spectrum3dGui.file, "Source is an audio file");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.file, FALSE, TRUE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.file), "clicked", G_CALLBACK(change_source_button), &spectrum3dGui);

	spectrum3dGui.reload = gtk_button_new();
	image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.reload),image);
	gtk_widget_set_sensitive (spectrum3dGui.reload, FALSE);
	gtk_widget_set_tooltip_text (spectrum3dGui.reload, "Reload audio file (usefull if some audio parameters like equalizer or filters have been changed)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.reload, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.reload), "clicked", G_CALLBACK(load_audio_file), NULL);

	widget = gtk_check_button_new_with_label("Analyse in\nrealtime");
	gtk_widget_set_tooltip_text (GTK_WIDGET(widget), "If checked, harmonics will be analysed and displayed at the same time; if unchecked, harmonics will be analysed first, then displayed for the whole audio file, then it can be played afterwards");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), TRUE);
	g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(set_analyse_in_rt), &spectrum3dGui);

/* separator */
	widget = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);
	
/* "Play/Pause" button */
	playButton = gtk_button_new();
	gtk_widget_set_tooltip_text (playButton, "Play/Pause the audio stream");
	setPlayButtonIcon();
	gtk_widget_set_size_request (playButton, 50, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), playButton, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(playButton), "clicked", G_CALLBACK(playFromSource), "NO_MESSAGE");
	
/* "Stop" button */
	button = gtk_button_new();
	gtk_widget_set_tooltip_text (button, "Stop playing audio stream");
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(button),image);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), button, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_stop), NULL);

/* "Record" button */
	spectrum3dGui.record = gtk_button_new();
	gtk_widget_set_tooltip_text (spectrum3dGui.record, "Record from microphone");
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_RECORD, GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.record),image);
	gtk_widget_set_sensitive (spectrum3dGui.record, FALSE);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.record, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.record), "clicked", G_CALLBACK(record_window), NULL);

/* separator */
	widget = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);

/* JACK check button */
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "qjackctl.png", NULL);
	image = gtk_image_new_from_file(filename);
	widget = gtk_check_button_new ();
	gtk_button_set_image (GTK_BUTTON(widget), image);
	gtk_widget_set_tooltip_text (widget, "Use Jack-Audio-Connection-Kit (JACK)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(use_jack), &spectrum3dGui);

/* separator */
	widget = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);

/* Button to open the Filter and Equalizer window */
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "equalizer.png", NULL);
	image = gtk_image_new_from_file(filename);
	widget = gtk_button_new();  
	gtk_button_set_image (GTK_BUTTON(widget), image);
	gtk_widget_set_tooltip_text (widget, "Show/Hide the filter and equalizer window");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(effects_window), NULL);

/* Time label */
	label=gtk_label_new("Time : ");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[5], FALSE, FALSE, 2);

/* Progress & seek scale */
	scaleSeek = gtk_hscale_new_with_range (0.0, 100.0, 1);
	gtk_scale_set_draw_value (GTK_SCALE (scaleSeek), FALSE);
	//gtk_scale_set_value_pos (GTK_SCALE (pScaleSeek), GTK_POS_TOP);
	gtk_range_set_value (GTK_RANGE (scaleSeek), 0);
	gtk_widget_set_size_request (scaleSeek, 500, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[5]), scaleSeek, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(scaleSeek), "value-changed", G_CALLBACK(on_seek), NULL);
	
	timeLabel=gtk_label_new("           0:00 / 0:00           ");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), timeLabel, FALSE, FALSE, 0);

/* Starting value of the display */
	frame = gtk_frame_new("Start value of display (in Hz)");
	gtk_widget_set_tooltip_text (frame, "The lower displayed frequency (in herz)");
	adjust_start = gtk_adjustment_new(0, 0, 5000, ((gdouble)hzStep * (gdouble)zoomFactor), (((gdouble)hzStep * (gdouble)zoomFactor) * 10), 0);
	pScaleStart = gtk_hscale_new(GTK_ADJUSTMENT(adjust_start));
	gtk_scale_set_digits (GTK_SCALE(pScaleStart), 0);
	gtk_scale_set_value_pos(GTK_SCALE(pScaleStart), GTK_POS_RIGHT);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[11], FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), pScaleStart);
	g_signal_connect(G_OBJECT(pScaleStart), "value-changed", G_CALLBACK(change_start), NULL);
	g_signal_connect(G_OBJECT(pScaleStart), "format-value", G_CALLBACK(format_value_start), NULL);

/* Range of display */
	frame = gtk_frame_new("Range of display (in Hz)");
	gtk_widget_set_tooltip_text (frame, "The range of the displayed frequency (in herz)");
	adjust_bands = gtk_adjustment_new(1000, 20, 1000, 20, 50, 0);
	pScaleBands = gtk_hscale_new(GTK_ADJUSTMENT(adjust_bands));
	gtk_scale_set_digits (GTK_SCALE(pScaleBands), 0);
	gtk_scale_set_value_pos(GTK_SCALE(pScaleBands), GTK_POS_RIGHT);
	gtk_container_add(GTK_CONTAINER(frame), pScaleBands);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pScaleBands), "value-changed", G_CALLBACK(change_bands), NULL);
	g_signal_connect(G_OBJECT(pScaleBands), "format-value", G_CALLBACK(format_value_bands), NULL);
	
/* "x" label */
	label=gtk_label_new("x");
	gtk_container_add(GTK_CONTAINER(pHBox[11]), label);

/* Factor that multiplies the range of display */
	frame = gtk_frame_new("");
	gtk_widget_set_tooltip_text (frame, "Factor that multiplies the frequency range, to make it larger");
	pComboRange = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(frame), pComboRange);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	for (i = 1; i <= 20; i++){
		gchar text[4];
		sprintf(text, "%d", i);
		gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), text);
		}
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), 0);
	g_signal_connect( G_OBJECT(pComboRange), "changed", G_CALLBACK( cb_range_changed ), NULL );
	
/* Label that shows starting value, ending value and range of display */
	frame = gtk_frame_new("Values displayed");
	gtk_widget_set_tooltip_text (frame, "The lower and the highest displayed value (in herz), and their range");
	displayLabel=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(frame), displayLabel);
	getTextDisplayLabel();
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);

/* 'Gain' Gtk Scale */
	frame = gtk_frame_new("Display Gain");
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 10);
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "gain.png", NULL);
	image = gtk_image_new_from_file(filename);
	
	spectrum3dGui.scaleGain = gtk_scale_button_new (GTK_ICON_SIZE_LARGE_TOOLBAR, 0, 1, 0.01, NULL);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.scaleGain),image);
	//gtk_button_set_label (GTK_BUTTON(spectrum3dGui.scaleGain), "GAIN");
	gtk_container_add(GTK_CONTAINER(frame), spectrum3dGui.scaleGain);
	//gtk_box_pack_start(GTK_BOX(pHBox[11]), pScaleGain, FALSE, FALSE, 0);
	//gtk_widget_set_size_request (pScaleGain, 200, 20);
	//gtk_scale_set_value_pos(GTK_SCALE(pScaleGain), GTK_POS_RIGHT);
	gtk_widget_set_tooltip_text (spectrum3dGui.scaleGain, "Adjust the displayed level of the intensity of the sound");
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(spectrum3dGui.scaleGain), 0.2);
	//g_object_set (pScaleGain, "update-policy", GTK_UPDATE_DISCONTINUOUS, NULL);
	g_signal_connect(G_OBJECT(spectrum3dGui.scaleGain), "value-changed", G_CALLBACK(change_gain), NULL);

#ifdef HAVE_LIBUTOUCH_GEIS
	setupGeis();
#endif

	gtk_widget_show_all (mainWindow);
	timeoutEvent = g_timeout_add (100, (GSourceFunc)sdl_event, &spectrum3dGui);
	spectrum3d.timeoutExpose = g_timeout_add (intervalDisplaySpectro, (GSourceFunc)display_spectro, NULL);
	gtk_main ();

/* Quit everything */

#ifdef HAVE_LIBUTOUCH_GEIS
	geisQuit();
#endif
	on_stop();
	g_source_remove(spectrum3d.timeoutExpose);
	g_source_remove(timeoutEvent);
	SDL_Quit();

	print_rc_file("var");

	printf("Quit everything\nGood Bye!\n");
	
	return 0;
}



