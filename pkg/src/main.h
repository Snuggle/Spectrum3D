
#ifndef DEFINE_MAIN
#define DEFINE_MAIN

gboolean analyse_rt, jack, newEvent;
int playing, pose, zoomFactor, hzStep;
static GtkWidget *mainWindow;
GtkWidget *pScaleBands, *pComboRange, *timeLabel, *scaleSeek;
GtkObject *adjust_bands, *adjust_start;

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

void test_sound_window(GtkWidget *widget, gpointer *data);
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
void init_display_values();
void sdl_event();
void getFileName();
void load_audio_file();
void on_stop();
void record_window();
void set_source_to_none();
void change_source_button (GtkWidget *widget, Spectrum3dGui *spectrum3dGui);
void set_analyse_in_rt(GtkWidget *check, gpointer data);
void use_jack(GtkWidget *check, gpointer data);
void playFromSource(gchar *message);
void playTestSound(GtkWidget *pWidget, gpointer data);
void change_freq_test_sound(GtkWidget *widget, gpointer data);
void change_volume_test_sound(GtkWidget *pWidget, gpointer data);
void cb_zoom_changed(GtkComboBox *combo, gpointer data);
void cb_speed_changed(GtkComboBox *combo, gpointer data);
void cb_range_changed(GtkWidget *combo, gpointer data);
void cb_scale_changed(GtkComboBox *combo, gpointer data);
void change_adjust(GtkWidget *pWidget, gpointer data);
void change_bands(GtkWidget *pWidget, gpointer data);
void change_start(GtkWidget *pWidget, gpointer data);
gchar* format_value_start (GtkScale *scale, gdouble value);
gchar* format_value_bands (GtkScale *scale, gdouble value);
gboolean change_gain(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data);
void effects_window();
void onPreferences(GtkWidget* widget, gpointer data);
void saveChange(GtkWidget* widget, gpointer data);
void errorMessageWindow(char *message);

gboolean configure (GtkWidget *da, GdkEventConfigure *event, gpointer user_data);
gboolean display_spectro(GtkWidget *da);
gboolean on_key_press (GtkWidget * window, GdkEventKey*	pKey, gpointer userdata);
gboolean on_mouse_motion (GtkWidget * window, GdkEventMotion *event, gpointer userdata);
gboolean on_mouse_scroll (GtkWidget * window, GdkEventScroll *event, gpointer userdata);

void on_seek (GtkRange *range, gchar *data);

#endif



