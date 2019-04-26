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

#include "config.h"
#include "spectrum3d.h"
#include "menu.h"

/* Preferences window */
void onPreferences(GtkWidget* widget, Spectrum3dGui *spectrum3dGui)
{
	gboolean bState;
	int i = 0;
	gchar *sTabLabel[3];
	GtkWidget *pPreferences, *pNotebook, *pTabLabel, *label, *spinFrames, *spinZStep, *spinDisplayInterval, *spinSpectrumInterval, *comboColor, *pCheckRealtime, *pComboPolicy, *pSpinPriority, *pCheckPreset, *pCheckEnableTouch, *pFrame, *content_area;
	GtkWidget *pVBox[3];
	for (i = 0; i < 3; i++) {
		pVBox[i] = gtk_vbox_new(FALSE, 4);
		}
	GtkWidget *pHBox[25];
	for (i = 0; i < 25; i++) {
		pHBox[i] = gtk_hbox_new(FALSE, 4);
		}
		
        pPreferences = gtk_dialog_new_with_buttons("Preferences",
			NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_default_size(GTK_WINDOW(pPreferences), 300, 300);

	pNotebook = gtk_notebook_new();

	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(pNotebook), GTK_POS_TOP);
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(pNotebook), TRUE);

///////////////////////// 1st Notebook : "Display" //////////////////////////
	
        sTabLabel[0] = g_strdup_printf("Display");
	pTabLabel = gtk_label_new(sTabLabel[0]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[0], pTabLabel);
	g_free(sTabLabel[0]);

	/* SpinButton for choosing the distance between frames (spectrum3d.zStep) in 3D mode */
	pFrame = gtk_frame_new("Distance between frames");
	spinZStep = gtk_spin_button_new_with_range(0.005, 0.03, 0.005);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinZStep), (gdouble)spectrum3d.zStep);
	gtk_widget_set_tooltip_text (pFrame, "Set the distance between frames; the biggest this value is, the deepest the image will go");
	gtk_container_add(GTK_CONTAINER(pFrame), spinZStep);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pFrame, TRUE, TRUE, 0);

	/* SpinButton for choosing the number of frames (spectrum3d.frames) displayed */
	pFrame = gtk_frame_new("Number of displayed frames");
	spinFrames = gtk_spin_button_new_with_range(50, 400, 50);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinFrames), (gdouble)spectrum3d.frames);
	gtk_widget_set_tooltip_text (pFrame, "Set the number of displayed frames; the lower this value is, the less cpu is used");
	gtk_container_add(GTK_CONTAINER(pFrame), spinFrames);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pFrame, TRUE, TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[1], FALSE, FALSE, 0);

	label = gtk_label_new("WARNING : the lower the number of frames is, \n\
	the less cpu is used");	
	gtk_box_pack_start(GTK_BOX(pVBox[0]), label, FALSE, FALSE, 0);

	/* SpinButton for choosing the interval of time (in milliseconds) between each display of the spectrum */
	pFrame = gtk_frame_new("Interval between each spectrum display (msec)");
	spinDisplayInterval = gtk_spin_button_new_with_range(100, 500, 50);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinDisplayInterval), (gdouble)spectrum3d.interval_display);
	gtk_widget_set_tooltip_text (pFrame, "Set the interval of time (in milliseconds) between each spectrum display; the lower this value is, the lower cpu is used");
	gtk_container_add(GTK_CONTAINER(pFrame), spinDisplayInterval);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pFrame, TRUE, TRUE, 0);

	label = gtk_label_new("WARNING : the lower the value, the more cpu is used; \n ideally, this value should match the 'Interval \nbetween each spectrum analysis (msec)' value");	
	gtk_box_pack_start(GTK_BOX(pVBox[0]), label, FALSE, FALSE, 0);

	/* Combo box to change the color of the display */
	pFrame = gtk_frame_new("Color of Display");
	comboColor = gtk_combo_box_text_new();
	gtk_container_add(GTK_CONTAINER(pFrame), comboColor);
	gtk_widget_set_tooltip_text (pFrame, "Set the color of display in 'analyse in real-time' mode");
	gtk_box_pack_start(GTK_BOX(pHBox[3]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[3], FALSE, FALSE, 20);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboColor), "PURPLE");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboColor), "RED");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboColor), "RAINBOW");
	gtk_combo_box_set_active(GTK_COMBO_BOX(comboColor), 0);

	/* Check button to save the current position as 'Preset' */
	pCheckPreset = gtk_check_button_new_with_label("Save current position as preset");
	gtk_widget_set_tooltip_text (pCheckPreset, "Save the current position of display as preset");
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pCheckPreset, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[7], FALSE, FALSE, 0);

///////////////////////// 2d Notebook : "Audio" //////////////////////
	
	sTabLabel[1] = g_strdup_printf("Audio");
	pTabLabel = gtk_label_new(sTabLabel[1]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[1], pTabLabel);
	g_free(sTabLabel[1]);

	/* SpinButton for choosing the "interval" property of the 'spectrum' element of Gstreamer*/
	pFrame = gtk_frame_new("Interval time between each spectrum analysis (msec)");
	spinSpectrumInterval = gtk_spin_button_new_with_range(100, 500, 50);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinSpectrumInterval), (gdouble)spectrum3d.interval_rt);
	gtk_widget_set_tooltip_text (pFrame, "Set the interval of time (in milliseconds) between each spectrum analysis; the lower this value is, the lower cpu is used");
	gtk_container_add(GTK_CONTAINER(pFrame), spinSpectrumInterval);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pFrame, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[11], FALSE, FALSE, 0);

	label = gtk_label_new("WARNING : The lower the value is, the less\n cpu is 	used; ideally, this value should match the 'Interval \nbetween each spectrum display' value");	
	gtk_box_pack_start(GTK_BOX(pVBox[1]), label, FALSE, FALSE, 0);

	/* Realtime */
	pCheckRealtime = gtk_check_button_new_with_label("Enable realtime \n(without Jack)");
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pCheckRealtime, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[15], TRUE, FALSE, 0);
	if (spectrum3d.realtime) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pCheckRealtime), TRUE);
		}

	pFrame = gtk_frame_new("Realtime Policy");
	pComboPolicy = gtk_combo_box_text_new();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboPolicy);
	gtk_widget_set_tooltip_text (pFrame, "Set the policy of realtime mode (when Jack is not used)");
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pFrame, TRUE, TRUE, 0);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pComboPolicy), "SCHED_RR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pComboPolicy), "SCHED_FIFO");
	if (strstr(policyName, "FIFO")){
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboPolicy), 1);
		}
  	else {
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboPolicy), 0);
		}
	
	pFrame = gtk_frame_new("Priority");
	pSpinPriority = gtk_spin_button_new_with_range(50, 80, 1);
	gtk_widget_set_tooltip_text (pFrame, "Set the priority if realtime mode is used(whithout Jack)");
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinPriority);
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pFrame, FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinPriority), spectrum3d.priority);

#ifdef HAVE_LIBUTOUCH_GEIS
///////////////////////// 3d Notebook : "Touch" //////////////////////
	
	sTabLabel[2] = g_strdup_printf("Touch");
	pTabLabel = gtk_label_new(sTabLabel[2]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[2], pTabLabel);
	g_free(sTabLabel[2]);

	pCheckEnableTouch = gtk_check_button_new_with_label("Enable Touch");
	gtk_widget_set_tooltip_text (pCheckEnableTouch, "Enable touch input");
	gtk_box_pack_start(GTK_BOX(pHBox[21]), pCheckEnableTouch, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[2]), pHBox[21], TRUE, FALSE, 0);
	//realtime = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckRealtime));
	if (spectrum3d.enableTouch) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pCheckEnableTouch), TRUE);
		}
#endif

//////////////////////// End of the Notebooks //////////////////////////

#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (pPreferences));
	gtk_container_add (GTK_CONTAINER (content_area), pNotebook);
	gtk_widget_show_all(pPreferences); 
#elif defined GTK2
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pNotebook, TRUE, TRUE, 0);
	gtk_widget_show_all(GTK_DIALOG(pPreferences)->vbox);
#endif
	
	
    	switch (gtk_dialog_run(GTK_DIALOG(pPreferences)))
		{
		case GTK_RESPONSE_OK:
			// get new spectrum3d.zStep & spectrum3d.frames values and reinitialise z with those new values
			/// spectrum3d.zStep = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spinZStep)); GTK3
			spectrum3d.zStep = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinZStep));
			spectrum3d.frames = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinFrames));	
			spectrum3d.previousFrames = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinFrames));
			z = (float)spectrum3d.frames * spectrum3d.zStep;
			// get new spectrum3d.interval_display value, then stop & and restart timeout that calls display_spectro() with the new time interval value
			spectrum3d.interval_display = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinDisplayInterval));	
			guint intervalDisplaySpectro = (guint)spectrum3d.interval_display; // since the time interval is a guint, spectrum3d.interval_rt has to be casted; 
			g_source_remove(spectrum3d.timeoutExpose);
			spectrum3d.timeoutExpose = g_timeout_add (intervalDisplaySpectro, (GSourceFunc)display_spectro, spectrum3dGui);
			// get other values
			colorType = gtk_combo_box_get_active(GTK_COMBO_BOX(comboColor));
				 // ColorType enum is detailed in menu.h
			bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckPreset));
				if (bState) {
					spectrum3d.presetX = X;
					spectrum3d.presetY = Y;
					spectrum3d.presetZ = Z;
					spectrum3d.presetAngleH = AngleH;
					spectrum3d.presetAngleV = AngleV;
					spectrum3d.presetAngleZ = AngleZ;
				}
			spectrum3d.interval_rt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinSpectrumInterval));
			spectrum3d.realtime = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckRealtime));
			gchar *string = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(pComboPolicy));
			sprintf(policyName, "%s", string);
			spectrum3d.priority = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pSpinPriority));
			spectrum3d.enableTouch = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckEnableTouch));
			newEvent = TRUE;
		break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
		    break;
	}
	gtk_widget_destroy(pPreferences);	
}

void test_sound_window_destroy(GtkDialog *dialog, gint response_id, gpointer user_data) {
	if (loopTestSound != NULL){
		g_main_loop_quit(loopTestSound);
		}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void test_sound_window(GtkWidget *pWidget, gpointer *data){
	GtkWidget *dialog, *widget, *hbox, *content_area;
#ifdef GTK3
	GtkAdjustment *adjustment;
#elif defined GTK2
	GtkObject *adjustment;
#endif
	dialog = gtk_dialog_new_with_buttons ("Test sound", NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
			NULL);
	hbox = gtk_hbox_new (FALSE, 15);
	
	gtk_widget_set_tooltip_text (dialog, "Play a pure sine wave sound");
	widget = gtk_button_new_with_label("Play/Pause");
	gtk_widget_set_tooltip_text (widget, "Play/Pause a pure sine wave sound");
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 4);	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(playTestSound), NULL);

	adjustment = gtk_adjustment_new(440, 1, 20000, 1, 100, 0);
	widget = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_widget_set_tooltip_text (widget, "Select frequency (in Herz) of the sine wave test sound");
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 4);
	g_signal_connect (G_OBJECT (widget), "value_changed", G_CALLBACK (change_freq_test_sound),
NULL);

	widget = gtk_label_new("Hz");
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
	
	widget = gtk_volume_button_new ();
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 4);
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(widget), 0.8);
	g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(change_volume_test_sound), NULL);
#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	gtk_container_add (GTK_CONTAINER (content_area), hbox);
#elif defined GTK2
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), hbox);

#endif
	gtk_widget_show_all (dialog);

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (test_sound_window_destroy), NULL);
}

/* Keyboard and Mouse shortcuts menu */
void onShortcuts (GtkWidget* widget, gpointer data)
{
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar, *content_area;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[16] = {"Play/Pause", "Stop", "Rotate around X axis", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis", "Increase/decrease Gain", "Move pointer up/down or right/left", "Move pointer up/down or right/left fast", "Show/hide text/line/pointer", "Reset view / Front view"};
	gchar *keyboard[16] = {"Space bar", "Escape", "Up/Down", "Right/Left", "SHIFT + Up/Down", "CTRL + Left/Right", "CTRL + Up/Down", "SHIFT + CTRL + Up/Down", "'G' + Up/Down", "'Q' + up/down or right/left", "'A' + up/down or right/left", "'T' / 'L' / 'P'", "'R' / 'O'"};
	gchar *mouse[16] = {"", "", "Left Click + Mouse Up/Down", "Left Click + Mouse Right/Left", "Left Click + scroll Up/Down", "Right Click + mouse Right/Left", "Right Click + mouse Up/Down", "Right Click + scroll Up/Down", "'G' + mouse Up/Down" };
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
			NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 500, 160);
	
	pListStore = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	for(i = 0 ; i < 13 ; ++i){
		GtkTreeIter pIter;
		gtk_list_store_append(pListStore, &pIter);
		gtk_list_store_set(pListStore, &pIter,
		    ACTION_COLUMN, action[i],
		    KEYBOARD_COLUMN, keyboard[i],
		    MOUSE_COLUMN, mouse[i],
		    -1);
	}
	
	pListView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pListStore));

	/* 1st column */
        pCellRenderer = gtk_cell_renderer_text_new();
    	pColumn = gtk_tree_view_column_new_with_attributes("ACTION",
        	pCellRenderer,
		"text", ACTION_COLUMN,
		NULL);

    	gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 2d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("KEYBORD SHORTCUT",
		pCellRenderer,
		"text", KEYBOARD_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 3d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("MOUSE SHORTCUT",
		pCellRenderer,
		"text", MOUSE_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	//gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);

#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (shortcutsWindow));
	//hbox = gtk_hbox_new(TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox), pScrollbar, TRUE, TRUE, 4);
	gtk_container_add (GTK_CONTAINER (content_area), pListView);
	gtk_widget_show_all(shortcutsWindow);
#elif defined GTK2
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pListView);
	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
#endif
	
    	switch (gtk_dialog_run(GTK_DIALOG(shortcutsWindow)))
		{
		case GTK_RESPONSE_OK:
	    		break;
		default:
		    break;
	}
	gtk_widget_destroy(shortcutsWindow);
}
 
/* Gestures menu */
void onGesturesShortcuts (GtkWidget* widget, gpointer data)
{
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar, *content_area;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[8] = {"Play/Pause", "Rotate around X axis", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis"};
	gchar *keyboard[8] = {"Double Tap", "1 Finger Drag Up/Down", "1 Finger Drag Right/Left", "2 Fingers Rotate", "2 Fingers Drag Right/Left", "2 Fingers Drag Up/Down", "2 Fingers Pinch/Spread"};
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
			NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 480, 240);
	
	pListStore = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	for(i = 0 ; i < 8 ; ++i){
		GtkTreeIter pIter;
		gtk_list_store_append(pListStore, &pIter);
		gtk_list_store_set(pListStore, &pIter,
		    ACTION_COLUMN, action[i],
		    KEYBOARD_COLUMN, keyboard[i],
		    -1);
	}
	
	pListView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pListStore));

	/* 1st column */
        pCellRenderer = gtk_cell_renderer_text_new();
    	pColumn = gtk_tree_view_column_new_with_attributes("ACTION",
        	pCellRenderer,
		"text", ACTION_COLUMN,
		NULL);

    	gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	/* 2d column */
	pCellRenderer = gtk_cell_renderer_text_new();
   	pColumn = gtk_tree_view_column_new_with_attributes("GESTURE",
		pCellRenderer,
		"text", KEYBOARD_COLUMN,
		NULL);

        gtk_tree_view_append_column(GTK_TREE_VIEW(pListView), pColumn);

	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	//gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);
#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (shortcutsWindow));
	gtk_container_add(GTK_CONTAINER(content_area), pListView);
	gtk_widget_show_all(shortcutsWindow);
#elif defined GTK2
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pListView);
	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
#endif
	
    	switch (gtk_dialog_run(GTK_DIALOG(shortcutsWindow)))
		{
		case GTK_RESPONSE_OK:
	    		break;
		default:
		    break;
	}
	gtk_widget_destroy(shortcutsWindow);
}

void onQuickStart(GtkWidget* widget, gpointer data)
{
	GtkWidget *quickStartWindow, *label, *content_area;
	quickStartWindow = gtk_dialog_new_with_buttons("Spectrum3d : Quick Start",
			NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			NULL);
	gtk_window_set_default_size(GTK_WINDOW(quickStartWindow), 500, 300);

	gchar *quickStartText = "* source has to be chosen first: audio file or microphone;\n\
 * if 'Analyse in real time' is checked, harmonics will be analysed displayed \n\
on the fly, directly while playing, either from microphone or from an audio file\n\
 that you have chosen;\
 * if 'Analyse in real time' is unchecked, harmonics will be analysed and displayed\n\
 first, then the file can be played afterwards, either from a selected audio file, \n\
or from a recorded file from the microphone;\
 * the JACK server can be used for eveything said above : check 'Use JACK';\n\
 * 3 views are possible : 3D, 3D'Flat' or 2D (flat view);\n\
 * in 3D, position of display and perspective can be translated or rotated along \n\
the 3 axis (see 'Shortcuts' in menu)\n\
 * zoom can be adjusted :\n\
       - first displayed frequency can have almost any value;\n\
       - the range of displayed frequencies can vary from 40 to 20000 Hz;\n\
 * scale (text and line) and a pointer pointing to an exact location can be displayed;\n\
 * perspective and zoom can be changed even when playing is paused.";

	label = gtk_label_new (quickStartText);
#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (quickStartWindow));
	gtk_container_add (GTK_CONTAINER (content_area), label);
	gtk_widget_show_all(quickStartWindow);
#elif GTK2
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(quickStartWindow)->vbox), label);
	gtk_widget_show_all(GTK_DIALOG(quickStartWindow)->vbox);
#endif
	
	
    	switch (gtk_dialog_run(GTK_DIALOG(quickStartWindow)))
		{
		case GTK_RESPONSE_OK:
	    		break;
		default:
		    break;
	}
	gtk_widget_destroy(quickStartWindow);
}

/* About menu */
void onAbout(GtkWidget* widget, gpointer data)
{
    GtkWidget *pAbout;
    pAbout = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        PACKAGE_STRING
        "\n  " PACKAGE_URL
	"\n    " PACKAGE_BUGREPORT);
    gtk_dialog_run(GTK_DIALOG(pAbout));
    gtk_widget_destroy(pAbout);
}



