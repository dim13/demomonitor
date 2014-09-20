/* $Id$ */

#ifndef _BarDisplayP_h
#define _BarDisplayP_h

#include "BarDisplay.h"
#include "GraphDisplayP.h"

typedef struct {
	Dimension space;
	Dimension default_graph_width;
	String format;
} BarDisplayPart;

typedef struct _BarDisplayRec {
	ObjectPart object;
	GraphDisplayPart graphDisplay;
	BarDisplayPart barDisplay;
} BarDisplayRec;

typedef struct {
	XtPointer extension;
} BarDisplayClassPart;

typedef struct _BarDisplayClassRec {
	ObjectClassPart object_class;
	GraphDisplayClassPart graphDisplay_class;
	BarDisplayClassPart barDisplay_class;
} BarDisplayClassRec, *BarDisplayObjectClass;

extern BarDisplayClassRec barDisplayClassRec;

#endif
