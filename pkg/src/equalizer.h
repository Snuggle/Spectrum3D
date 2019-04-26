
#ifndef DEFINE_EQUALIZER
#define DEFINE_EQUALIZER

gboolean showEqualizerWindow;
int playing;
gfloat BPlowerFreq, BPupperFreq;
GtkWidget *effectsWindow, *comboFilter;
GstElement *equalizer, *equalizer2, *equalizer3, *BP_BRfilter;

#endif



