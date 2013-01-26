#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "list.h"

#define COLOR_BYTES 4
#define DEFAULT_POINT (tvector3d) { 0.0, 0.0, 0.0 }
#define DEFAULT_DIRECTION (tvector3d) { 0.0, 1.0, 0.0 }
#define DEFAULT_SCENE_BKCOLOR (tvector4d) { 0.5, 0.5, 0.5, 1.0 }
#define DEFAULT_SCENE_ENV_INTENSITY 1.0
#define DEFAULT_LIGHT_COLOR (tvector4d) { 1.0, 1.0, 1.0, 1.0 }
#define DEFAULT_LIGHT_INTENSITY 1.0
#define DEFAULT_OBJECT_COLOR (tvector4d) { 0.0, 1.0, 0.0, 1.0 }
#define DEFAULT_OBJECT_ENV_K 1.0
#define DEFAULT_OBJECT_DIFUSE_K 1.0
#define DEFAULT_OBJECT_SPECULAR_K 0.8
#define DEFAULT_OBJECT_SPECULAR_N 4
#define DEFAULT_OBJECT_RADIUS 100.0


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
	tvector3d anchor;
	tvector3d direction;
	tscalar radius;
	tscalar radius2;
	tscalar h1;
	tscalar h2;
	void (*coeficientsf)(tscalar, tscalar, tscalar, tscalar, tscalar, tscalar *, tscalar *, tscalar *);
} tcylinder;

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
tobject tobject_init (void* properties, tvector4d color,
					  tscalar env_k, tscalar difuse_k, tscalar specular_k, tscalar specular_n,
					  intersect_fun intersections, normal_fun normal, memfree_fun free_properties);
void tobject_destroy (void *obj);
void tobject_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tobject_normal (void* properties, tvector3d point);

// tintersection functions
tintersection * tintersection_new(tscalar distance, tobject *object);
tintersection tintersection_init (tscalar distance, tobject *object);
int tintersection_compare (void *a, void *b);

// sphere functions
tsphere * tsphere_new ();
tsphere tsphere_init (tvector3d anchor, tscalar radius);
void tsphere_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tsphere_normal (void* properties, tvector3d point);

// cylinder functions
tcylinder * tcylinder_new();
tcylinder tcylinder_init (tvector3d anchor, tvector3d direction, tscalar radius, tscalar h1, tscalar h2);
void tcylinder_with_coeficients (tscalar radius2, tscalar ID, tscalar IX, tscalar DX, tscalar k, tscalar *A, tscalar *b, tscalar *C);
void tcylinder_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tcylinder_normal (void* properties, tvector3d point);

// scene functions
tscene * tscene_new ();
tscene tscene_init (tvector4d bkcolor, tscalar env_intensity);
void tscene_clear (tscene *scn);
void tscene_destroy (void * p);

#endif
