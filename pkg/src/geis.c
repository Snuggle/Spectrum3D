/* This file is part of Spectrum3D. It is based on the geis2.c file from 
the utouch-geis-2.0.10 at https://launchpad.net/canonical-multitouch/utouch-geis

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

#include "config.h"

#ifdef HAVE_LIBUTOUCH_GEIS
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <geis/geis.h>
#include <libbamf/bamf-matcher.h>

#include "geis.h"
#include "spectrum3d.h"

char *getCurrentApp()
{
	g_type_init();
	char *windowName;
   	BamfWindow *window = bamf_matcher_get_active_window (bamf_matcher_get_default());
	if (window) {
		windowName = (char *) bamf_view_get_name(BAMF_VIEW(window));
		return windowName;
		} 
	else {
		printf("Libbamf : Cannot get the window name\n");
		return "";
		}	
}

void setupGeis()
{
	printf("Set up Geis\n");
#ifdef GEIS_12
	geis = geis_new(GEIS_INIT_TRACK_DEVICES, GEIS_INIT_TRACK_GESTURE_CLASSES, NULL);
	geis_get_configuration(geis, GEIS_CONFIGURATION_FD, &fd);

	subscription = geis_subscription_new(geis, "example", GEIS_SUBSCRIPTION_CONT);
#endif
#ifndef GEIS_12
	//GeisFilter filter;
	geis = geis_new(GEIS_INIT_TRACK_DEVICES, NULL);
	subscription = geis_subscription_new(geis, "example", GEIS_SUBSCRIPTION_CONT);
	/*filter = geis_filter_new(geis, "filter");
	
	geis_filter_add_term(filter,
			GEIS_FILTER_CLASS,
                        GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, 2,
			NULL);
	
	status = geis_subscription_add_filter(subscription, filter);*/
	status = geis_subscription_activate(subscription);

	geis_get_configuration(geis, GEIS_CONFIGURATION_FD, &fd);
#endif

	if (spectrum3d.enableTouch){
		timeoutTouch = g_timeout_add (50, (GSourceFunc) geisGesture, NULL);
		}		
}

void geisQuit()
{
	g_source_remove(timeoutTouch);
	geis_subscription_delete(subscription);
  	geis_delete(geis);
}


void processTouchEvent(int drag, int pinch, int rotate, int tap, int touchNumber, float velocityX, float velocityY, float radialVelocity, float angularVelocity, float tapTime)
{
	if (drag) {
		if (touchNumber == 1){
			AngleH+=velocityX/2;
			AngleV+=velocityY/2;
			}
		else if (touchNumber == 2){
			X+=velocityX/100;
			Y-=velocityY/100;
			}
		}
	else if (pinch) {
		Z+=radialVelocity/100;
		}
	else if (rotate) {
		AngleZ-=angularVelocity * 100;
		}
	else if (tap && tapTime > 150) {
		playFromSource(NULL, NULL);
		}
}

/*void print_attr(GeisAttr attr)
{
  GeisString attr_name = geis_attr_name(attr);
  switch (geis_attr_type(attr))
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      printf("  \"%s\": %s\n", attr_name,
             geis_attr_value_to_boolean(attr) ? "true" : "false");
      break;
    case GEIS_ATTR_TYPE_FLOAT:
      printf("  \"%s\": %g\n", attr_name, geis_attr_value_to_float(attr));
      break;
    case GEIS_ATTR_TYPE_INTEGER:
      printf("  \"%s\": %d\n", attr_name, geis_attr_value_to_integer(attr));
      break;
    case GEIS_ATTR_TYPE_STRING:
      printf("  \"%s\": %s\n", attr_name, geis_attr_value_to_string(attr));
      break;
    default:
      break;
  }
}*/


/*void
dump_device_event(GeisEvent event)
{
  GeisDevice device;
  GeisAttr attr;
  GeisSize i;
  //GeisInputDeviceId device_id = 0;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  device = geis_attr_value_to_pointer(attr);
  printf("device %02d \"%s\"\n",
         geis_device_id(device), geis_device_name(device));
  for (i = 0; i < geis_device_attr_count(device); ++i)
  {
    print_attr(geis_device_attr(device, i));
  }
}*/


gboolean dump_gesture_event(GeisEvent event)
{
	GeisSize i;
	GeisTouchSet touchset;
	GeisGroupSet groupset;
	GeisAttr     attr;
	int drag = 0, pinch = 0, rotate = 0, tap = 0, touchNumber = 0;
	float velocityX = 0, velocityY = 0, radialVelocity = 0, angularVelocity = 0, tapTime = 0;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  touchset = geis_attr_value_to_pointer(attr);

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  groupset = geis_attr_value_to_pointer(attr);

  //printf("gesture\n");
  for (i= 0; i < geis_groupset_group_count(groupset); ++i) {
    GeisSize j;
    GeisGroup group = geis_groupset_group(groupset, i);
    //printf("+group %u\n", geis_group_id(group));

    for (j=0; j < geis_group_frame_count(group); ++j) {
      GeisSize k;
      GeisFrame frame = geis_group_frame(group, j);
      GeisSize attr_count = geis_frame_attr_count(frame);

      //printf("+frame %u\n", geis_frame_id(frame));
	for (k = 0; k < attr_count; ++k) {
		//print_attr(geis_frame_attr(frame, k));
		GeisAttr attrIndex = geis_frame_attr(frame, k);
		GeisString attr_name = geis_attr_name(geis_frame_attr(frame, k));
		if (geis_attr_type(attrIndex) == GEIS_ATTR_TYPE_STRING) {
			if ((strcmp(geis_attr_value_to_string(attrIndex), "Drag,touch=2") == 0) || (strcmp(geis_attr_value_to_string(attrIndex), "Drag,touch=1")== 0)) {
				drag = 1;
				}
			else if ((strcmp(geis_attr_value_to_string(attrIndex), "Pinch,touch=2") == 0)) {
				pinch = 1;
				}
			else if ((strcmp(geis_attr_value_to_string(attrIndex), "Rotate,touch=2") == 0)) {
				rotate = 1;
				}
			else if ((strcmp(geis_attr_value_to_string(attrIndex), "Tap,touch=2") == 0)) {
				tap = 1;
				}			
			} 
		else if (geis_attr_type(attrIndex) == GEIS_ATTR_TYPE_INTEGER) {
			if (strcmp(attr_name, "touches") == 0) {
				touchNumber = geis_attr_value_to_integer(attrIndex);
				}
			} 
		else if (geis_attr_type(attrIndex) == GEIS_ATTR_TYPE_FLOAT) {
			if (strcmp(attr_name, "velocity x") == 0) {
				velocityX = geis_attr_value_to_float(attrIndex);
				}
			else if (strcmp(attr_name, "velocity y") == 0) {
				velocityY = geis_attr_value_to_float(attrIndex);
				}
			else if (strcmp(attr_name, "radial velocity") == 0) {
				radialVelocity = geis_attr_value_to_float(attrIndex);
				}
			else if (strcmp(attr_name, "angular velocity") == 0) {
				angularVelocity = geis_attr_value_to_float(attrIndex);
				}
			else if (strcmp(attr_name, "tap time") == 0) {
				tapTime = geis_attr_value_to_float(attrIndex);
				}
			} 
		}
	
	processTouchEvent(drag, pinch, rotate, tap, touchNumber, velocityX, velocityY, radialVelocity, angularVelocity, tapTime);
	
      /*for (k = 0; k < geis_frame_touchid_count(frame); ++k)
      {
	GeisSize  touchid = geis_frame_touchid(frame, k);
	GeisTouch touch = geis_touchset_touch_by_id(touchset, touchid);
	GeisSize  n;
	printf("+touch %lu\n", k);
	for (n = 0; n < geis_touch_attr_count(touch); ++n)
	{
	  print_attr(geis_touch_attr(touch, n));
	}
      }*/
    }
  }
if (drag == 1 || pinch == 1 || rotate == 1){
	newEvent = TRUE;
	return TRUE;
	}
else {
	return FALSE;
	}
}

#ifdef GEIS_12
void
target_subscription(Geis geis, GeisSubscription subscription)
{
  GeisStatus status;
  GeisFilter filter = geis_filter_new(geis, "filter");
  geis_filter_add_term(filter,
                       GEIS_FILTER_CLASS,
                       GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, 2,
                       NULL);

  status = geis_subscription_add_filter(subscription, filter);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error adding filter\n");
  }
}
#endif


gboolean geisGesture()
{
	char *windowName = getCurrentApp();
	//printf("windowName = %s\n", windowName);
	GeisEvent eventGeis;
	status = geis_dispatch_events(geis);
	status = geis_next_event(geis, &eventGeis);
	while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS){
		switch (geis_event_type(eventGeis)){
#ifdef GEIS_12
		case GEIS_EVENT_INIT_COMPLETE:
		    target_subscription(geis, subscription);
		    status = geis_subscription_activate(subscription);
		    break;
#endif
		  /*case GEIS_EVENT_DEVICE_AVAILABLE:
		  case GEIS_EVENT_DEVICE_UNAVAILABLE:
		    dump_device_event(eventGeis);
		    break;*/

		case GEIS_EVENT_GESTURE_BEGIN:
		case GEIS_EVENT_GESTURE_UPDATE:
		case GEIS_EVENT_GESTURE_END:
			if (strcmp(windowName, PACKAGE_NAME) == 0 || strcmp(windowName, PACKAGE) == 0) {
		    		event = dump_gesture_event(eventGeis);
				}
			break;
		default:
		    break;
		}
	geis_event_delete(eventGeis);
	status = geis_next_event(geis, &eventGeis);
	}
return TRUE;
}

#endif




