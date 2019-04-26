
#ifndef DEFINE_SPECTRUM3D
#define DEFINE_SPECTRUM3D

/* Spectrum3dGui structure declares elements that are part the the Gtk GUI of Spectrum3D and have to be passed to another function */    
typedef struct 
{
	GtkWidget *mainWindow;
	
	gboolean textScale;
	gboolean lineScale;
	gboolean pointer;
	
	GtkWidget *checkMenuTestSound;
	GtkWidget *dialogTestSound;
	GtkWidget *radio2D;
	GtkWidget *radio3D;
	GtkWidget *radio3Dflat;
	GtkWidget *checkMenuLines;
	GtkWidget *checkMenuText;
	GtkWidget *checkMenuPointer;
	GtkWidget *reset;
	GtkWidget *front;

	GtkWidget *stop;
	GtkWidget *file;
	GtkWidget *mic;
	GtkWidget *reload;
	GtkWidget *record;

	GtkWidget *drawing_area;

	GtkWidget *cbRange;
	GtkWidget *scaleGain;
	GtkWidget *scaleBands;

	GtkWidget *effectsWindow;
#ifdef GTK3
	GtkAdjustment *adjustBands, *adjustStart;
#elif GTK2
	GtkObject *adjustBands, *adjustStart;
#endif
} Spectrum3dGui; 

/* Spectrum3d structure declares variables (display or audio variables) that have to be passed to several function all through the program */ 
typedef struct 
{
	gboolean realtime;	// enable realtime mode when Jack is not used
	gboolean enableTouch;	// enable controlling by touch

	int width;      	// width of the sdl window or drawing area
	int height;		// height of the sdl window or drawing area
	int interval_display;	// interval between 2 displays; used as time interval for the callback that calls display_spectro()
	int interval_rt;	// interval between 2 spectrum alalysis by spectrum element of Gstreamer when analyse and display are done while playing; 
	int interval_loaded;	// interval between 2 spectrum alalysis by spectrum element of Gstreamer when analyse and display are done before playing; 
	int frames;		// number of 'frames' (time dimension, Z axis for 3D view, X axis for 2D view)
	int previousFrames;	// current number of frames that is stored when 'Front view' is used, so that this number of frames can be restored when 'reset' is done
	int priority;		// priority when realtime is used without Jack

	guint timeoutExpose;	// timeout returned by the callback calling display_spectro() function 

	GLfloat presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ;
				// preset values that are stored in and retrieved from the rc file
	GLfloat zStep;		// distance interval between 2 frames (Z axis) in the 3D view
} Spectrum3d;
Spectrum3d spectrum3d;  

#endif



