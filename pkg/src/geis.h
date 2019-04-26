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

#ifndef DEFINE_GEIS
#define DEFINE_GEIS

gboolean timeoutTouch, event, newEvent;
int playing, fd;
GLfloat X, Y, Z, AngleH, AngleV, AngleZ;
GeisStatus status;
Geis geis;
GeisSubscription subscription;

int geisGesture(); 
void playFromSource(GtkWidget *widget, gpointer *data);

#endif



