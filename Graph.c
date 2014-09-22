/* $Id$ */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "GraphP.h"
#include "GraphDisplayP.h"

#define Offset(field) XtOffsetOf(GraphRec, graph.field)
static XtResource resources[] = {
	{ XtNnumEntries, XtCNumEntries, XtRInt, sizeof(int),
		Offset(num_entries), XtRImmediate, (XtPointer) 0 },
	{ XtNlabels, XtCLabels, XtRStringTable, sizeof(String *),
		Offset(labels), XtRImmediate, (XtPointer) NULL },
	{ XtNvalues, XtCValues, XtRPointer, sizeof(int *),
		Offset(values), XtRImmediate, (XtPointer) NULL },
	{ XtNmaxValue, XtCMaxValue, XtRInt, sizeof(int),
		Offset(max_value), XtRImmediate, (XtPointer) 100 },
	{ XtNscale, XtCScale, XtRInt, sizeof(int),
		Offset(scale), XtRImmediate, (XtPointer) 1 },
};
#undef Offset

static void ClassInitialize();
static void Initialize();
static void Redisplay();
static void Destroy();
static void Resize();
static void Realize();
static void InsertChild();
static Boolean SetValues();

static CompositeClassExtensionRec compositeExtension = {
	.next_extension		= NULL,
	.record_type		= NULLQUARK,
	.version		= XtCompositeExtensionVersion,
	.record_size		= sizeof(CompositeClassExtensionRec),
	.accepts_objects	= True,
	.allows_change_managed_set = False,
};

GraphClassRec graphClassRec = {
	.core_class = {
		.superclass		= (WidgetClass)&compositeClassRec,
		.class_name		= "Graph",
		.widget_size		= sizeof(GraphRec),
		.class_initialize	= ClassInitialize,
		.class_part_initialize	= NULL,
		.class_inited		= False,
		.initialize		= Initialize,
		.initialize_hook	= NULL,
		.realize		= Realize,
		.actions		= NULL,
		.num_actions		= 0,
		.resources		= resources,
		.num_resources		= XtNumber(resources),
		.xrm_class		= NULLQUARK,
		.compress_motion	= True,
		.compress_exposure	= XtExposeCompressMultiple,
		.compress_enterleave	= True,
		.visible_interest	= False,
		.destroy		= Destroy,
		.resize			= Resize,
		.expose			= Redisplay,
		.set_values		= SetValues,
		.set_values_hook	= NULL,
		.set_values_almost	= XtInheritSetValuesAlmost,
		.get_values_hook	= NULL,
		.accept_focus		= NULL,
		.version		= XtVersion,
		.callback_private	= NULL,
		.tm_table		= NULL,
		.query_geometry		= NULL,
		.display_accelerator	= NULL,
		.extension		= NULL,
	},
	.composite_class = {
		.geometry_manager	= NULL,
		.change_managed		= NULL,
		.insert_child		= InsertChild,
		.delete_child		= XtInheritDeleteChild,
		.extension		= &compositeExtension,
	},
	.graph_class = {
		.extension		= NULL,
	},
};

WidgetClass graphWidgetClass = (WidgetClass) &graphClassRec;

static Boolean
CvtStringToStringList(Display *dpy,
	XrmValuePtr args,
	Cardinal *num_args,
	XrmValuePtr from,
	XrmValuePtr to,
	XtPointer *data)
{
	int i, count = 1;
	char *ch, *start = from->addr;
	static String *list;
	int len;

	if (*num_args != 0) {
		XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
			"cvtStringToStringList", "wrongParameters",
			"XtToolkitError",
			"String to string list conversion needs no extra arguments",
			NULL, NULL);
	}

	if (to->addr != NULL && to->size < sizeof(String *)) {
		to->size = sizeof(String *);
		return False;
	}

	if (start == NULL || *start == '\0')
		list = NULL;
	else {
		for (ch = start; *ch != '\0'; ch++) {
			if (*ch == '\n')
				count++;
		}

		list = (String *)XtCalloc(count + 1, sizeof(String));
		for (i = 0; i < count; i++) {
			for (ch = start; *ch != '\n' && *ch != '\0'; ch++)
				;
			len = ch - start;
			list[i] = XtMalloc(len + 1);
			strncpy(list[i], start, len);
			list[i][len] = '\0';
			start = ch + 1;
		}
	}
	if (to->addr == NULL)
		to->addr = (caddr_t)&list;
	else
		*(String **)to->addr = list;
	to->size = sizeof(String *);

	return True;
}

static void
StringListDestructor(XtAppContext app,
	XrmValuePtr to,
	XtPointer converter_data,
	XrmValuePtr args,
	Cardinal *num_args)
{
	String *list = (String *)to->addr;
	String *entry;

	for (entry = list; entry != NULL; entry++)
		XtFree((XtPointer)entry);

	XtFree((XtPointer)list);
}

static void
ClassInitialize()
{
	XtSetTypeConverter(XtRString, XtRStringTable,
		CvtStringToStringList, NULL, 0,
		XtCacheAll|XtCacheRefCount, StringListDestructor);
}

static void
Initialize(Widget request,
	Widget new,
	ArgList args,
	Cardinal *num_args)
{
	GraphWidget gw = (GraphWidget) new;
	int *values;
	int i;

	values = (int *)XtCalloc(gw->graph.num_entries, sizeof(int));
	for (i = 0; i < gw->graph.num_entries; i++)
		values[i] = gw->graph.values[i];
	gw->graph.values = values;
}

static Boolean
SetValues(Widget old,
	Widget req,
	Widget new,
	ArgList args,
	Cardinal *num_args)
{
	GraphWidget oldgraph = (GraphWidget) old;
	GraphWidget newgraph = (GraphWidget) new;
	int *values;
	int i;

#define NE(field)	(newgraph->graph.field != oldgraph->graph.field)
	if (NE(values)) {
		values = (int *)XtCalloc(newgraph->graph.num_entries, sizeof(int));
		XtFree((XtPointer)oldgraph->graph.values);
		for (i = 0; i < newgraph->graph.num_entries; i++)
			values[i] = newgraph->graph.values[i];
		newgraph->graph.values = values;
		return True;
	}

#define EQ(field)	(!NE(field))
	if (NE(num_entries) && (EQ(labels) || EQ(values))) {
		XtAppErrorMsg(XtWidgetToApplicationContext(new),
			"counterError", "numEntries", "WidgetError",
			"Number of graph entries changed but not labels of values",
			NULL, NULL);
	}
#undef EQ

	return NE(num_entries) || NE(labels) || NE(max_value);
#undef NE
}

static void
Destroy(Widget w)
{
	GraphWidget gw = (GraphWidget) w;

	XtFree((XtPointer)gw->graph.values);
}

static void
InsertChild(Widget w)
{
	String params[2];
	Cardinal num_params;
	CompositeWidget parent = (CompositeWidget) XtParent(w);
	GraphDisplayObjectClass childClass;

	if (!XtIsSubclass(w, graphDisplayObjectClass)) {
		params[0] = XtClass(w)->core_class.class_name;
		params[1] = XtClass(parent)->core_class.class_name;
		num_params = 2;
		XtAppErrorMsg(XtWidgetToApplicationContext(w),
			"childError", "number", "WidgetError",
			"Children of class %s cannot be added to %n widgets",
			params, &num_params);
	}

	if (parent->composite.num_children != 0) {
		params[0] = XtClass(parent)->core_class.class_name;
		num_params = 1;
		XtAppErrorMsg(XtWidgetToApplicationContext(w),
			"childError", "number", "WidgetError",
			"%s widgets can only take one child",
			params, &num_params);
	}

	(*((CompositeWidgetClass)(graphWidgetClass->
		core_class.superclass))->composite_class.insert_child)(w);

	childClass = (GraphDisplayObjectClass)XtClass(w);
	if (childClass->graphDisplay_class.compute_size != NULL)
		(*childClass->graphDisplay_class.compute_size)(parent);
}

static void
Realize(Widget w,
	XtValueMask *valueMask,
	XSetWindowAttributes *attributes)
{
	GraphWidget gw = (GraphWidget) w;
	String params[2];
	Cardinal num_params;

	if (gw->composite.num_children != 1) {
		params[0] = XtClass(w)->core_class.class_name;
		num_params = 1;
		XtAppErrorMsg(XtWidgetToApplicationContext(w),
			"childError", "number", "WidgetError",
			"%s widgets must have exactly one child",
			params, &num_params);
	}

	(*graphWidgetClass->core_class.superclass->core_class.realize)(w,
		valueMask, attributes);
}

static void
Redisplay(Widget w,
	XEvent *event,
	Region region)
{
	GraphWidget gw = (GraphWidget) w;
	GraphDisplayObject d = (GraphDisplayObject) gw->composite.children[0];
	GraphDisplayObjectClass childClass;

	childClass = (GraphDisplayObjectClass)XtClass((Widget)d);
	if (childClass->graphDisplay_class.expose != NULL)
		(*childClass->graphDisplay_class.expose)(w, event, region);
}

static void
Resize(Widget w)
{
	if (XtIsRealized(w)) {
		XClearWindow(XtDisplay(w), XtWindow(w));
		(*(XtClass(w)->core_class.expose))(w, NULL, NULL);
	}
}
