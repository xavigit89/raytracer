#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <stdlib.h>
#include "vector.h"
#include "list.h"

#define COLOR_BYTES 4

typedef void (*intersect_fun) (void *, tvector3d, tvector3d, long *, tscalar *);
typedef tvector3d (*normal_fun) (void *, tvector3d);

typedef struct {
	unsigned long width,height;
	unsigned char * bytes;
} tframe;

typedef struct {
	tvector3d origin;
	tscalar width, height;
} twindow;

typedef struct {
	tvector3d anchor;
	tvector4d color;
	tscalar intensity, c1, c2, c3;
} tlight;

typedef struct stobject {
	void* properties;
	tvector4d color;
	tscalar env_k;
	tscalar difuse_k;
	tscalar specular_k;	
	tscalar specular_n;
	intersect_fun intersections;
	normal_fun normal;
	memfree_fun free_properties;
} tobject;

typedef struct {
	tscalar distance;
	tobject * object;
} tintersection;

typedef struct {
	tvector3d anchor;
	tscalar radius;
	tscalar radius2;
} tsphere;

typedef struct {
	tvector4d bkcolor;
	tscalar env_intensity;
	tlist objects;
	tlist lights;
} tscene;

// tlight functions
tlight * tlight_new();
tlight tlight_init (tvector3d anchor, tvector4d color, tscalar intensity, tscalar c1, tscalar c2, tscalar c3);

// tobject functions
tobject * tobject_new();
tobject tobject_init (tvector4d color, tscalar env_k, tscalar difuse_k, tscalar specular_k, tscalar specular_n,
					  intersect_fun intersections, normal_fun normal, memfree_fun free_properties);
void tobject_destroy (void *obj);
void tobject_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tobject_normal (void* properties, tvector3d point);

// tintersection functions
tintersection * tintersection_new (tscalar distance, tobject *object);
int tintersection_compare (void *a, void *b);

// sphere functions
tsphere * tsphere_new();
tsphere tsphere_init(tvector3d anchor, tscalar radius);
void tsphere_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tsphere_normal (void* properties, tvector3d point);

// cylinder functions
tsphere * tsphere_new();
tsphere tsphere_init(tvector3d anchor, tscalar radius);
void tsphere_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tsphere_normal (void* properties, tvector3d point);

// scene functions
void tscene_init(tscene *scn, tvector4d bkcolor, tscalar env_intensity);

#endif
