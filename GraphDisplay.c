/* $Id$ */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "GraphDisplayP.h"

#define Offset(field) XtOffsetOf(GraphDisplayRec, graphDisplay.field)
static XtResource resources[] = {
	{ XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
		Offset(font), XtRString, (XtPointer)XtDefaultFont },
	{ XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
		Offset(foreground), XtRString, (XtPointer)XtDefaultForeground },
};
#undef Offset

static void ClassPartInitialize(WidgetClass);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

GraphDisplayClassRec graphDisplayClassRec = {
	.object_class = {
		.superclass		= (WidgetClass)&objectClassRec,
		.class_name		= "GraphDisplay",
		.widget_size		= sizeof(GraphDisplayRec),
		.class_initialize	= NULL,
		.class_part_initialize	= ClassPartInitialize,
		.class_inited		= False,
		.initialize		= Initialize,
		.initialize_hook	= NULL,
		.resources		= resources,
		.num_resources		= XtNumber(resources),
		.xrm_class		= NULLQUARK,
		.destroy		= Destroy,
		.set_values		= SetValues,
		.set_values_hook	= NULL,
		.get_values_hook	= NULL,
		.version		= XtVersion,
		.callback_private	= NULL,
		.extension		= NULL,
	},
	.graphDisplay_class = {
		.compute_size		= NULL,
		.expose			= NULL,
		.extension		= NULL,
	},
};

WidgetClass graphDisplayObjectClass = (WidgetClass) &graphDisplayClassRec;

static void
ClassPartInitialize(WidgetClass widgetClass)
{
	GraphDisplayObjectClass wc = (GraphDisplayObjectClass) widgetClass;
	GraphDisplayObjectClass super =
		(GraphDisplayObjectClass) wc->object_class.superclass;

	if (wc->graphDisplay_class.compute_size == InheritComputeSize)
		wc->graphDisplay_class.compute_size =
			super->graphDisplay_class.compute_size;
	if (wc->graphDisplay_class.expose == XtInheritExpose)
		wc->graphDisplay_class.expose =
			super->graphDisplay_class.expose;
}

static GC
GetGC(GraphDisplayObject gd)
{
	XGCValues values;

	values.foreground = gd->graphDisplay.foreground;
	values.font = gd->graphDisplay.font->fid;

	return XtGetGC(XtParent((Widget)gd), GCForeground|GCFont, &values);
}

static void
Initialize(Widget request,
	Widget new,
	ArgList args,
	Cardinal * num_args)
{
	GraphDisplayObject gd = (GraphDisplayObject) new;

	gd->graphDisplay.gc = GetGC(gd);
}

static Boolean
SetValues(Widget old,
	Widget req,
	Widget new,
	ArgList args,
	Cardinal *num_args)
{
	GraphDisplayObject oldgd = (GraphDisplayObject) old;
	GraphDisplayObject newgd = (GraphDisplayObject) new;

#define NE(field)	(newgd->field != oldgd->field)
	if (NE(graphDisplay.foreground) || NE(graphDisplay.font->fid)) {
		XtReleaseGC(new, oldgd->graphDisplay.gc);
		newgd->graphDisplay.gc = GetGC(newgd);
	}
#undef NE

	return XtIsRealized(XtParent(new));
}

static void
Destroy(Widget w)
{
	GraphDisplayObject gd = (GraphDisplayObject) w;

	XtReleaseGC(XtParent(w), gd->graphDisplay.gc);
}
