
#ifndef DEFINE_TYPESOURCE
#define DEFINE_TYPESOURCE

gboolean analyse_rt, loading, newEvent;
gchar *selected_file;
int spect_bands, frame_number_counter, playing, numberOfFrames;
gint64 len, pos;
GstElement *pipeline;
GLfloat spec_data[405][11030];

typedef enum TypeSource TypeSource;
enum TypeSource
{
	NONE,
	MIC, 			// source is the microphone
	AUDIO_FILE, 		// source is an audio file 
};
TypeSource typeSource;

void playFromSource(GtkWidget *widget, gpointer *data);
void on_stop();
void load_audio_file();
void error_message_window(gchar *message);

#endif




