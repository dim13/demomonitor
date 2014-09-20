/* $Id$ */

#ifndef _Graph_h
#define _Graph_h

// #include <X11/Intrinsic.h>

#define XtNnumEntries	"numEntries"
#define XtCNumEntries	"NumEntries"
#define XtNlabels	"labels"
#define XtCLabels	"Labels"
#define XtNvalues	"values"
#define XtCValues	"Values"
#define XtNmaxValue	"maxValue"
#define XtCMaxValue	"MaxValue"
#define XtNscale	"scale"
#define XtCScale	"Scale"

extern WidgetClass graphWidgetClass;

typedef struct _GraphRec *GraphWidget;
typedef struct _GraphClassRec *GraphWidgetClass;

#endif
