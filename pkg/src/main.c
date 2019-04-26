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
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "config.h"

#if defined HAVE_LIBSDL2
    #include <SDL2/SDL.h>
#elif defined HAVE_LIBSDL
    #include <SDL/SDL.h>
#endif

#if defined (GTKGLEXT3) || defined (GTKGLEXT1)
    #include <gtk/gtkgl.h>
#endif

#include "spectrum3d.h"
#include "main.h"

char prefPath[100];
GtkWidget *playButton, *pScaleStart, *displayLabel;

/* Main window icon */
GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}


/* Sets the icon of 'playButton' as 'play' or 'pause' */
void setPlayButtonIcon (){
	gchar *filename;
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-play.png", NULL);
	GtkWidget *playImage = gtk_image_new_from_file(filename);
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-pause.png", NULL);
	GtkWidget *pauseImage = gtk_image_new_from_file(filename);
	//GtkWidget *playImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_SMALL_TOOLBAR);
	//GtkWidget *pauseImage = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_SMALL_TOOLBAR);
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
void getTextDisplayLabel(GtkWidget *widget, Spectrum3dGui *spectrum3dGui){
	char textDisplayLabel[80];
	int startDisplay = (int)gtk_range_get_value(GTK_RANGE(pScaleStart)) * hzStep;
	int displayRange = (int)gtk_range_get_value(GTK_RANGE(spectrum3dGui->scaleBands)) * hzStep * zoomFactor;
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

void create_external_window_drawing_area(Spectrum3dGui *spectrum3dGui){
	GtkWidget *window;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	gtk_window_set_default_size (GTK_WINDOW(window), (gint)spectrum3d.width, (gint)spectrum3d.height);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (quit_spectrum3d), NULL);
	gtk_widget_set_events (window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
	gtk_widget_realize(window);
	spectrum3dGui->drawing_area = gtk_drawing_area_new ();
	 
	gtk_widget_realize(spectrum3dGui->drawing_area);
	//gtk_box_pack_start (GTK_BOX (vBox), spectrum3dGui->drawing_area, TRUE, TRUE, 0);

#if defined (GTKGLEXT3) || defined (GTKGLEXT1)
	GdkGLConfig *glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
					GDK_GL_MODE_DEPTH  |
					GDK_GL_MODE_DOUBLE);
	  if (glconfig == NULL)
	    {
	      g_print ("\n*** Cannot find the double-buffered visual.\n");
	      g_print ("\n*** Trying single-buffered visual.\n");

	      /* Try single-buffered visual */
	      glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
						    GDK_GL_MODE_DEPTH);
	      if (glconfig == NULL)
		{
		  g_print ("*** No appropriate OpenGL-capable visual found.\n");
		  exit (1);
		}
	    }
	/* Set OpenGL-capability to the widget */
  	gtk_widget_set_gl_capability (spectrum3dGui->drawing_area,
				glconfig,
				NULL,
				TRUE,
				GDK_GL_RGBA_TYPE);
#endif
	
gtk_container_add(GTK_CONTAINER(window), spectrum3dGui->drawing_area);

#ifdef HAVE_LIBSDL 
	/* Hack to get SDL to use GTK window */
	/*{ char SDL_windowhack[32];
		sprintf(SDL_windowhack,"SDL_WINDOWID=%ld",
			GDK_WINDOW_XID(gtk_widget_get_window(spectrum3dGui->drawing_area)));
			// GDK_WINDOW_XID( spectrum3dGui.drawing_area->window))); pour GTK2??
		putenv(SDL_windowhack);
	printf("%s\n", SDL_windowhack);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
		}*/
#endif
	
	//g_signal_connect (window, "key-press-event", G_CALLBACK (on_key_press), spectrum3dGui);
	//g_signal_connect (window, "key-release-event", G_CALLBACK (on_key_release), spectrum3dGui);
	//g_signal_connect (window, "motion-notify-event", G_CALLBACK (on_mouse_motion), NULL);
	//g_signal_connect (window, "scroll-event", G_CALLBACK (on_mouse_scroll), NULL);
	//g_signal_connect (G_OBJECT (spectrum3dGui->drawing_area), "configure_event", G_CALLBACK (configure_event), NULL);

	gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
	printf("%s \nPlease report any bug to %s\n", PACKAGE_STRING, PACKAGE_BUGREPORT);	
	//printf("number of arg = %d, argv = %s\n", argc, argv[1]);
	printf("argc = %d\n", argc);
	if (argv[1] != NULL) {
		if (strstr (argv[1],"debug") != NULL) {
		debug = TRUE;
		printf("debug = TRUE\n");
			}  
		}
	//DEBUG("debug is true\n");
	gchar *filename;
	int i = 0;
	gint initialWindowHeight = 170;
	guint timeoutEvent, intervalDisplaySpectro;
	GtkWidget *pVBox[4], *pHBox[13], *menuBar, *menu, *submenu, *menuItem, *submenuItem, *frame, *image, *label, *widget, *sourceRadioButton, *sourceMic, *sourceFile;
	GdkColor color;
	Spectrum3dGui spectrum3dGui;
	GSList *radio_menu_group;
	GError **error;

#if defined (GTKGLEXT3) || defined (GTKGLEXT1)
	GdkGLConfig *glconfig;
#endif 
	init_SDL(); 
	// It seems that SDL_Init must be called before gtk_init in sdl2, otherwise there will be a segfault
	gst_init (NULL, NULL);
	gtk_init (&argc, &argv);
		
	get_saved_values();
	intervalDisplaySpectro = (guint)spectrum3d.interval_display;

#if defined (GTKGLEXT3) || defined (GTKGLEXT1)
	gtk_gl_init(NULL, NULL);
	glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
					GDK_GL_MODE_DEPTH  |
					GDK_GL_MODE_DOUBLE);
	  if (glconfig == NULL)
	    {
	      g_print ("\n*** Cannot find the double-buffered visual.\n");
	      g_print ("\n*** Trying single-buffered visual.\n");

	      /* Try single-buffered visual */
	      glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
						    GDK_GL_MODE_DEPTH);
	      if (glconfig == NULL)
		{
		  g_print ("*** No appropriate OpenGL-capable visual found.\n");
		  exit (1);
		}
	    }
#endif

	initGstreamer();
	init_audio_values();
	init_display_values(&spectrum3dGui);
	configure_SDL_gl_window (spectrum3d.width, spectrum3d.height);
	
	spectrum3dGui.mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	//gtk_widget_set_size_request (spectrum3dGui.mainWindow, 700, initialWindowHeight);
	gtk_widget_set_size_request (spectrum3dGui.mainWindow, 700, 160);
	gtk_widget_realize(spectrum3dGui.mainWindow);
	gtk_window_set_title(GTK_WINDOW(spectrum3dGui.mainWindow), PACKAGE_NAME);
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d.png", NULL);
	gtk_window_set_icon(GTK_WINDOW(spectrum3dGui.mainWindow), gdk_pixbuf_new_from_file (filename, error));
	g_signal_connect (G_OBJECT (spectrum3dGui.mainWindow), "destroy", G_CALLBACK (quit_spectrum3d), NULL);

#ifdef GTK3
	gtk_widget_queue_draw (spectrum3dGui.mainWindow);

	//gtk_container_set_reallocate_redraws (GTK_CONTAINER (spectrum3dGui.mainWindow), TRUE);
#endif

#ifdef GTK3
	for (i = 0; i < 4; i++) {
		pVBox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		}
	pHBox[0] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	for (i = 1; i < 12; i++) {
		pHBox[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		}
	pHBox[12] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#elif defined GTK2
	for (i = 0; i < 4; i++) {
		pVBox[i] = gtk_vbox_new(FALSE, 0);
		}
	pHBox[0] = gtk_hbox_new(TRUE, 0);
	for (i = 1; i < 12; i++) {
		pHBox[i] = gtk_hbox_new(FALSE, 0);
		}
	pHBox[12] = gtk_hbox_new(TRUE, 0);
#endif
	
	gtk_container_add(GTK_CONTAINER(spectrum3dGui.mainWindow), pVBox[1]); 
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
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onPreferences), &spectrum3dGui);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

	menu = gtk_menu_new(); // 'Sound' submenu
	menuItem = gtk_menu_item_new_with_label("Sound");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

		submenu = gtk_menu_new();// 'Play test sound' sub-submenu
		spectrum3dGui.checkMenuTestSound = gtk_check_menu_item_new_with_label("Test Sound");
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), spectrum3dGui.checkMenuTestSound);
		g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuTestSound), "activate", G_CALLBACK(menu_check_test_sound), &spectrum3dGui);

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
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuText), "activate", G_CALLBACK(check_menu_text), &spectrum3dGui);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuText);
	
			spectrum3dGui.checkMenuLines = gtk_check_menu_item_new_with_label("Lines (L)");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spectrum3dGui.checkMenuLines), TRUE);
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuLines), "activate", G_CALLBACK(check_menu_lines), &spectrum3dGui);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuLines);

			spectrum3dGui.checkMenuPointer = gtk_check_menu_item_new_with_label("Pointer (P)");
			g_signal_connect(G_OBJECT(spectrum3dGui.checkMenuPointer), "activate", G_CALLBACK(check_menu_pointer), &spectrum3dGui);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), spectrum3dGui.checkMenuPointer);

		submenu = gtk_menu_new();// 'Change/reset view' sub-submenu
		menuItem = gtk_menu_item_new_with_label("Change/reset view");
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuItem), submenu);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);	
			
			widget = gtk_menu_item_new_with_label("Reset view (R)"); // Reset to initial view
			gtk_widget_set_tooltip_text (widget, "Reset the view as if Spectrum3d was just started");
			g_signal_connect(G_OBJECT(widget), "activate", G_CALLBACK(reset_view), NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), widget);

			widget = gtk_menu_item_new_with_label("Front view (O)"); // Set to front view
			gtk_widget_set_tooltip_text (widget, "2D view showing frequency versus intensity of the sound; shows a snapshot of the harmonics at a given time");
			g_signal_connect(G_OBJECT(widget), "activate", G_CALLBACK(front_view), NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), widget);

			submenuItem = gtk_menu_item_new_with_label("Preset view");
			gtk_widget_set_tooltip_text (submenuItem, "Set the view with the chosen preset values");
			g_signal_connect(G_OBJECT(submenuItem), "activate", G_CALLBACK(set_view_from_preset), submenuItem);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenuItem);

	menu = gtk_menu_new(); // 'Help...' submenu
	menuItem = gtk_menu_item_new_with_label("Shortcuts"); 
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onShortcuts), (GtkWidget*) spectrum3dGui.mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#ifdef HAVE_LIBGEIS
	menuItem = gtk_menu_item_new_with_label("Gestures Shortcuts");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onGesturesShortcuts), (GtkWidget*) spectrum3dGui.mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
#endif 
	menuItem = gtk_menu_item_new_with_label("About...");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onAbout), (GtkWidget*) spectrum3dGui.mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Quick start");
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(onQuickStart), (GtkWidget*) spectrum3dGui.mainWindow);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);
	gtk_box_pack_start(GTK_BOX(pHBox[0]), menuBar, FALSE, TRUE, 0);

/* SourceButtons to set type of source (microphone or audio file) : NEW : no STOP button anymore */

	sourceMic = gtk_button_new(); // Microphone button
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-microphone.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image (GTK_BUTTON(sourceMic), image);
	gtk_widget_set_name(sourceMic, "mic");
	gtk_widget_set_tooltip_text (sourceMic, "Source is microphone; select this to record something and then load it");
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[1], FALSE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), sourceMic, FALSE, TRUE, 2);
	
	sourceFile = gtk_button_new(); // File button
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-file_grey.png", NULL);
	image = gtk_image_new_from_file(filename);
	gtk_button_set_image(GTK_BUTTON(sourceFile),image);
	gtk_widget_set_name(sourceFile, "file");
	gtk_widget_set_tooltip_text (sourceFile, "Source is an audio file");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), sourceFile, FALSE, FALSE, 2);

	g_signal_connect(G_OBJECT(sourceMic), "clicked", G_CALLBACK(change_source_button), sourceFile);
	g_signal_connect(G_OBJECT(sourceFile), "clicked", G_CALLBACK(change_source_button), sourceMic);
	
/* "Reload" button */
	spectrum3dGui.reload = gtk_button_new();
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-refresh.png", NULL);
	image = gtk_image_new_from_file(filename);
	//image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.reload),image);
	gtk_widget_set_sensitive (spectrum3dGui.reload, FALSE);
	gtk_widget_set_tooltip_text (spectrum3dGui.reload, "Reload audio file (usefull if some audio parameters like equalizer or filters have been changed)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.reload, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.reload), "clicked", G_CALLBACK(load_audio_file), NULL);

/* "Analyse in real-time" button */
	widget = gtk_check_button_new_with_label("Analyse in\nrealtime");
	gtk_widget_set_tooltip_text (GTK_WIDGET(widget), "If checked, harmonics will be analysed and displayed at the same time; if unchecked, harmonics will be analysed first, then displayed for the whole audio file, then it can be played afterwards");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), TRUE);
	g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(set_analyse_in_rt), &spectrum3dGui);

/* separator */
#ifdef GTK3
	widget = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#elif defined GTK2
	widget = gtk_vseparator_new();
#endif
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);
	
/* "Play/Pause" button */
	playButton = gtk_button_new();
	gtk_widget_set_tooltip_text (playButton, "Play/Pause the audio stream");
	setPlayButtonIcon();
	gtk_widget_set_size_request (playButton, 80, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), playButton, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(playButton), "clicked", G_CALLBACK(playFromSource), "NO_MESSAGE");
	
/* "Stop" button */
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-stop.png", NULL);
	image = gtk_image_new_from_file(filename);
	widget = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(widget),image);
	gtk_widget_set_tooltip_text (widget, "Stop playing audio stream");
	gtk_widget_set_size_request (widget, 50, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_stop), NULL);

/* "Record" button */
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-record.png", NULL);
	image = gtk_image_new_from_file(filename);
	spectrum3dGui.record = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(spectrum3dGui.record),image);
	gtk_widget_set_tooltip_text (spectrum3dGui.record, "Record from microphone\n'Analyse in realtime' must be uncheck to use this");
	//image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_RECORD, GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_set_sensitive (spectrum3dGui.record, FALSE);
	gtk_box_pack_start(GTK_BOX(pHBox[1]), spectrum3dGui.record, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(spectrum3dGui.record), "clicked", G_CALLBACK(record_window), NULL);

/* separator */
#ifdef GTK3
	widget = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#elif defined GTK2
	widget = gtk_vseparator_new();
#endif	
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);

/* JACK check button */
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-qjackctl.png", NULL);
	image = gtk_image_new_from_file(filename);
	widget = gtk_check_button_new ();
	gtk_button_set_image (GTK_BUTTON(widget), image);
	gtk_widget_set_tooltip_text (widget, "Use Jack-Audio-Connection-Kit (JACK)");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 2);
	g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(use_jack), &spectrum3dGui);

/* separator */
#ifdef GTK3
	widget = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#elif defined GTK2
	widget = gtk_vseparator_new();
#endif	
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 5);

/* Button to open the Filter and Equalizer window */
	// create effectsWindow first without showing it
	effects_window(&spectrum3dGui);
	// then create a button that will call its display when clicked
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-equalizer.png", NULL);
	image = gtk_image_new_from_file(filename);
	widget = gtk_button_new();  
	gtk_button_set_image (GTK_BUTTON(widget), image);
	gtk_widget_set_tooltip_text (widget, "Show/Hide the filter and equalizer window");
	gtk_box_pack_start(GTK_BOX(pHBox[1]), widget, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(show_effects_window), &spectrum3dGui);

/* Time label */
	label=gtk_label_new("Time : ");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[5], FALSE, FALSE, 2);

/* Progress & seek scale */
#ifdef GTK3
	scaleSeek = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1);
#elif defined GTK2
	scaleSeek = gtk_hscale_new_with_range (0.0, 100.0, 1);
#endif
	gtk_scale_set_draw_value (GTK_SCALE (scaleSeek), FALSE);
	//gtk_scale_set_value_pos (GTK_SCALE (pScaleSeek), GTK_POS_TOP);
	gtk_range_set_value (GTK_RANGE (scaleSeek), 0);
	gtk_widget_set_size_request (scaleSeek, 500, 20);
	gtk_box_pack_start(GTK_BOX(pHBox[5]), scaleSeek, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(scaleSeek), "value-changed", G_CALLBACK(on_seek), NULL);
	
	timeLabel=gtk_label_new("           0:00 / 0:00           ");
	gtk_box_pack_start(GTK_BOX(pHBox[5]), timeLabel, FALSE, FALSE, 0);

/* Create drawing area */
	if (externalWindow == FALSE){
		/* Resize spectrum3dGui.mainWindow to contain drawing_area; using gtk_window_set_defaut() allows to shrink the window (gtk_widget_set_size_request() does not allow to shrink the window below the requested size); */
		//gtk_window_set_default_size (GTK_WINDOW(spectrum3dGui.mainWindow), (gint)spectrum3d.width, initialWindowHeight + (gint)spectrum3d.height);
		
		//gtk_widget_realize(spectrum3dGui.mainWindow);

		spectrum3dGui.drawing_area = gtk_drawing_area_new ();
		
#if defined (GTKGLEXT3) || defined (GTKGLEXT1)
		/* Set OpenGL-capability to the widget */
	  	gtk_widget_set_gl_capability (spectrum3dGui.drawing_area,
					glconfig,
					NULL,
					TRUE,
					GDK_GL_RGBA_TYPE);		
#endif	
		
		/* drawing_area has to be put in vBox AFTER the call to gtk_widget_set_gl_capability() (if GtkGlExt is used) and BEFORE the call to the sdl-gtk hack (if sdl is used)*/ 
		//gtk_box_pack_start (GTK_BOX (pVBox[1]), spectrum3dGui.drawing_area, TRUE, TRUE, 0);	
		
#ifdef HAVE_LIBSDL 
		/* Hack to get SDL to use GTK window */
		/*{ char SDL_windowhack[32];
			sprintf(SDL_windowhack,"SDL_WINDOWID=%ld",
				GDK_WINDOW_XID(gtk_widget_get_window(spectrum3dGui.drawing_area)));
				// GDK_WINDOW_XID( spectrum3dGui.drawing_area->window))); pour GTK2??
			putenv(SDL_windowhack);
		printf("%s\n", SDL_windowhack);
		}*/

		
		//SDL_EnableKeyRepeat(10, 10);
#endif

		//g_signal_connect (spectrum3dGui.mainWindow, "key-press-event", G_CALLBACK (on_key_press), &spectrum3dGui);
		//g_signal_connect (spectrum3dGui.mainWindow, "key-release-event", G_CALLBACK (on_key_release), &spectrum3dGui);
		//g_signal_connect (spectrum3dGui.mainWindow, "motion-notify-event", G_CALLBACK (on_mouse_motion), NULL);
		//g_signal_connect (spectrum3dGui.mainWindow, "scroll-event", G_CALLBACK (on_mouse_scroll), NULL);
		//g_signal_connect (G_OBJECT (spectrum3dGui.drawing_area), "configure_event", G_CALLBACK (configure_event), NULL);
		//gtk_widget_add_events (spectrum3dGui.mainWindow, gtk_widget_get_events (spectrum3dGui.mainWindow) | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);
	}
	else {
		create_external_window_drawing_area(&spectrum3dGui);
		}	

/* Starting value of the display */
	frame = gtk_frame_new("Start value of display (in Hz)");
	gtk_widget_set_tooltip_text (frame, "The lower displayed frequency (in herz)");
	spectrum3dGui.adjustStart = gtk_adjustment_new(0, 0, 9000, ((gdouble)hzStep * (gdouble)zoomFactor), (((gdouble)hzStep * (gdouble)zoomFactor) * 10), 0);
#ifdef GTK3
	pScaleStart = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT(spectrum3dGui.adjustStart));
#elif defined GTK2
	pScaleStart = gtk_hscale_new(GTK_ADJUSTMENT(spectrum3dGui.adjustStart));
#endif
	gtk_scale_set_digits (GTK_SCALE(pScaleStart), 0);
	gtk_scale_set_value_pos(GTK_SCALE(pScaleStart), GTK_POS_RIGHT);
	gtk_box_pack_start(GTK_BOX(pVBox[1]), pHBox[11], FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), pScaleStart);
	g_signal_connect(G_OBJECT(pScaleStart), "value-changed", G_CALLBACK(change_start), &spectrum3dGui);
	g_signal_connect(G_OBJECT(pScaleStart), "format-value", G_CALLBACK(format_value_start), NULL);
	g_signal_connect(G_OBJECT(pScaleStart), "value-changed", G_CALLBACK(getTextDisplayLabel), &spectrum3dGui);

/* Range of display */
	frame = gtk_frame_new("Range of display (in Hz)");
	gtk_widget_set_tooltip_text (frame, "The range of the displayed frequency (in herz)");
	spectrum3dGui.adjustBands = gtk_adjustment_new(1000, 20, 1000, 10, 50, 0);
#ifdef GTK3
	spectrum3dGui.scaleBands = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT(spectrum3dGui.adjustBands));
#elif defined GTK2
	spectrum3dGui.scaleBands = gtk_hscale_new(GTK_ADJUSTMENT(spectrum3dGui.adjustBands));
#endif
	gtk_scale_set_digits (GTK_SCALE(spectrum3dGui.scaleBands), 0);
	gtk_scale_set_value_pos(GTK_SCALE(spectrum3dGui.scaleBands), GTK_POS_RIGHT);
	gtk_container_add(GTK_CONTAINER(frame), spectrum3dGui.scaleBands);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(spectrum3dGui.scaleBands), "value-changed", G_CALLBACK(change_bands), &spectrum3dGui);
	g_signal_connect(G_OBJECT(spectrum3dGui.scaleBands), "format-value", G_CALLBACK(format_value_bands), NULL);
	g_signal_connect(G_OBJECT(spectrum3dGui.scaleBands), "value-changed", G_CALLBACK(getTextDisplayLabel), &spectrum3dGui);
	
/* "x" label */
	label=gtk_label_new("x");
	gtk_container_add(GTK_CONTAINER(pHBox[11]), label);

/* Factor that multiplies the range of display */
	frame = gtk_frame_new("");
	gtk_widget_set_tooltip_text (frame, "Factor that multiplies the frequency range, to make it larger");
	spectrum3dGui.cbRange = gtk_combo_box_text_new();
	gtk_container_add(GTK_CONTAINER(frame), spectrum3dGui.cbRange);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);
	for (i = 1; i <= 20; i++){
		gchar text[4];
		sprintf(text, "%d", i);
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(spectrum3dGui.cbRange), text);
		}
	gtk_combo_box_set_active(GTK_COMBO_BOX(spectrum3dGui.cbRange), 0);
	g_signal_connect( G_OBJECT(spectrum3dGui.cbRange), "changed", G_CALLBACK( cb_range_changed ), &spectrum3dGui );
	g_signal_connect( G_OBJECT(spectrum3dGui.cbRange), "changed", G_CALLBACK(getTextDisplayLabel), &spectrum3dGui );
	
/* Label that shows starting value, ending value and range of display */
	frame = gtk_frame_new("Values displayed");
	gtk_widget_set_tooltip_text (frame, "The lower and the highest displayed value (in herz), and their range");
	displayLabel=gtk_label_new(""); 
	gtk_container_add(GTK_CONTAINER(frame), displayLabel);
	getTextDisplayLabel(NULL, &spectrum3dGui);
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 0);

/* 'Gain' Gtk Scale */
	frame = gtk_frame_new("Display Gain");
	gtk_box_pack_start(GTK_BOX(pHBox[11]), frame, FALSE, FALSE, 10);
	filename = g_build_filename (G_DIR_SEPARATOR_S, DATADIR, "icons", "spectrum3d-gain.png", NULL);
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
	// FIXME error message here with the previous line : gtk_image_set_from_stock: assertion `GTK_IS_IMAGE (image)' failed; it could be a bug in Gtk
	//g_object_set (pScaleGain, "update-policy", GTK_UPDATE_DISCONTINUOUS, NULL);
	g_signal_connect(G_OBJECT(spectrum3dGui.scaleGain), "value-changed", G_CALLBACK(change_gain), NULL);

#ifdef HAVE_LIBGEIS
	setupGeis();
#endif

	gtk_widget_show_all (spectrum3dGui.mainWindow);

	timeoutEvent = g_timeout_add (100, (GSourceFunc)sdl_event, &spectrum3dGui);
	spectrum3d.timeoutExpose = g_timeout_add (intervalDisplaySpectro, (GSourceFunc)display_spectro, &spectrum3dGui);

	printf("Showing Gtk GUI\n");
	gtk_main ();
	

/* Quit everything */

#ifdef HAVE_LIBGEIS
	geisQuit();
#endif
	on_stop();
	g_source_remove(spectrum3d.timeoutExpose);
	g_source_remove(timeoutEvent);

	//SDL_Quit();

	print_rc_file();

	printf("Quit everything\nGood Bye!\n");
	
	return 0;
}



