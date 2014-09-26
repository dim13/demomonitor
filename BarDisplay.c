/* $Id$ */

#include <stdio.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "BarDisplayP.h"
#include "GraphP.h"

#define Offset(field) XtOffsetOf(BarDisplayRec, barDisplay.field)
static XtResource resources[] = {
	{ XtNspace, XtCSpace, XtRDimension, sizeof(Dimension),
		Offset(space), XtRImmediate, (XtPointer) 5 },
	{ XtNdefaultGraphWidth, XtCDefaultGraphWidth,
		XtRDimension, sizeof(Dimension),
		Offset(default_graph_width), XtRImmediate, (XtPointer) 200 },
	{ XtNformat, XtCFormat, XtRString, sizeof(String),
		Offset(format), XtRString, "%g" },
};
#undef Offset

static void Initialize();
static void Redisplay();
static void ComputeSize();
static Boolean SetValues();

BarDisplayClassRec barDisplayClassRec = {
	.object_class = {
		.superclass		= (WidgetClass)&graphDisplayClassRec,
		.class_name		= "BarDisplay",
		.widget_size		= sizeof(BarDisplayRec),
		.class_initialize	= NULL,
		.class_part_initialize	= NULL,
		.class_inited		= False,
		.initialize		= Initialize,
		.initialize_hook	= NULL,
		.resources		= resources,
		.num_resources		= XtNumber(resources),
		.xrm_class		= NULLQUARK,
		.destroy		= NULL,
		.set_values		= SetValues,
		.set_values_hook	= NULL,
		.get_values_hook	= NULL,
		.version		= XtVersion,
		.callback_private	= NULL,
		.extension		= NULL,
	},
	.graphDisplay_class = {
		.compute_size		= ComputeSize,
		.expose			= Redisplay,
		.extension		= NULL,
	},
	.barDisplay_class = {
		.extension		= NULL,
	},
};

WidgetClass barDisplayObjectClass = (WidgetClass) &barDisplayClassRec;

static void
Initialize(Widget request,
	Widget new,
	ArgList args,
	Cardinal * num_args)
{
	BarDisplayObject bd = (BarDisplayObject) new;

	bd->barDisplay.format = XtNewString(bd->barDisplay.format);
}

static Boolean
SetValues(Widget old,
	Widget req,
	Widget new,
	ArgList args,
	Cardinal *num_args)
{
	BarDisplayObject oldbd = (BarDisplayObject) old;
	BarDisplayObject newbd = (BarDisplayObject) new;

#define NE(field)	(oldbd->barDisplay.field != newbd->barDisplay.field)
	return XtIsRealized(XtParent(new)) && (NE(space) || NE(format));
#undef NE
}

static void
ComputeLabelDimensions(BarDisplayObject bd,
	Dimension *label_w,
	Dimension *total_w,
	Dimension *height)
{
	XFontStruct *fs = bd->graphDisplay.font;
	int i;
	int width;
	GraphWidget parent = (GraphWidget) XtParent((Widget) bd);
	char buf[100];

	*label_w = *total_w = 0;
	if (parent->graph.labels != NULL) {
		for (i = 0; i < parent->graph.num_entries; i++) {
			width = XTextWidth(fs, parent->graph.labels[i],
				strlen(parent->graph.labels[i]));
			if (width > *label_w)
				*label_w = width;
		}
	}

	for (i = 0; i < parent->graph.num_entries; i++) {
		snprintf(buf, sizeof(buf), bd->barDisplay.format,
			(float)parent->graph.values[i] / parent->graph.scale);
		width = XTextWidth(fs, buf, strlen(buf));
		if (width > *total_w)
			*total_w = width;
	}

	*total_w += *label_w;
	*height = fs->max_bounds.ascent + fs->max_bounds.descent;
}

static void
ComputeSize(GraphWidget w)
{
	BarDisplayObject bd = (BarDisplayObject) w->composite.children[0];
	Dimension label_width, total_width, label_height;

	ComputeLabelDimensions(bd, &label_width, &total_width, &label_height);

	if (w->core.width == 0) {
		w->core.width = 4 * bd->barDisplay.space + total_width +
			bd->barDisplay.default_graph_width;
	}

	if (w->core.height == 0) {
		w->core.height = w->graph.num_entries * (bd->barDisplay.space +
			label_height) + bd->barDisplay.space;
	}
}

static void
Redisplay(GraphWidget w,
	XEvent *event,
	Region region)
{
	BarDisplayObject bd = (BarDisplayObject) w->composite.children[0];
	Dimension label_width, total_width, label_height;
	Boolean displayBars;
	int i;
	int x, y, bar_width;
	char buf[100];
	int *values = w->graph.values;
	String *labels = w->graph.labels;

	ComputeLabelDimensions(bd, &label_width, &total_width, &label_height);

	bar_width = w->core.width - total_width - 4 * bd->barDisplay.space;
	displayBars = (bar_width > (int)bd->barDisplay.space);

	y = bd->barDisplay.space;
	for (i = 0; i < w->graph.num_entries; i++) {
		if (labels != NULL) {
			XDrawString(XtDisplay(w), XtWindow(w),
				bd->graphDisplay.gc,
				bd->barDisplay.space,
				y + bd->graphDisplay.font->max_bounds.ascent,
				labels[i], strlen(labels[i]));
			x = label_width + 2 * bd->barDisplay.space;
		} else
			x = 0;
		
		if (displayBars) {
			XFillRectangle(XtDisplay(w), XtWindow(w),
				bd->graphDisplay.gc, x, y,
				bar_width * values[i] / w->graph.max_value,
				bd->graphDisplay.font->max_bounds.ascent);
			x += bar_width * values[i] / w->graph.max_value +
				bd->barDisplay.space;
		}

		snprintf(buf, sizeof(buf), bd->barDisplay.format,
			(float) values[i] / w->graph.scale);
		XDrawString(XtDisplay(w), XtWindow(w), bd->graphDisplay.gc,
			x, y + bd->graphDisplay.font->max_bounds.ascent,
			buf, strlen(buf));
		y += label_height + bd->barDisplay.space;
	}
}
