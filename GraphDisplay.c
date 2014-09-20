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

static void ClassPartInitialize();
static void Initialize();
static void Destroy();
static Boolean SetValues();

#define Superclass	(&objectClassRec)
GraphDisplayClassRec graphDisplayClassRec = {
	/* object */
	{
		/* superclass */		(WidgetClass) Superclass,
		/* class_name */		"GraphDisplay",
		/* widget_size */		sizeof(GraphDisplayRec),
		/* class_initialize */		NULL,
		/* class_part_initialize */	ClassPartInitialize,
		/* class_inited */		False,
		/* initialize */		Initialize,
		/* initialize_hook */		NULL,
		/* obj1 */			NULL,
		/* obj2 */			NULL,
		/* obj3 */			0,
		/* resources */			resources,
		/* num_resources */		XtNumber(resources),
		/* xrm_class */			NULLQUARK,
		/* obj4 */			0,
		/* obj5 */			0,
		/* obj6 */			0,
		/* obj7 */			0,
		/* destroy */			Destroy,
		/* obj8 */			0,
		/* obj9 */			0,
		/* set_values */		SetValues,
		/* set_values_hook */		NULL,
		/* obj10 */			NULL,
		/* get_values_hook */		NULL,
		/* accept_focus */		NULL,
		/* version */			XtVersion,
		/* callback offsets */		NULL,
		/* obj12 */			NULL,
		/* obj13 */			NULL,
		/* obj14 */			NULL,
		/* extension */			NULL,
	},
	/* graphDisplay */
	{
		/* compute_size */		NULL,
		/* expose */			NULL,
		/* extension */			NULL,
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

	if (XtIsRealized(XtParent((Widget)newgd)))
		XClearArea(XtDisplayOfObject(new),
			XtWindowOfObject(new), 0, 0, 0, 0, True);
#undef NE
	
	return False;
}

static void
Destroy(Widget w)
{
	GraphDisplayObject gd = (GraphDisplayObject) w;

	XtReleaseGC(XtParent(w), gd->graphDisplay.gc);
}
