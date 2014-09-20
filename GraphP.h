#ifndef _GraphP_h
#define _GraphP_h

// #include <X11/CoreP.h>
#include "Graph.h"

typedef struct {
	int num_entries;
	String *labels;
	int *values;
	int max_value;
	int scale;
} GraphPart;

typedef struct _GraphRec {
	CorePart core;
	CompositePart composite;
	GraphPart graph;
} GraphRec;

typedef struct {
	XtPointer extension;
} GraphClassPart;

typedef struct _GraphClassRec {
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	GraphClassPart graph_class;
} GraphClassRec;

extern GraphClassRec graphClassRec;

#endif
