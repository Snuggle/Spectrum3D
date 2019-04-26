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
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "config.h"

#ifdef HAVE_LIBUTOUCH_GEIS
	#include <geis/geis.h>
#endif

#include "main.h"

char prefPath[100];
GtkWidget *playButton, *pScaleStart, *displayLabel;

/* Quit main window and eveything */
static void on_window_destroy (GObject * object, gpointer user_data)
{
	printf("Quit everything\nGood Bye!\n");
	if (sFile != NULL){
		g_free(sFile);
		}
	onStop();
	print_rc_file("var");
	gtk_main_quit ();
}

/* Get the name of the file to be played */
void getFileName()
{
	if (filenames != NULL) {
		g_print ("Getting the file name from GSList");
		sFile = (gchar*) filenames->data;
		g_print ("%s was selected.\n", sFile);
		}
	else {
		g_print ("No file to read - GSList seems to be empty \nYou have to select a file");
		}
}

 /* Select audio file if "Audio File" Source is choosen" */
void selectFile(GtkWidget *pWidget, gpointer data) 
{
	
	if (typeSource == AUDIO_FILE) {
		printf("Opening file selection dialog\n");
		GtkWidget *pFileSelection;
		GtkWidget *pParent;
		pParent = GTK_WIDGET(data);

		pFileSelection = gtk_file_chooser_dialog_new("Select File",
		GTK_WINDOW(pParent),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_OK,
		NULL);
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (pFileSelection), TRUE);
		gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

		switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
		{
		case GTK_RESPONSE_OK:
		       	filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (pFileSelection));
			getFileName();
		default:
		    break;
		}
		gtk_widget_destroy(pFileSelection);
	}
	else {
		printf("Select a file is only possible if source is Audio file\n");
		}
}

/* Sets the icon of 'playButton' as 'play' or 'pause' */
void setPlayButtonIcon (){
	GtkWidget *playImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
	GtkWidget *pauseImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
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

int main(int argc, char *argv[])
{
	printf("%s \nPlease report any bug to %s\n", PACKAGE_STRING, PACKAGE_BUGREPORT);	
	
	typeSource = MIC; 
	playing = 0;
	zoom = 0;
	forward = 0;
	backward = 0;
	pose = 0;
	scale = 0;
	firstPass3D = 1;
	zoomFactor = 1;
	showGain = 0.2;
	textScale = 1;
	lineScale = 1;
	f = 2;
	intervalTimeout = 150;
	spect_bands = 11025;
	AUDIOFREQ = 44100;
	pointer = FALSE;
	colorType = PURPLE;
	useCopyPixels = TRUE;
					
	get_saved_values();

	width = presetWidth;
				
	GtkWidget *pVBox[4], *pHBox[13], *pMenuBar, *pMenu, *pMenuItem, *button, *pButton[7], *pButtonSelect, *pRadio[6], *pComboZoom, *pComboSpeed, *pSpinSpeed, *pFrame, *pScaleGain, *pCheckTextScale, *pCheckLineScale, *checkPointer, *separator, *spinTestSound, *image, *label, *volumeTestSound;
	GtkObject *adjustment;
	GdkColor colorFrame;
	int i = 0;
	if (flatView) {
		widthFrame = ((width-200)/2);
		}
	else {
		widthFrame = (width-200);
		}
	bandsNumber = widthFrame;
	hzStep = (AUDIOFREQ/2) / spect_bands;

	initGstreamer();
	gtk_init (&argc, &argv);
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (mainWindow, 660, 450);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "Spectrum3d : Real-Time Spectral Analysis");
	g_signal_connect (G_OBJECT (mainWindow), "destroy", G_CALLBACK (on_window_destroy), NULL);

	for (i = 0; i < 4; i++) {
		pVBox[i] = gtk_vbox_new(FALSE, 0);
		}
	pHBox[0] = gtk_hbox_new(TRUE, 0);
	pHBox[1] = gtk_hbox_new(FALSE, 20);
	for (i = 2; i < 10; i++) {
		pHBox[i] = gtk_hbox_new(FALSE, 20);
		}
	pHBox[10] = gtk_hbox_new(FALSE, 20);
	pHBox[11] = gtk_hbox_new(FALSE, 20);
	pHBox[12] = gtk_hbox_new(FALSE, 0);
	
	gtk_container_add(GTK_CONTAINER(mainWindow), pVBox[1]); 

	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[0], FALSE, FALSE, 0);
		
/* Menu */
	pMenuBar = gtk_menu_bar_new();

	pMenu = gtk_menu_new(); // 'Quit' submenu
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);    
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(on_window_destroy), (GtkWidget*) mainWindow); 
	pMenuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new(); // 'Edit' submenu
        pMenuItem = gtk_menu_item_new_with_label("Preferences");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onPreferences), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);

	pMenu = gtk_menu_new(); // 'Help...' submenu
	pMenuItem = gtk_menu_item_new_with_label("Shortcuts"); 
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
#ifdef HAVE_LIBUTOUCH_GEIS
	pMenuItem = gtk_menu_item_new_with_label("Gestures Shortcuts");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onGesturesShortcuts), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
#endif 
	pMenuItem = gtk_menu_item_new_with_label("About...");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onAbout), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Quick start");
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(onQuickStart), (GtkWidget*) mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	pMenuItem = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pMenuBar, FALSE, TRUE, 0);

/* Buttons frame */
	pFrame = gtk_frame_new("");
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[1], TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pFrame, TRUE, TRUE, 0);	
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[12]);

/* "Rewind" button */
	button = gtk_button_new();
	gtk_widget_set_tooltip_text (button, "Seek 5 seconds backward");
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_box_pack_start(GTK_BOX(pHBox[12]), button, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(button), "button-press-event", G_CALLBACK(timeBackward), NULL);
	g_signal_connect(G_OBJECT(button), "button-release-event", G_CALLBACK(stopSeek), NULL);
	
/* "Play/Pause" button */
	playButton = gtk_button_new();
	gtk_widget_set_tooltip_text (playButton, "Play/Pause the audio stream");
	setPlayButtonIcon();
	gtk_widget_set_size_request (playButton, 100, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[12]), playButton, TRUE, TRUE, 10);
	g_signal_connect(G_OBJECT(playButton), "clicked", G_CALLBACK(playFromSource), NULL);
	
/* "Stop" button */
	button = gtk_button_new();
	gtk_widget_set_tooltip_text (button, "Stop playing audio stream");
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button),image);
	gtk_box_pack_start(GTK_BOX(pHBox[12]), button, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(onStop), NULL);

/* "Seek Forward" button */
	button = gtk_button_new();
	gtk_widget_set_tooltip_text (button, "Seek 5 seconds forward");
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button),image);
	gtk_box_pack_start(GTK_BOX(pHBox[12]), button, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(button), "button-press-event", G_CALLBACK(timeForward), NULL);
	g_signal_connect(G_OBJECT(button), "button-release-event", G_CALLBACK(stopSeek), NULL);
 
/* "Select File" button */
	pButtonSelect = gtk_button_new(); 
	gtk_widget_set_tooltip_text (pButtonSelect, "Select Audio File");
	image = gtk_image_new_from_stock (GTK_STOCK_FILE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(pButtonSelect),image);
	gtk_box_pack_start(GTK_BOX(pHBox[12]), pButtonSelect, TRUE, TRUE, 10);
	g_signal_connect(G_OBJECT(pButtonSelect), "clicked", G_CALLBACK(selectFile), NULL);

/* Test sound : frame, 'Play/Pause' button, frequency spin button and 'Volume' scale button */
	pFrame = gtk_frame_new("Test Sound");
	gtk_widget_set_tooltip_text (pFrame, "Play a pure sine wave sound");
	button = gtk_button_new_with_label("Play/Pause");
	gtk_widget_set_tooltip_text (button, "Play/Pause a pure sine wave sound");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), pFrame, FALSE, FALSE, 0);	
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[11]);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playTestSound), NULL);

	adjustment = gtk_adjustment_new(440, 1, 20000, 1, 100, 0);
	spinTestSound = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_widget_set_tooltip_text (spinTestSound, "Select frequency (in Herz) of the sine wave test sound");
	gtk_box_pack_start(GTK_BOX(pHBox[11]), spinTestSound, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (spinTestSound), "value_changed", G_CALLBACK (change_freq_test_sound),
NULL);
	
	volumeTestSound = gtk_volume_button_new ();
	gtk_box_pack_start(GTK_BOX(pHBox[11]), volumeTestSound, TRUE, TRUE, 0);
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(volumeTestSound), 0.8);
	g_signal_connect(G_OBJECT(volumeTestSound), "value-changed", G_CALLBACK(change_volume_test_sound), NULL);

/* GENERAL FRAME FOR AUDIO SETTINGS */
	GtkWidget *textLabel_Widget;
	char textLabel[64];
	sprintf(textLabel, "<big><b>%s</b></big>", "AUDIO SETTINGS");
	textLabel_Widget = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL (textLabel_Widget), textLabel);
	gdk_color_parse("dark slate blue", &colorFrame);
	pFrame = gtk_frame_new(NULL);
	gtk_widget_set_tooltip_text (pFrame, "AUDIO SETTINGS");
	gtk_frame_set_label_widget (GTK_FRAME(pFrame), textLabel_Widget);
	gtk_widget_modify_base (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_widget_modify_bg (pFrame, GTK_STATE_NORMAL, &colorFrame);
	//gtk_frame_set_label_align (GTK_FRAME(pFrame), 0.5, 1.0);
	//gtk_frame_set_shadow_type (GTK_FRAME(pFrame), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pFrame, TRUE, TRUE, 10);
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[7]);

/* 'Source' radio buttons */
	pFrame = gtk_frame_new("Source");
	gtk_widget_set_tooltip_text (pFrame, "Select audio source : microphone, audio file or Jack");
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pFrame, TRUE, TRUE, 10);	
	pRadio[0] = gtk_radio_button_new_with_label(NULL, "Mic");
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[8]);
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[0], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
	pRadio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Audio\n File");
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[1], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[1]), "toggled", G_CALLBACK(onSource), pRadio[0]);
#ifdef HAVE_LIBJACK
    	pRadio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[0]), "Jack");
    	gtk_box_pack_start(GTK_BOX (pHBox[8]), pRadio[2], TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(pRadio[0]), "toggled", G_CALLBACK(onSource), pRadio[0]);
#endif

/* "Speed of display" combo box */
        pFrame = gtk_frame_new("Interval (msec)");
	gtk_widget_set_tooltip_text (pFrame, "Defines the time interval (in milliseconds) before the display is refreshed");
	pComboSpeed = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboSpeed);
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pFrame, TRUE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "100");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "150");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "200");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "250");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "300");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "350");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "400");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "450");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboSpeed), "500");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboSpeed), 1);
	g_signal_connect( G_OBJECT(pComboSpeed), "changed", G_CALLBACK( cb_speed_changed ), NULL );

/* Button to open the Filter and Equalizer window */
	buttonEqualizer = gtk_button_new_with_label("Show Filter & \nEqualizer Window");  
	gtk_widget_set_tooltip_text (buttonEqualizer, "Show/Hide the filter and equalizer window");
	gtk_box_pack_start(GTK_BOX(pHBox[7]), buttonEqualizer, FALSE, FALSE, 20);
	g_signal_connect(G_OBJECT(buttonEqualizer), "clicked", G_CALLBACK(effects_window), NULL);

/* GENERAL FRAME DISPLAY SETTINGS*/
	sprintf(textLabel, "<big><b>%s</b></big>", "DISPLAY SETTINGS");
	textLabel_Widget = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL (textLabel_Widget), textLabel);
	pFrame = gtk_frame_new(NULL);
	gtk_widget_set_tooltip_text (pFrame, "DISPLAY SETTINGS");
	gtk_frame_set_label_widget (GTK_FRAME(pFrame), textLabel_Widget);
	gtk_widget_modify_base (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_widget_modify_bg (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pFrame, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(pFrame), pVBox[2]);

/* GENERAL FRAME FOR VALUES & ZOOM SETTINGS */
	gdk_color_parse("light slate grey", &colorFrame);
	sprintf(textLabel, "<b>%s</b>", "VALUES &amp; ZOOM SETTINGS");
	textLabel_Widget = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL (textLabel_Widget), textLabel);
	pFrame = gtk_frame_new(NULL);
	gtk_widget_set_tooltip_text (pFrame, "DISPLAY SETTINGS : VALUES & ZOOM SETTINGS");
	gtk_frame_set_label_widget (GTK_FRAME(pFrame), textLabel_Widget);
	gtk_widget_modify_base (pFrame, GTK_STATE_NORMAL, &colorFrame);
	//gtk_widget_modify_fg (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_widget_modify_bg (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_frame_set_label_align (GTK_FRAME(pFrame), 0.1, 0.4);
	gtk_box_pack_start(GTK_BOX(pVBox[2]), pFrame, TRUE, TRUE, 10);
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[4]);
	
/* Starting value of the display */
	pFrame = gtk_frame_new("Start value of display (in Hz)");
	gtk_widget_set_tooltip_text (pFrame, "The lower displayed frequency (in herz)");
	adjust_start = gtk_adjustment_new(0, 0, 5000, ((gdouble)hzStep * (gdouble)zoomFactor), (((gdouble)hzStep * (gdouble)zoomFactor) * 10), 0);
	pScaleStart = gtk_hscale_new(GTK_ADJUSTMENT(adjust_start));
	gtk_scale_set_digits (GTK_SCALE(pScaleStart), 0);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleStart);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(pScaleStart), "value-changed", G_CALLBACK(change_start), NULL);
	g_signal_connect(G_OBJECT(pScaleStart), "format-value", G_CALLBACK(format_value_start), NULL);

/* Range of display */
	pFrame = gtk_frame_new("Range of display (in Hz)");
	gtk_widget_set_tooltip_text (pFrame, "The range of the displayed frequency (in herz)");
	adjust_bands = gtk_adjustment_new((gdouble)widthFrame, 20, (gdouble)widthFrame, 20, 50, 0);
	pScaleBands = gtk_hscale_new(GTK_ADJUSTMENT(adjust_bands));
	gtk_scale_set_digits (GTK_SCALE(pScaleBands), 0);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleBands);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(pScaleBands), "value-changed", G_CALLBACK(change_bands), NULL);
	g_signal_connect(G_OBJECT(pScaleBands), "format-value", G_CALLBACK(format_value_bands), NULL);
	
/* "x" label */
	label=gtk_label_new("x");
	gtk_container_add(GTK_CONTAINER(pHBox[4]), label);

/* Factor that multiplies the range of display */
	pFrame = gtk_frame_new("");
	gtk_widget_set_tooltip_text (pFrame, "Factor that multiplies the frequency range, to make it larger");
	pComboRange = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboRange);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, FALSE, FALSE, 0);
	for (i = 1; i <= 20; i++){
		gchar text[4];
		sprintf(text, "%d", i);
		gtk_combo_box_append_text(GTK_COMBO_BOX(pComboRange), text);
		}
	gtk_combo_box_set_active(GTK_COMBO_BOX(pComboRange), 0);
	g_signal_connect( G_OBJECT(pComboRange), "changed", G_CALLBACK( cb_range_changed ), NULL );
	
/* Label that shows starting value, ending value and range of display */
	pFrame = gtk_frame_new("Values displayed");
	gtk_widget_set_tooltip_text (pFrame, "The lower and the highest displayed value (in herz), and their range");
	displayLabel=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(pFrame), displayLabel);
	getTextDisplayLabel();
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, FALSE, FALSE, 10);

/* GENERAL FRAME FOR ORIENTATION & SCALE SETTINGS*/
	sprintf(textLabel, "<b>%s</b>", "ORIENTATION &amp; SCALE SETTINGS");
	textLabel_Widget = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL (textLabel_Widget), textLabel);
	pFrame = gtk_frame_new(NULL);
	gtk_widget_set_tooltip_text (pFrame, "DISPLAY SETTINGS : ORIENTATION & SCALE SETTINGS");
	gtk_frame_set_label_widget (GTK_FRAME(pFrame), textLabel_Widget);
	gtk_widget_modify_base (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_widget_modify_bg (pFrame, GTK_STATE_NORMAL, &colorFrame);
	gtk_frame_set_label_align (GTK_FRAME(pFrame), 0.1, 0.4);
	gtk_box_pack_start(GTK_BOX(pVBox[2]), pFrame, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(pFrame), pVBox[3]);

/* "view' radio buttons */
	pFrame = gtk_frame_new("View");	
	gtk_widget_set_tooltip_text (pFrame, "Type of view : 3D or flat view");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pFrame, TRUE, TRUE, 0);	
	pRadio[4] = gtk_radio_button_new_with_label(NULL, "3D");
	gtk_container_add(GTK_CONTAINER(pFrame), pHBox[10]);
    	gtk_box_pack_start(GTK_BOX (pHBox[10]), pRadio[4], TRUE, FALSE, 0);
	gtk_widget_set_tooltip_text (pRadio[4], "3D view");

	pRadio[5] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[4]), "3D Flat");
	gtk_box_pack_start(GTK_BOX (pHBox[10]), pRadio[5], TRUE, FALSE, 0);
	gtk_widget_set_tooltip_text (pRadio[5], "3D 'flat' view");
	g_signal_connect(G_OBJECT(pRadio[5]), "toggled", G_CALLBACK(on_view), pRadio[4]);

	pRadio[6] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio[5]), "Flat");
    	gtk_box_pack_start(GTK_BOX (pHBox[10]), pRadio[6], TRUE, FALSE, 0);
	gtk_widget_set_tooltip_text (pRadio[6], "Flat (2D) view, with intensity beeing represented as the amount of red colour");
	g_signal_connect(G_OBJECT(pRadio[6]), "toggled", G_CALLBACK(on_view), pRadio[4]);

/* 'Reset', 'Front' and 'Preset' buttons */
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pHBox[9], FALSE, FALSE, 0);
	button = gtk_button_new_with_label("Reset");
	gtk_widget_set_tooltip_text (button, "Reset the view as if Spectrum3d was just started");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(onReset), NULL);

	button = gtk_button_new_with_label("Front");
	gtk_widget_set_tooltip_text (button, "Show a front flat view of the 3D spectrogram");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(onFrontView), NULL);

	button = gtk_button_new_with_label("Preset");
	gtk_widget_set_tooltip_text (button, "Shows the 3D spectrogram with saved coordinates");
	gtk_box_pack_start(GTK_BOX(pHBox[9]), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(onPreset), NULL);
	
/* 'Text', 'Lines' and 'Pointer' */
	gtk_box_pack_start(GTK_BOX(pVBox[3]), pHBox[5], TRUE, TRUE, 0);
	pCheckTextScale = gtk_check_button_new_with_label("Text");
	gtk_widget_set_tooltip_text (pCheckTextScale, "Show the frequency scale");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckTextScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckTextScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckTextScale), "toggled", G_CALLBACK(onCheckTextScale), NULL);

	pCheckLineScale = gtk_check_button_new_with_label("Lines");
	gtk_widget_set_tooltip_text (pCheckLineScale, "Show the lines of the scale");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), pCheckLineScale, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(pCheckLineScale), TRUE);
	g_signal_connect(G_OBJECT(pCheckLineScale), "toggled", G_CALLBACK(onCheckLineScale), NULL);

	checkPointer = gtk_check_button_new_with_label("Pointer");
	gtk_widget_set_tooltip_text (checkPointer, "Show a pointer that defines precisely a frequency");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), checkPointer, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(checkPointer), "toggled", G_CALLBACK(onCheckPointer), NULL);

/* 'Depth' Gtk Scale */
	pFrame = gtk_frame_new("Depth");
	gtk_widget_set_tooltip_text (pFrame, "Adjust the number of frames in depth");
	gtk_widget_set_size_request (pFrame, 100, 20);
	pScaleDepth = gtk_hscale_new_with_range(0, 2, 0.1);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleDepth);
	gtk_box_pack_start(GTK_BOX(pHBox[6]), pFrame, FALSE, FALSE, 0);
	gtk_range_set_value(GTK_RANGE(pScaleDepth), 1);
	g_signal_connect(G_OBJECT(pScaleDepth), "value-changed", G_CALLBACK(changeDepth), NULL);

/* 'Gain' Gtk Scale */
	gtk_box_pack_start(GTK_BOX(pVBox[3]), pHBox[6], FALSE, FALSE, 10);
	pFrame = gtk_frame_new("Gain");
	gtk_widget_set_tooltip_text (pFrame, "Adjust the displayed level of the intensity of the sound");
	pScaleGain = gtk_hscale_new_with_range(0, 0.8, 0.001);
	gtk_container_add(GTK_CONTAINER(pFrame), pScaleGain);
	gtk_box_pack_start(GTK_BOX(pHBox[6]), pFrame, TRUE, TRUE, 0);
	gtk_range_set_value(GTK_RANGE(pScaleGain), 0.2);
	g_object_set (pScaleGain, "update-policy", GTK_UPDATE_DISCONTINUOUS, NULL);
	g_signal_connect(G_OBJECT(pScaleGain), "change-value", G_CALLBACK(changeGain), NULL);

	gtk_widget_show_all (mainWindow);
	gtk_main ();

	return 0;
}



