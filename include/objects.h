#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
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
#define DEFAULT_OBJECT_RATIO 1.0

typedef enum {
	IM_BARYCENTRIC_COORDS,
	IM_SAME_SIDE,	
	IM_UNKNOWN = -1
} einside_method;

typedef void (*intersect_fun) (void *, tvector3d, tvector3d, long *, tscalar *);
typedef tvector3d (*normal_fun) (void *, tvector3d);
typedef int (*inside_method_f) (void *, tvector3d);

typedef struct {
	long width,height;
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

typedef tcylinder tcone;

typedef tvector4d tplane;

typedef struct {
	tplane plane;
	tvector3d a, b, c;
	tvector3d ab;
	tvector3d ac;
	tvector3d bc;
	tscalar inv_denom;
	inside_method_f method;
} ttriangle;

typedef struct {
	tplane plane;
	tvector2d *points;
	long num_points;
	int u_axis;
	int v_axis;
} tpolygon;

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

// cone functions
tcone * tcone_new();
tcone tcone_init (tvector3d anchor, tvector3d direction, tscalar ratio, tscalar h1, tscalar h2);
void tcone_with_coeficients (tscalar ratio2, tscalar ID, tscalar IX, tscalar DX, tscalar k, tscalar *A, tscalar *b, tscalar *C);
void tcone_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tcone_normal (void* properties, tvector3d point);

// plane functions
tplane * tplane_new();
tplane tplane_init (tvector3d anchor, tvector3d direction);
void tplane_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tplane_normal (void* properties, tvector3d point);

// triangle functions
ttriangle * ttriangle_new();
ttriangle ttriangle_init (tvector3d a, tvector3d b, tvector3d c, einside_method method);
int ttriangle_inside_same_side_method(void *properties, tvector3d p);
int ttriangle_inside_barycentric_coords_method(void *properties, tvector3d p);
void ttriangle_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d ttriangle_normal (void* properties, tvector3d point);

// polygon functions
tpolygon * tpolygon_new();
tpolygon tpolygon_init (tvector3d *points, long num_points);
void tpolygon_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances);
tvector3d tpolygon_normal (void* properties, tvector3d point);
void tpolygon_destroy (void* p);

// scene functions
tscene * tscene_new ();
tscene tscene_init (tvector4d bkcolor, tscalar env_intensity);
void tscene_clear (tscene *scn);
void tscene_destroy (void * p);

#endif
