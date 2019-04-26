
#ifndef DEFINE_PREFERENCES
#define DEFINE_PREFERENCES

gboolean realtime, enableTouch, pointer;
gchar policyName[20];
int zoom, zoomFactor, textScale, lineScale, priority;
guint spect_bands;
float showGain; 

FILE *rcFile;

typedef enum ColorType ColorType;
enum ColorType
{
	PURPLE, RAINBOW, RED
};
ColorType colorType;

// Structure containing everything regarding the default values that are an Integer
typedef struct PreferenceInt PreferenceInt;
struct PreferenceInt
{
	int* var;		// variable name, stored as a pointer
	int def;		// default value
	int min;		// minimum value
	int max;		// maximun value
	char *name;		// name of the variable, in a string
	//char* info;		// any information that could be uselfull
}; 

// Structure containing everything regarding the default values that are a GLFloat
typedef struct PreferenceGLFloat PreferenceGLFloat;
struct PreferenceGLFloat
{
	GLfloat* var;		// variable name, stored as a pointer
	GLfloat def;		// default value
	GLfloat min;		// minimum value
	GLfloat max;		// maximun value
	char *name;		// name of the variable, in a string
}; 

// Structure containing everything regarding the default values that are a gboolean
typedef struct PreferenceGbool PreferenceGbool;
struct PreferenceGbool
{
	gboolean* var;		// variable name, stored as a pointer
	gboolean def;		// default value
	char *name;		// name of the variable, in a string
}; 

typedef struct PreferenceString PreferenceString;
struct PreferenceString
{
	char* var[20];		// variable name, stored as a pointer
	char *name;		// name of the variable, in a string
}; 

void getFileName();
void defaultValues();
void makeDefaultPreferencesFile();
void onPreferences(GtkWidget* widget, gpointer data);
void changeWidth(GtkSpinButton *spinButton, gpointer user_data);
void saveChange(GtkWidget* widget, gpointer data);
void onQuickStart(GtkWidget* widget, gpointer data);
void onAbout(GtkWidget* widget, gpointer data);
void onDisplay(GtkWidget *pBtn, gpointer data);
void onShortcuts (GtkWidget* widget, gpointer data);
void onGesturesShortcuts (GtkWidget* widget, gpointer data);
//void errorMessageWindow(char *message);

#endif



