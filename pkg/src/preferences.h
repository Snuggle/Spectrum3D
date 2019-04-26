
#ifndef DEFINE_PREFERENCES
#define DEFINE_PREFERENCES

gchar policyName[20];
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
	gboolean min;		// minimum value; always 0 or FALSE
	gboolean max;		// maximun value; always 1 or TRUE
	char *name;		// name of the variable, in a string
}; 

void error_message_window(gchar *message);

#endif



