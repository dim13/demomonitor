/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Graph.h"
#include "BarDisplay.h"

XtAppContext app;
Widget graph;
int *fields;

void Timer(XtPointer, XtIntervalId *);
void CreateGraphWidget(Widget);
void GetGraphData();
void quit();

typedef struct {
	int timeout;
	int num_fields;
	String command;
} OptionsRec;

OptionsRec options;

#define Offset(field) XtOffsetOf(OptionsRec, field)
XtResource resources[] = {
	{ "interval", "Interval", XtRInt, sizeof(int),
		Offset(timeout), XtRImmediate, (XtPointer) 5 },
	{ "numFields", "NumFields", XtRInt, sizeof(int),
		Offset(num_fields), XtRImmediate, (XtPointer) 3 },
	{ "command", "Command", XtRString, sizeof(String),
		Offset(command), XtRString,
		"vmstat 1 2 | awk '{if (NR == 4) print $(NF-2), $(NF-1), $(NF)}'" },
};
#undef Offset

XrmOptionDescRec optionsDesc[] = {
	{ "-interval", "*interval", XrmoptionSepArg, (XtPointer) NULL },
};

String fallback_resources[] = {
	"*graph.labels:		User\\nSystem\\nIdle",
	"*graph.maxValue:	100",
	NULL
};

XtActionsRec actionsList[] = {
	{ "quit", quit },
};

void
quit()
{
	exit(1);
}

int
main(int argc, char **argv)
{
	Widget toplevel;

	toplevel = XtAppInitialize(&app, "DemoMonitor",
		optionsDesc, XtNumber(optionsDesc), &argc, argv,
		fallback_resources, NULL, 0);
	XtAppAddActions(app, actionsList, XtNumber(actionsList));
	XtGetApplicationResources(toplevel, (XtPointer)&options,
		resources, XtNumber(resources), NULL, 0);
	XtOverrideTranslations(toplevel,
		XtParseTranslationTable("<Key>q: quit()"));
	fields = (int *)XtCalloc(options.num_fields, sizeof(int));

	CreateGraphWidget(toplevel);
	XtRealizeWidget(toplevel);
	XtAppAddTimeOut(app, options.timeout * 1000, Timer, NULL);
	XtAppMainLoop(app);

	return 0;
}

void
CreateGraphWidget(Widget parent)
{
	Arg args[10];
	int n;

	GetGraphData();

	n = 0;
	XtSetArg(args[n], XtNnumEntries, options.num_fields);	n++;
	XtSetArg(args[n], XtNvalues, fields);			n++;
	graph = XtCreateManagedWidget("graph", graphWidgetClass, parent,
		args, n);
	XtCreateWidget("bar", barDisplayObjectClass, graph, NULL, 0);
}

void
Timer(XtPointer client_data, XtIntervalId *id)
{
	Arg args[10];
	int n;

	GetGraphData();

	n = 0;
	XtSetArg(args[n], XtNvalues, fields);			n++;
	XtSetValues(graph, args, n);

	XtAppAddTimeOut(app, options.timeout * 1000, Timer, NULL);
}

void
GetGraphData()
{
	int status;
	FILE *f;
	int i;

	f = popen(options.command, "r");

	for (i = 0; i < options.num_fields; i++) {
		status = fscanf(f, "%d", &fields[i]);
		if (status != 1) {
			XtAppWarningMsg(app, "noData", "getGraphData",
				"DemoLoadError", "Not enough fields in data",
				NULL, NULL);
			for (; i < options.num_fields; i++)
				fields[i] = 0;
			break;
		}
	}

	pclose(f);
}
