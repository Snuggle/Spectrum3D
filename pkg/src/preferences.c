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
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "config.h"

#ifdef HAVE_LIBUTOUCH_GEIS
	#include <geis/geis.h>
#endif

#include "preferences.h"

char prefPath[100], rcPath[100];
GtkWidget *playButton, *pScaleStart, *displayLabel;

int sizeofPrefInt = 4;
PreferenceInt preferenceInt[4] = {{&flatviewDefinition, 400, 200, 400, "flatviewDefinition"}, {&presetWidth, 850, 700, 1500, "presetWidth"}, {&interval, 150, 100, 250, "interval"}, {&priority, 50, 50, 80, "priority"}};  // FIXME interval is guint64

int sizeofPrefFloat = 6;
PreferenceFloat preferenceFloat[6] = {{&presetX, -0.495833, 0, 0, "presetX"}, {&presetY, -0.070833, 0, 0, "presetY"}, {&presetZ, -1.050000, 0, 0, "presetZ"}, {&presetAngleH, -16.000000, 0, 0, "presetAngleH"}, {&presetAngleV, -10.000000, 0, 0, "presetAngleV"}, {&presetAngleZ, 0.000000, 0, 0, "presetAngleZ"}};

int sizeofPrefGbool = 2;
PreferenceGbool preferenceGbool[2] = {{&realtime, FALSE, "realtime"}, {&enableTouch, TRUE, "enableTouch"}};

int sizeofPrefString = 2;
char *preferenceString[2] = {policyName, fontPreference};
char prefStringPossibleValue[2][4][40] = {{"policyName", "SCHED_RR", "SCHED_RR", "SCHED_FIFO"}, {"fontPreference", "null", ".ttf", "null"}};

#define PRINT_TEST printf("print test = %s\n", preferenceString[0]);

void set_ttf_file(){
	char fontHomePath[100];
	if (g_file_test("/usr/local/share/fonts/FreeSans.ttf", G_FILE_TEST_IS_REGULAR)) {
			sprintf(fontPreference, "/usr/local/share/fonts/FreeSans.ttf");
			}
	else if (g_file_test("/usr/share/fonts/FreeSans.ttf", G_FILE_TEST_IS_REGULAR)) {
		sprintf(fontPreference, "/usr/share/fonts/FreeSans.ttf");
		}
	else if (g_file_test(fontHomePath, G_FILE_TEST_IS_REGULAR) == 0) {
		sprintf(fontPreference, "%s", fontHomePath);
		}
	else {
		printf("WARNING : font file was not found; please select a '.ttf' font file via 'Menu->Edit->Preferences->Select Font'.\n");
		errorMessageWindow("WARNING : font file was not found \nPlease select a '.ttf' font file via 'Menu->Edit->Preferences->Select Font'.");
		sprintf(fontPreference, "null");
		} 
}

void set_default_values(){
	int i = 0;
	for (i = 0; i < sizeofPrefInt; i++){
		*preferenceInt[i].var = preferenceInt[i].def;
		}
	for (i = 0; i < sizeofPrefFloat; i++){
		*preferenceFloat[i].var = preferenceFloat[i].def;
		}
	for (i = 0; i < sizeofPrefGbool; i++){
		*preferenceGbool[i].var = preferenceGbool[i].def;
		}
	set_ttf_file();
	for (i = 0; i < sizeofPrefString; i++){
		if (strcmp(prefStringPossibleValue[i][1], "null") != 0){
			sprintf(preferenceString[i], "%s", prefStringPossibleValue[i][1]);
			}
		else {

			}
		}
}

void print_rc_file(const char *data){
	int i = 0;
	rcFile = fopen(rcPath, "w+");
	for (i = 0; i < sizeofPrefInt ; i++){
		if (strcmp(data, "var") == 0){
			fprintf(rcFile, "# %s \n%d\n", preferenceInt[i].name, *preferenceInt[i].var);
			}
		else if (strcmp(data, "def") == 0){
			fprintf(rcFile, "# %s \n%d\n", preferenceInt[i].name, preferenceInt[i].def);
			}
		}
	for (i = 0; i < sizeofPrefFloat ; i++){
		if (strcmp(data, "var") == 0){
			fprintf(rcFile, "# %s \n%f\n", preferenceFloat[i].name, *preferenceFloat[i].var);
			}
		else if (strcmp(data, "def") == 0){
			fprintf(rcFile, "# %s \n%f\n", preferenceFloat[i].name, preferenceFloat[i].def);
			}
		}
	for (i = 0; i < sizeofPrefGbool ; i++){
		if (strcmp(data, "var") == 0){
			fprintf(rcFile, "# %s \n%d\n", preferenceGbool[i].name, *preferenceGbool[i].var);
			}
		else if (strcmp(data, "def") == 0){
			fprintf(rcFile, "# %s \n%d\n", preferenceGbool[i].name, preferenceGbool[i].def);
			}
		}
	for (i = 0; i < sizeofPrefString ; i++){
		if (strcmp(data, "var") == 0){
			fprintf(rcFile, "# %s \n%s\n", prefStringPossibleValue[i][0], preferenceString[i]);
			}
		else if (strcmp(data, "def") == 0){
			if (strcmp(prefStringPossibleValue[i][1], "null") != 0){
				fprintf(rcFile, "# %s \n%s\n", prefStringPossibleValue[i][0], prefStringPossibleValue[i][1]);
				}
			else {
				fprintf(rcFile, "# %s \n%s\n", prefStringPossibleValue[i][0], preferenceString[i]);
				}
			}
		}
	fclose(rcFile);
}

void compare_values(){
	int i = 0;
	gboolean error = FALSE, match = FALSE;
	printf("Checking values retrieved from rc file for consistency ");
	for (i = 0; i < sizeofPrefInt; i++){
		if (*preferenceInt[i].var < preferenceInt[i].min || *preferenceInt[i].var > preferenceInt[i].max){
			printf("\n'%s' seems out of range -> using default values and saving them as default\n", preferenceInt[i].name);
			set_default_values();
			print_rc_file("def");
			error = TRUE;
			}
		}
	for (i = 0; i < sizeofPrefString; i++){
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
			print_rc_file("def");
			i = sizeofPrefString;
			} 
		}
	if (error == FALSE && match == TRUE){
		printf("  ... OK\n");
		}
}

/* Get the saved values from the configuration file */
void get_saved_values(){
	char confString[105]="";
	int i = 0;

	//sprintf(prefPath, "%s/.spectrum3d/spectrum3d.pref", getenv("HOME"));
	sprintf(rcPath, "%s/.spectrum3d/spectrum3drc", g_get_home_dir());
	rcFile = fopen(rcPath, "r+"); 
	if (rcFile != NULL){
		printf("Opening the 'spectrum3drc' file and getting the saved values\n");
		char *result = NULL;
		while (i < sizeofPrefInt){ 
			result = fgets(confString, 100, rcFile); 
				if (strchr(confString, '#') == NULL){
					*preferenceInt[i].var = strtol(confString, NULL, 10);
					//printf("%s = %d\n", preferenceInt[i].name, *preferenceInt[i].var);
					i++;
					}
			}
		i = 0;
		while (i < sizeofPrefFloat){ 
			result = fgets(confString, 100, rcFile); 
				if (strchr(confString, '#') == NULL){
					*preferenceFloat[i].var = strtof(confString, NULL);
					//printf("%s = %f\n", preferenceFloat[i].name, *preferenceFloat[i].var);
					i++;
					}
			}
		i = 0;
		while (i < sizeofPrefGbool){ 
			result = fgets(confString, 100, rcFile); 
				if (strchr(confString, '#') == NULL){
					*preferenceGbool[i].var = strtol(confString, NULL, 10);
					//printf("%s = %d\n", preferenceGbool[i].name, *preferenceGbool[i].var);
					i++;
					}
			}
		i = 0;
		while (i < sizeofPrefString){ 
			result = fgets(confString, 100, rcFile); 
				if (strchr(confString, '#') == NULL){
					sprintf(preferenceString[i], "%s", confString);
					preferenceString[i][strlen(preferenceString[i])-1] = 0;
					//printf("%s = %s\n", prefStringPossibleValue[i][0], preferenceString[i]);
					i++;
					}
			}
		fclose(rcFile);	
		compare_values();
		}
	else {  
//If the 'preferences file doesn't exist, create one with default values
		printf ("WARNING : RC file doesn't exist or cannot be open; this is normal if you run Spectrum3d for the first time; \n");
		//rcFile = fopen(rcPath, "r+");
		set_default_values();
		print_rc_file("def");
		}
}



