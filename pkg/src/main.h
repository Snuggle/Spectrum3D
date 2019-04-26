
#ifndef DEFINE_MAIN
#define DEFINE_MAIN

gboolean analyse_rt, jack, newEvent, externalWindow;
int playing, pose, zoomFactor, hzStep;
//static GtkWidget *mainWindow;
GtkWidget *timeLabel, *scaleSeek;

typedef enum TypeSource TypeSource;
enum TypeSource
{
	NONE,
	MIC, 			// source is the microphone
	AUDIO_FILE, 		// source is an audio file 
};
TypeSource typeSource;

typedef enum ColorType ColorType;
enum ColorType
{
	PURPLE, RAINBOW, RED
};
ColorType colorType;

void onPreferences(GtkWidget* widget, gpointer data);
void menu_check_test_sound(GtkWidget *widget, Spectrum3dGui *spectrum3dGui);
void change_perspective(GtkWidget *widget, gpointer data);
void check_menu_text(GtkWidget *widget, gpointer data);
void check_menu_lines(GtkWidget *widget, gpointer data);
void check_menu_pointer(GtkWidget *widget, gpointer data);
void reset_view();
void front_view();
void set_view_from_preset();
void onQuickStart(GtkWidget* widget, gpointer data);
void onAbout(GtkWidget* widget, gpointer data);
void onShortcuts (GtkWidget* widget, gpointer data);
void onGesturesShortcuts (GtkWidget* widget, gpointer data);

void get_saved_values();		
void initGstreamer();
void init_audio_values();
void init_display_values();
void setupGeis();
void geisQuit();
void print_rc_file();

void change_source_button (GtkWidget *widget, Spectrum3dGui *spectrum3dGui);
void load_audio_file();
void set_analyse_in_rt(GtkWidget *check, gpointer data);
void playFromSource(gchar *message);
void on_stop();
void record_window();
void use_jack(GtkWidget *check, gpointer data);
void effects_window(Spectrum3dGui *spectrum3dGui);
void show_effects_window(GtkWidget *widget, Spectrum3dGui *spectrum3dGui);
void on_seek (GtkRange *range, gchar *data);
void cb_range_changed(GtkWidget *combo, gpointer data);
void change_adjust(GtkWidget *pWidget, gpointer data);
void change_bands(GtkWidget *pWidget, gpointer data);
void change_start(GtkWidget *pWidget, gpointer data);
gchar* format_value_start (GtkScale *scale, gdouble value);
gchar* format_value_bands (GtkScale *scale, gdouble value);
gboolean change_gain(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data);
gboolean display_spectro(GtkWidget *drawing_area);

gboolean configure_SDL_gl_window (int width, int height);
void sdl_event();

gboolean configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean on_key_press (GtkWidget * window, GdkEventKey *event, gpointer userdata);
gboolean on_key_release (GtkWidget * window, GdkEventKey *event, gpointer userdata);
gboolean on_mouse_motion (GtkWidget * window, GdkEventMotion *event, gpointer userdata);
gboolean on_mouse_scroll (GtkWidget * window, GdkEventScroll *event, gpointer userdata);

#endif



