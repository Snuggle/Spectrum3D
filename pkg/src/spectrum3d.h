
#ifndef DEFINE_SPECTRUM3D
#define DEFINE_SPECTRUM3D

typedef struct 
{
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

	GtkWidget *scaleGain;
} Spectrum3dGui; 

typedef struct 
{
	int width;
	int height;
	int interval_display;
	int interval_rt;
	int interval_loaded;
	int frames;
	int previousFrames;

	guint timeoutExpose;

	GLfloat presetX, presetY, presetZ, presetAngleH, presetAngleV, presetAngleZ;
	GLfloat zStep;
} Spectrum3d;
Spectrum3d spectrum3d;  

#endif



