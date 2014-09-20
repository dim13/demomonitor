/* $Id$ */

#ifndef _GraphDisplayP_h
#define _GraphDisplayP_h

// #include <X11/CoreP.h>
#include "GraphDisplay.h"

typedef struct {
	Pixel foreground;
	XFontStruct *font;
	GC gc;
} GraphDisplayPart;

typedef struct _GraphDisplayRec {
	ObjectPart object;
	GraphDisplayPart graphDisplay;
} GraphDisplayRec;

typedef void (*ComputeSizeProc)();

typedef struct {
	ComputeSizeProc compute_size;
	XtExposeProc expose;
	XtPointer extension;
} GraphDisplayClassPart;

typedef struct _GraphDisplayClassRec {
	ObjectClassPart object_class;
	GraphDisplayClassPart graphDisplay_class;
} GraphDisplayClassRec, *GraphDisplayObjectClass;

extern GraphDisplayClassRec graphDisplayClassRec;

#define InheritComputeSize ((ComputeSizeProc) _XtInherit)

#endif
