
#ifndef DEFINE_EFFECTS
#define DEFINE_EFFECTS

gboolean showEqualizerWindow;
int playing;
gfloat BPlowerFreq, BPupperFreq;
GtkWidget *effectsWindow, *buttonEqualizer, *comboFilter;
GstElement *equalizer, *equalizer2, *equalizer3, *BP_BRfilter;

#endif



