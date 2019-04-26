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

#include "config.h"

#include "menu.h"

/* Select font */
void selectFont(GtkWidget *comboColor, gpointer data)
{
	GtkFileFilter *filter = gtk_file_filter_new();
	GtkWidget *pFileSelection;
	GtkWidget *pParent;
	pParent = GTK_WIDGET(data);
	gchar *selectedFont;

	pFileSelection = gtk_file_chooser_dialog_new("Select File",
	GTK_WINDOW(pParent),
	GTK_FILE_CHOOSER_ACTION_OPEN,
	GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	GTK_STOCK_OPEN, GTK_RESPONSE_OK,
	NULL);
	gtk_file_filter_set_name(filter, ".ttf Files");
	gtk_file_filter_add_pattern(filter, "*.ttf");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(pFileSelection), filter);
	gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

	switch(gtk_dialog_run(GTK_DIALOG(pFileSelection))) {
		case GTK_RESPONSE_OK:
			selectedFont = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
			sprintf(fontPreference, "%s", selectedFont);
			printf("Selected font is %s\n", fontPreference);
		    break;
		default:
		    break;
		}
	gtk_widget_destroy(pFileSelection);
}

/* Preferences window */
void onPreferences(GtkWidget* widget, gpointer data)
{
	GtkWidget *pPreferences, *pNotebook, *pTabLabel;
    	GtkWidget *pSpinWidth, *pLabel, *pCheckRealtime, *pComboPolicy, *pSpinPriority, *pCheckPreset, *pCheckEnableTouch, *spinInterval, *comboColor, *checkCopyPixels, *comboFlatviewDefinition;
	GtkWidget *pFrame, *pButtonSelectFont;
	gboolean bState;
	int i = 0;
	gchar *sTabLabel[3];
	GtkWidget *pVBox[3];
	for (i = 0; i < 3; i++) {
		pVBox[i] = gtk_vbox_new(TRUE, 0);
		}
	GtkWidget *pHBox[25];
	for (i = 0; i < 25; i++) {
		pHBox[i] = gtk_hbox_new(TRUE, 0);
		}
		
        pPreferences = gtk_dialog_new_with_buttons("Preferences",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(pPreferences), 300, 300);

	pNotebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pNotebook, TRUE, TRUE, 0);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(pNotebook), GTK_POS_TOP);
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(pNotebook), TRUE);

///////////////////////// 1st Notebook : "Display" //////////////////////
	
        sTabLabel[0] = g_strdup_printf("Display");
	pTabLabel = gtk_label_new(sTabLabel[0]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[0], pTabLabel);
	g_free(sTabLabel[0]);

	/* SpinButton for choosing the width of the main window */
	pFrame = gtk_frame_new("Width of the window");
	pSpinWidth = gtk_spin_button_new_with_range(700, 1500, 50);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[0], TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(pHBox[0]), pFrame, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinWidth);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinWidth), (float)presetWidth); 
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[1], TRUE, FALSE, 0);

	/* Button to select the font */
	pButtonSelectFont = gtk_button_new_with_mnemonic("Select Font"); 
	gtk_box_pack_start(GTK_BOX(pHBox[2]), pButtonSelectFont, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[2], FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButtonSelectFont), "clicked", G_CALLBACK(selectFont), NULL);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pPreferences)->vbox), pHBox[3], TRUE, FALSE, 0);

	/* Check button to select use of CopyPixels */
	checkCopyPixels = gtk_check_button_new_with_label("Use CopyPixels\n for Flat View");
	gtk_widget_set_tooltip_text (checkCopyPixels, "Use the 'glCopyPixels' function for Flat View; this will make the CPU load much lower");
	if (useCopyPixels) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkCopyPixels), TRUE);
		}
	gtk_box_pack_start(GTK_BOX(pHBox[3]), checkCopyPixels, TRUE, TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[3], FALSE, FALSE, 0);

	/* Combo box to change the definition of the display of the Flat View*/
	pFrame = gtk_frame_new("Definition of Display of Flat View");
	comboFlatviewDefinition = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), comboFlatviewDefinition);
	gtk_box_pack_start(GTK_BOX(pHBox[3]), pFrame, TRUE, TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[3], FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboFlatviewDefinition), "Low"); // flatviewDefinition = 200
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboFlatviewDefinition), "Medium"); // flatviewDefinition = 300
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboFlatviewDefinition), "High"); // flatviewDefinition = 400
	gtk_combo_box_set_active(GTK_COMBO_BOX(comboFlatviewDefinition), 2);


	/* Combo box to change the color of the display */
	pFrame = gtk_frame_new("Color of Display");
	comboColor = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), comboColor);
	gtk_box_pack_start(GTK_BOX(pHBox[4]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[4], FALSE, FALSE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboColor), "PURPLE");
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboColor), "RED");
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboColor), "RAINBOW");
	gtk_combo_box_set_active(GTK_COMBO_BOX(comboColor), 0);

	/* Check button to save the current position as 'Preset' */
	pCheckPreset = gtk_check_button_new_with_label("Save current position as preset");
	gtk_box_pack_start(GTK_BOX(pHBox[7]), pCheckPreset, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[0]), pHBox[7], FALSE, FALSE, 0);

///////////////////////// 2d Notebook : "Audio" //////////////////////
	
	sTabLabel[1] = g_strdup_printf("Audio");
	pTabLabel = gtk_label_new(sTabLabel[1]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[1], pTabLabel);
	g_free(sTabLabel[1]);

	/* SpinButton for choosing the speed */
	pFrame = gtk_frame_new("Interval (msec)");
	spinInterval = gtk_spin_button_new_with_range(100, 250, 50);
	 gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinInterval), (gdouble)interval);
	gtk_container_add(GTK_CONTAINER(pFrame), spinInterval);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), pFrame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[11], FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spinInterval), interval);

	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[13], FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[14], FALSE, FALSE, 0);

	/* Realtime */
	pCheckRealtime = gtk_check_button_new_with_label("Enable realtime \n(without Jack)");
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pCheckRealtime, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[15], TRUE, FALSE, 0);
	//realtime = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckRealtime));
	if (realtime) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pCheckRealtime), TRUE);
		}

	pFrame = gtk_frame_new("Realtime Policy");
	pComboPolicy = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(pFrame), pComboPolicy);
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pFrame, TRUE, TRUE, 0);
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboPolicy), "SCHED_RR");
	gtk_combo_box_append_text(GTK_COMBO_BOX(pComboPolicy), "SCHED_FIFO");
	if (strstr(policyName, "FIFO")){
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboPolicy), 1);
		}
  	else {
		gtk_combo_box_set_active(GTK_COMBO_BOX(pComboPolicy), 0);
		}
	
	pFrame = gtk_frame_new("Priority");
	pSpinPriority = gtk_spin_button_new_with_range(50, 80, 1);
	gtk_container_add(GTK_CONTAINER(pFrame), pSpinPriority);
	gtk_box_pack_start(GTK_BOX(pHBox[15]), pFrame, FALSE, FALSE, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(pSpinPriority), priority);

#ifdef HAVE_LIBUTOUCH_GEIS
///////////////////////// 3d Notebook : "Touch" //////////////////////
	
	sTabLabel[2] = g_strdup_printf("Touch");
	pTabLabel = gtk_label_new(sTabLabel[2]);
	gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBox[2], pTabLabel);
	g_free(sTabLabel[2]);

	pCheckEnableTouch = gtk_check_button_new_with_label("Enable Touch");
	gtk_box_pack_start(GTK_BOX(pHBox[21]), pCheckEnableTouch, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[2]), pHBox[21], TRUE, FALSE, 0);
	//realtime = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckRealtime));
	if (enableTouch) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pCheckEnableTouch), TRUE);
		}
#endif

//////////////////////// End of the Notebooks //////////////////////////

	gtk_widget_show_all(GTK_DIALOG(pPreferences)->vbox);
	
    	switch (gtk_dialog_run(GTK_DIALOG(pPreferences)))
		{
		case GTK_RESPONSE_OK:
    			presetWidth = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pSpinWidth));
				change_adjust(NULL, NULL);
			//	fprintf(pref, "%d\n", presetWidth);
				width = presetWidth;
				//fprintf(pref, "%s\n", fontPreference);
			bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkCopyPixels));
				if (bState) {
					useCopyPixels = TRUE;
				}
				else {
					useCopyPixels = FALSE;
					}
			gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(comboFlatviewDefinition));
				if (index == 0){
					flatviewDefinition = 200;
					}
				else if (index == 1){
					flatviewDefinition = 300;
					}
				else if (index == 2){
					flatviewDefinition = 400;
					}
			colorType = gtk_combo_box_get_active(GTK_COMBO_BOX(comboColor));
				 // ColorType enum is detailed in menu.h
			bState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckPreset));
				if (bState) {
					presetX = X;
					presetY = Y;
					presetZ = Z;
					presetAngleH = AngleH;
					presetAngleV = AngleV;
					presetAngleZ = AngleZ;
				}
			interval = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinInterval));
			realtime = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckRealtime));
			gchar *string = gtk_combo_box_get_active_text(GTK_COMBO_BOX(pComboPolicy));
			sprintf(policyName, "%s", string);
			priority = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pSpinPriority));
			enableTouch = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pCheckEnableTouch));
		break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
		    break;
	}
	gtk_widget_destroy(pPreferences);	
}

/* Keyboard and Mouse shortcuts menu */
void onShortcuts (GtkWidget* widget, gpointer data)
{
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[15] = {"Play/Pause", "Stop", "Rotate around X axis", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis", "Increase/decrease Gain", "Change starting value of the zoom", "Read small 50 msec samples	of a paused audio file", "Move pointer up/down", "Move pointer up/down fast"	 };
	gchar *keyboard[15] = {"Space bar", "Escape", "Up/Down", "Right/Left", "'c' + Up/Down", "'x' + Left/Right", "'y' + Up/Down", "'z' + Up/Down", "'g' + Up/Down", "'s' + Up/Down", "'v' + Left", "SHIFT + right/left", "SHIFT + CTRL + right/left" };
	gchar *mouse[15] = {"", "", "Mouse Up/Down", "Mouse Right/Left", "'c' + mouse Up/Down", "'x' + mouse Right/Left", "'y' + mouse Up/Down", "'z' + mouse Up/Down", "'g' + mouse Up/Down" };
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 700, 400);
	
	pListStore = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	for(i = 0 ; i < 15 ; ++i){
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
	gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pScrollbar);


	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
	
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
	GtkWidget *shortcutsWindow, *pListView, *pScrollbar;
	GtkListStore *pListStore;
	GtkTreeViewColumn *pColumn;
	GtkCellRenderer *pCellRenderer;
	gchar *action[8] = {"Play/Pause", "Rotate around X axis                   ", "Rotate around Y axis", "Rotate around Z axis", "Translate along X axis", "Translate along Y axis", "Translate along Z axis"};
	gchar *keyboard[8] = {"Double Tap", "1 Finger Drag Up/Down", "1 Finger Drag Right/Left", "2 Fingers Rotate", "2 Fingers Drag Right/Left", "2 Fingers Drag Up/Down", "2 Fingers Pinch/Spread"};
	gint i;

    	shortcutsWindow = gtk_dialog_new_with_buttons("Keyboard and mouse shortcuts",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(shortcutsWindow), 500, 300);
	
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
	gtk_container_add(GTK_CONTAINER(pScrollbar), pListView);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(shortcutsWindow)->vbox), pScrollbar);


	gtk_widget_show_all(GTK_DIALOG(shortcutsWindow)->vbox);
	
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
	GtkWidget *quickStartWindow, *label;
	quickStartWindow = gtk_dialog_new_with_buttons("Spectrum3d : Quick Start",
        GTK_WINDOW(mainWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        NULL);
	gtk_window_set_default_size(GTK_WINDOW(quickStartWindow), 500, 300);

	gchar *quickStartText = " * 3 views are possible : 3D, 3D'Flat' or 2D (flat view);\n * position of display and perspective can be translated or rotated along the 3 axis (see 'Shortcuts' in menu)\n * zoom can be adjusted :\n       - first displayed frequency can have almost any value;\n       - the range of displayed frequencies can vary from 40 to 20000 Hz;\n * scale (text and line) and a pointer pointing to an exact location can be displayed;\n * perspective and zoom can be changed even when playing is paused;\n\n * if the load of the CPU is too big, you can reduce it by :\n    - set the width smaller;\n    - set the depth and the speed smaller;\n    - check the 'Use CopyPixels for FlatView' box in Preferences->Display";
	label = gtk_label_new (quickStartText);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(quickStartWindow)->vbox), label);
	gtk_widget_show_all(GTK_DIALOG(quickStartWindow)->vbox);
	
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
        "\nhttp://www.presences.org/spectrum3d");
    gtk_dialog_run(GTK_DIALOG(pAbout));
    gtk_widget_destroy(pAbout);
}



