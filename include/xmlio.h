#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include "objects.h"

typedef enum
{
	SPHERE,
	CYLINDER,
	CONE,
	PLANE,
	TRIANGLE,
	POLYGON,
	DISC,
	ELLIPSE,
	/* Unknown object */
	UNKNOWN = -1
} eobject;

void parse_simple(xmlDocPtr doc, xmlNodePtr cur, char *format, void *value);
int parse_int(xmlDocPtr doc, xmlNodePtr cur);
unsigned long parse_ulong(xmlDocPtr doc, xmlNodePtr cur);
tscalar parse_tscalar(xmlDocPtr doc, xmlNodePtr cur);
tvector3d parse_tvector3d(xmlDocPtr doc, xmlNodePtr cur);
tvector4d parse_color(xmlDocPtr doc, xmlNodePtr cur);
twindow parse_twindow(xmlDocPtr doc, xmlNodePtr cur);
tframe parse_tframe(xmlDocPtr doc, xmlNodePtr cur);
tlight* parse_tlight(xmlDocPtr doc, xmlNodePtr cur);
void * parse_properties(xmlDocPtr doc, xmlNodePtr cur, eobject *type);
tobject* parse_tobject(xmlDocPtr doc, xmlNodePtr cur);
tscene parse_tscene(xmlDocPtr doc, xmlNodePtr cur);
int parse_model(const char *docname, tvector3d *origin, twindow *wnd, tframe *fra, tscene *scn);
