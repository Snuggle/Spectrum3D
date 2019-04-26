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

#include "record.h"

void record_window() {
	GtkWidget *hbox[2], *recordWindow, *button, *image, *content_area;

	if (typeSource == AUDIO_FILE){
		error_message_window("Source should be set on microphone to record sound");
		return;
		}
	
	recordWindow = gtk_dialog_new_with_buttons("Record",
		GTK_WINDOW(NULL),
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	gtk_window_set_default_size(GTK_WINDOW(recordWindow), 250, 50);

#ifdef GTK3
	hbox[0] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	hbox[1] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#elif defined GTK2
	hbox[0] = gtk_hbox_new(TRUE, 10);
	hbox[1] = gtk_hbox_new(TRUE, 0);
#endif	
	
#ifdef GTK3
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (recordWindow));
	gtk_container_add (GTK_CONTAINER (content_area), hbox[0]);
#elif defined GTK2
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(recordWindow)->vbox), hbox[0], TRUE, TRUE, 0);
#endif			

	button = gtk_button_new();
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_RECORD, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button),image);
	gtk_box_pack_start(GTK_BOX(hbox[0]), button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playFromSource), NULL);
	
	button = gtk_button_new();
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button),image);
	gtk_box_pack_start(GTK_BOX(hbox[0]), button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_stop), NULL);
	
  	gtk_widget_show_all (recordWindow);

	analyse_rt = TRUE; // analyse_rt is set to TRUE for the time of recording in order that spectrum values can passed to one another at the end of the displaySpectro() function; it will set back to FALSE when record window is destroyed;
	recording = TRUE; // recording is set to TRUE; it will be set to FALSE when the recordWindow is detroyed;

	switch (gtk_dialog_run(GTK_DIALOG(recordWindow)))
		{
		case GTK_RESPONSE_OK:
			recording = FALSE;
			analyse_rt = FALSE;
			selected_file = g_build_filename (tmpPath, NULL);
			printf("tmpPath is %s\n", selected_file);
    			load_audio_file("rec");
		break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
		    break;
	}
	analyse_rt = FALSE;
	recording = FALSE;
	gtk_widget_destroy(recordWindow);	
}



