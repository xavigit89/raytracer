#include <assert.h>
#include "objects.h"

tlight * tlight_new() {
	return (tlight *) malloc(sizeof(tlight));
}

tlight tlight_init (tvector3d anchor, tvector4d color, tscalar intensity, tscalar c1, tscalar c2, tscalar c3) {
	return (tlight) { anchor, color, intensity, c1, c2, c3 };
}

tobject * tobject_new() {
	return (tobject *) malloc(sizeof(tobject));
}

tobject tobject_init (tvector4d color, tscalar env_k, tscalar difuse_k, tscalar specular_k, tscalar specular_n,
					  intersect_fun intersections, normal_fun normal, memfree_fun free_properties) {
	return (tobject) { NULL, color, env_k, difuse_k, specular_k, specular_n, intersections, normal, free_properties };
}

void tobject_destroy (void *obj) {
	tobject *o = (tobject*) obj;
	(o->free_properties)(o->properties);
	free(obj);
}

void tobject_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances) {
	*count = 0;
}

tvector3d tobject_normal (void* properties, tvector3d point) {
	return (tvector3d) { 0.0, 0.0, 0.0 };
}

tintersection * tintersection_new(tscalar distance, tobject *object) {
	tintersection * intersection = (tintersection *) malloc(sizeof(tintersection));
	
	if (intersection) {
		intersection->distance = distance;
		intersection->object = object;
	}
	
	return intersection;
}

int tintersection_compare (void *a, void *b) {
	tscalar diff = ((tintersection *) a)->distance - ((tintersection *) b)->distance;
	
	return SIGN(diff);
}

// Spheres
tsphere * tsphere_new() {
	return (tsphere *) malloc(sizeof(tsphere));
}

tsphere tsphere_init(tvector3d anchor, tscalar radius) {
	return (tsphere) { anchor, radius, radius * radius };
}

void tsphere_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances) {		
	tsphere *sph = (tsphere *) properties;
	tvector3d from_anchor = v_sub(origin, sph->anchor);
	tscalar b = v_dot_product(direction, from_anchor);
	tscalar C = v_dot_product(from_anchor, from_anchor) - sph->radius2;
	tscalar d = b * b - C;
	tscalar aux = 4.0 * d;	
	
	if (ZERO(aux)) {
		*count = 1;
		distances[0] = -b;
	}
	else if (d > 0.0) {
		*count = 2;
		aux = sqrtl(d);
		
		distances[0] = -b - aux;
		distances[1] = -b + aux;
	}
	else {
		*count = 0;
	}
}

tvector3d tsphere_normal (void* properties, tvector3d point) {
	return v_scale(v_sub(point, ((tsphere *) properties)->anchor), 1.0 / ((tsphere *) properties)->radius);
}

void tscene_init(tscene *scn, tvector4d bkcolor, tscalar env_intensity) {
	scn->bkcolor = bkcolor;
	scn->env_intensity = env_intensity;
	scn->objects = tlist_init(tobject_destroy, NULL, NONREVERSE);
	scn->lights = tlist_init(free, NULL, NONREVERSE);
}
