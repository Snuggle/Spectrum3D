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
#include <errno.h>
#include <gtk/gtk.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "config.h"
#include "preferences.h"
#include "spectrum3d.h"

char prefPath[100], rcPath[100];

/* Preferences structures are declared here */
int sizeofPrefInt = 5;
PreferenceInt preferenceInt[5] = {
	{&spectrum3d.width, 600, 50, 2500, "spectrum3d.width"},
	{&spectrum3d.height, 300, 50, 1500, "spectrum3d.height"},
	{&spectrum3d.interval_display, 100, 100, 500, "spectrum3d.interval_display"},
	{&spectrum3d.interval_rt, 150, 100, 500, "spectrum3d.interval_rt"},
	{&spectrum3d.frames, 100, 50, 400, "spectrum3d.frames"}
	}; 

int sizeofPrefGLFloat = 7;
PreferenceGLFloat preferenceGLFloat[7] = {
	{&spectrum3d.presetX, -0.7, -100, 100, "spectrum3d.presetX"}, 
	{&spectrum3d.presetY, -0.1, -100, 100, "spectrum3d.presetY"},
	{&spectrum3d.presetZ, -1.05, -100, 100, "spectrum3d.presetZ"},
	{&spectrum3d.presetAngleH, -16, -360, 360, "spectrum3d.presetAngleH"},
	{&spectrum3d.presetAngleV, 10, -360, 360, "spectrum3d.presetAngleV"},
	{&spectrum3d.presetAngleZ, 0, -360, 360, "spectrum3d.presetAngleZ"},
	{&spectrum3d.zStep, 0.01, 0.01, 0.03, "spectrum3d.zStep"}
	};

int sizeofPrefGbool = 2;
PreferenceGbool preferenceGbool[2] = {
	{&spectrum3d.enableTouch, TRUE, 0, 1, "spectrum3d.enableTouch"},
	{&externalWindow, FALSE, 0, 1, "spectrum3d.externalWindow"}
	};

/*int sizeofPrefString = 1;
char *preferenceString[1] = {policyName};
char prefStringPossibleValue[1][4][40] = {
	{"policyName", "SCHED_RR", "SCHED_RR", "SCHED_FIFO"}
	};*/

#define ERROR_MESSAGE_PRINT_IN_RCFILE()  \
	if (result < 0){  \
		gchar message[200]; \
		sprintf(message, "***** ERROR writing in rc file: %s\nClosing preferences file", strerror( errno ));  \
		printf ("\n%s\n", message); \
		error_message_window(message);  \
		fclose(rcFile);  \
		return; \
		}

#define ERROR_MESSAGE_GET_PREFERENCES()   \
	if (result == NULL){  \
		printf("Error getting file content: %s\n", strerror( errno ));	  \
		}

#define ERROR_MESSAGE_COMPARE_VALUES(VARIABLE)	\
	gchar message[200]; \
	sprintf(message, "'%s' seems out of range -> using default values\n", VARIABLE); \
	printf ("\n%s\n", message); \
	error_message_window(message); \
	set_default_values(); \
	error = TRUE; \
	return; 


/* Set all values to default; this happens if Spectrum3d cannot open the preferences file, typically when it is run for the first time; this happen also if one on the values appears to be out of range */
void set_default_values(){
	int i = 0;
	for (i = 0; i < sizeofPrefInt; i++){
		*preferenceInt[i].var = preferenceInt[i].def;
		}
	for (i = 0; i < sizeofPrefGLFloat; i++){
		*preferenceGLFloat[i].var = preferenceGLFloat[i].def;
		}
	for (i = 0; i < sizeofPrefGbool; i++){
		*preferenceGbool[i].var = preferenceGbool[i].def;
		}
	/*for (i = 0; i < sizeofPrefString; i++){
		if (strcmp(prefStringPossibleValue[i][1], "null") != 0){
			sprintf(preferenceString[i], "%s", prefStringPossibleValue[i][1]);
			}
		else {

			}
		}*/
}

/* save preferences values in a 'spectrum3drc' preferences file */
void print_rc_file(){
	printf("Saving preferences :\n");
	int i = 0;
	GError *error = NULL;
	char rcDirectoryPath[100] = "";

	/* Preference file ('spectrum3drc') is in the /HOME/.spectrum3d/ directory; this directory will be created first if it doesn't exist; if it cannot be created, error message will be generated */
	sprintf(rcDirectoryPath, "%s/.spectrum3d/", g_get_home_dir());
	printf(" - Looking for '/HOME/.spectrum3d' directory ... ");
	GFile *rcDirectory = g_file_new_for_path (rcDirectoryPath);
	if (g_file_make_directory (rcDirectory, NULL, &error)){
		printf("doesn't exist -> creating\n");
		}
	else {
		if (error != NULL){
			if (error->code == G_FILE_ERROR_ACCES){
				printf("OK\n");
				}
			else {
				gchar message[500];
				sprintf(message, "\nUnable to create directory '%s' : %s\n", rcDirectoryPath, error->message);
				printf ("%s\n", message);
				error_message_window(message);
				}
			g_error_free (error);
			}
		else {
			gchar *message = "\nUnable to create directory, but GError is NULL\n";
			printf("%s\n", message);
			error_message_window(message);
			}
		}

/* Preferences file will be open to be written with preferences values */
	rcFile = fopen(rcPath, "w+");
/* Check if opening was succefull, then write current values to the file, then close it */
	if (rcFile != NULL){
		int result = 0;
		printf(" - Opening rc file and writing preferences values");
		fprintf(rcFile, "# This is an spectrum3drc file autogenerated by Spectrum3d \n");
		for (i = 0; i < sizeofPrefInt ; i++){
			result = fprintf(rcFile, "# %s \n%d\n", preferenceInt[i].name, *preferenceInt[i].var);
			ERROR_MESSAGE_PRINT_IN_RCFILE()
			}
		for (i = 0; i < sizeofPrefGLFloat ; i++){
			fprintf(rcFile, "# %s \n%f\n", preferenceGLFloat[i].name, *preferenceGLFloat[i].var);
			ERROR_MESSAGE_PRINT_IN_RCFILE()
			}
		for (i = 0; i < sizeofPrefGbool ; i++){
			fprintf(rcFile, "# %s \n%d\n", preferenceGbool[i].name, *preferenceGbool[i].var);
			ERROR_MESSAGE_PRINT_IN_RCFILE()
			}
		/*for (i = 0; i < sizeofPrefString ; i++){
			fprintf(rcFile, "# %s \n%s\n", prefStringPossibleValue[i][0], preferenceString[i]);
			ERROR_MESSAGE_PRINT_IN_RCFILE()
			}*/
		fclose(rcFile);
		printf("... OK\n");
		}
	else {
		printf(" -> Preferences file cannot be open\n");
		}
}

/* Check if retrieved values from preferences file are within their normal range */ 
void compare_values(){
	int i = 0;
	gboolean error = FALSE, match = FALSE;
	printf("Checking values retrieved from rc file for consistency\n");

	for (i = 0; i < sizeofPrefInt; i++){
		if (*preferenceInt[i].var < preferenceInt[i].min || *preferenceInt[i].var > preferenceInt[i].max){ 
			ERROR_MESSAGE_COMPARE_VALUES(preferenceInt[i].name)
			} 
		}

	for (i = 0; i < sizeofPrefGLFloat; i++){
		if (*preferenceGLFloat[i].var < preferenceGLFloat[i].min || *preferenceGLFloat[i].var > preferenceGLFloat[i].max){ 
			ERROR_MESSAGE_COMPARE_VALUES(preferenceGLFloat[i].name)
			} 
		}

	for (i = 0; i < sizeofPrefGbool; i++){
		if (*preferenceGbool[i].var < preferenceGbool[i].min || *preferenceGbool[i].var > preferenceGbool[i].max){ 
			ERROR_MESSAGE_COMPARE_VALUES(preferenceGbool[i].name)
			} 
		}

	/*for (i = 0; i < sizeofPrefString; i++){
		int a = 0;
		match = FALSE;
		for (a = 2; a < 4; a++){
			char *test = strstr(preferenceString[i], prefStringPossibleValue[i][a]);
			if (test != NULL){
				match = TRUE;
				}
			}
		if (match == FALSE){
			printf("\n'%s' seems not to be correct -> using default values and saving them as default\n", prefStringPossibleValue[i][0]);
			set_default_values();
			//print_rc_file("def");
			i = sizeofPrefString;
			} 
		}*/
	if (error == FALSE && match == TRUE){
		printf("  ... OK\n");
		}
}

/* Get the saved values from the configuration file */
void get_saved_values(){
	printf("Getting saved rom rc file\n"); 
	char confString[105]="";
	int i = 0;
	
/* Open preferences file if it exists; if it doesn't exists, all values will bet set to default values */
	sprintf(rcPath, "%s/.spectrum3d/spectrum3drc", g_get_home_dir());
	rcFile = fopen(rcPath, "r+"); 
	if (rcFile != NULL){
		printf("Opening the 'spectrum3drc' file and getting the saved values\n");
		char *result = NULL;
		i = 0;
		while (i < sizeofPrefInt){ 
			result = fgets(confString, 100, rcFile); 
			ERROR_MESSAGE_GET_PREFERENCES() 
			if (strchr(confString, '#') == NULL){
				*preferenceInt[i].var = strtol(confString, NULL, 10);
				i++;
				}
			}
		i = 0;
		while (i < sizeofPrefGLFloat){ 
			result = fgets(confString, 100, rcFile); 
			ERROR_MESSAGE_GET_PREFERENCES()
			if (strchr(confString, '#') == NULL){
				*preferenceGLFloat[i].var = strtof(confString, NULL);
				i++;
				}
			}
		i = 0;
		while (i < sizeofPrefGbool){ 
			result = fgets(confString, 100, rcFile);
			ERROR_MESSAGE_GET_PREFERENCES() 
			if (strchr(confString, '#') == NULL){
				*preferenceGbool[i].var = strtol(confString, NULL, 10);
				i++;
				}
			}
		/*i = 0;
		while (i < sizeofPrefString){ 
			result = fgets(confString, 100, rcFile); 
				if (strchr(confString, '#') == NULL){
					sprintf(preferenceString[i], "%s", confString);
					preferenceString[i][strlen(preferenceString[i])-1] = 0;
					i++;
					}
			}*/
		fclose(rcFile);	
		compare_values();
		}
	else {  
//If the 'preferences file doesn't exist, create one with default values
		printf ("WARNING : RC file doesn't exist or cannot be open; this is normal if you run Spectrum3d for the first time; \n");
		set_default_values();
		}
}



