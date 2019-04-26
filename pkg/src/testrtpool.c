/* This file is part of Spectrum3D. It comes from the rtpool-test.c from Gstreamer 
at http://cgit.freedesktop.org/gstreamer/gstreamer/tree/tests/examples/streams 
(Copyright (C) 2009 Wim Taymans <wim.taymans@gmail.com>)

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

#include <pthread.h>
#include <string.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "testrtpool.h"
#include "spectrum3d.h"

static void test_rt_pool_finalize (GObject * object);

typedef struct
{
  pthread_t thread;
} TestRTId;

G_DEFINE_TYPE (TestRTPool, test_rt_pool, GST_TYPE_TASK_POOL);

static void
default_prepare (GstTaskPool * pool, GError ** error)
{
  /* we don't do anything here. We could construct a pool of threads here that
   * we could reuse later but we don't */
  g_message ("prepare Realtime pool %p", pool);
}

static void
default_cleanup (GstTaskPool * pool)
{
  g_message ("cleanup Realtime pool %p", pool);
}

static gpointer
default_push (GstTaskPool * pool, GstTaskPoolFunction func, gpointer data,
    GError ** error)
{
  TestRTId *tid;
  gint res;
  pthread_attr_t attr;
  struct sched_param param;

  //g_message ("pushing Realtime pool %p, %p", pool, func);

  tid = g_slice_new0 (TestRTId);

  //g_message ("set policy");
  pthread_attr_init (&attr);
  if (strstr(policyName, "FIFO")){
	if ((res = pthread_attr_setschedpolicy (&attr, SCHED_RR)) != 0)
	g_warning ("setschedpolicy: failure: %p", g_strerror (res));
	}
  else {
	if ((res = pthread_attr_setschedpolicy (&attr, SCHED_FIFO)) != 0)
	g_warning ("setschedpolicy: failure: %p", g_strerror (res));
	}

  //g_message ("set prio");
  param.sched_priority = spectrum3d.priority;
  if ((res = pthread_attr_setschedparam (&attr, &param)) != 0)
    g_warning ("setschedparam: failure: %p", g_strerror (res));

  //g_message ("set inherit");
  if ((res = pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED)) != 0)
    g_warning ("setinheritsched: failure: %p", g_strerror (res));

  //g_message ("create thread");
  res = pthread_create (&tid->thread, &attr, (void *(*)(void *)) func, data);
	
  if (res == 0) {
	printf("REALTIME mode (%s, p = %d)\n", policyName, spectrum3d.priority);
	}
  else if (res != 0) {
    g_set_error (error, G_THREAD_ERROR, G_THREAD_ERROR_AGAIN,
        "Error creating thread: %s", g_strerror (res));
    g_slice_free (TestRTId, tid);
    tid = NULL;
  }

  return tid;
}

static void
default_join (GstTaskPool * pool, gpointer id)
{
  TestRTId *tid = (TestRTId *) id;

  //g_message ("joining Realtime pool %p", pool);

  pthread_join (tid->thread, NULL);

  g_slice_free (TestRTId, tid);
}

static void
test_rt_pool_class_init (TestRTPoolClass * klass)
{
  GObjectClass *gobject_class;
  GstTaskPoolClass *gsttaskpool_class;

  gobject_class = (GObjectClass *) klass;
  gsttaskpool_class = (GstTaskPoolClass *) klass;

  gobject_class->finalize = GST_DEBUG_FUNCPTR (test_rt_pool_finalize);

  gsttaskpool_class->prepare = default_prepare;
  gsttaskpool_class->cleanup = default_cleanup;
  gsttaskpool_class->push = default_push;
  gsttaskpool_class->join = default_join;
}

static void
test_rt_pool_init (TestRTPool * pool)
{
}

static void
test_rt_pool_finalize (GObject * object)
{
  G_OBJECT_CLASS (test_rt_pool_parent_class)->finalize (object);
}

GstTaskPool *
test_rt_pool_new (void)
{
  GstTaskPool *pool;
  pool = g_object_new (TEST_TYPE_RT_POOL, NULL);

  return pool;
}




