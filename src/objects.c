#include <assert.h>
#include "objects.h"

tlight * tlight_new()
{
	
	tlight * light = (tlight *) malloc(sizeof(tlight));
	
	if (light)
	{
		*light = tlight_init(DEFAULT_POINT, DEFAULT_LIGHT_COLOR, DEFAULT_LIGHT_INTENSITY, 1.0, 0.0, 0.0);
	}
	
	return light;
}

tlight tlight_init (tvector3d anchor, tvector4d color, tscalar intensity, tscalar c1, tscalar c2, tscalar c3)
{
	return (tlight)
{ anchor, color, intensity, c1, c2, c3 };
}

tobject * tobject_new()
{
	tobject *obj  = (tobject *) malloc(sizeof(tobject));
	
	if (obj)
	{
		*obj = tobject_init(NULL, DEFAULT_OBJECT_COLOR,
							DEFAULT_OBJECT_ENV_K, DEFAULT_OBJECT_DIFUSE_K, DEFAULT_OBJECT_SPECULAR_K, DEFAULT_OBJECT_SPECULAR_N,
							tobject_intersections, tobject_normal, free);
	}
	
	return obj;
}

tobject tobject_init (void* properties, tvector4d color, tscalar env_k, tscalar difuse_k, tscalar specular_k, tscalar specular_n,
					  intersect_fun intersections, normal_fun normal, memfree_fun free_properties)
{
	return (tobject) { properties, color, env_k, difuse_k, specular_k, specular_n, intersections, normal, free_properties };
}

void tobject_destroy (void *obj)
{
	tobject *o = (tobject*) obj;
	(o->free_properties)(o->properties);
	free(obj);
}

void tobject_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	*count = 0;
}

tvector3d tobject_normal (void* properties, tvector3d point)
{
	return DEFAULT_POINT;
}

tintersection * tintersection_new(tscalar distance, tobject *object)
{
	tintersection * intersection = (tintersection *) malloc(sizeof(tintersection));
	
	if (intersection)
	{
		*intersection = tintersection_init(distance, object);
	}
	
	return intersection;
}

tintersection tintersection_init(tscalar distance, tobject *object)
{
	return (tintersection) { distance, object };
}

int tintersection_compare (void *a, void *b)
{
	tscalar diff = ((tintersection *) a)->distance - ((tintersection *) b)->distance;
	
	return SIGN(diff);
}

// Spheres
tsphere * tsphere_new()
{
	tsphere *sph = (tsphere *) malloc(sizeof(tsphere));
	
	if (sph)
	{
		*sph = tsphere_init(DEFAULT_POINT, DEFAULT_OBJECT_RADIUS);
	}
	
	return sph;
}

tsphere tsphere_init (tvector3d anchor, tscalar radius)
{
	return (tsphere) { anchor, radius, radius * radius };
}

void tsphere_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{		
	tsphere *sph = (tsphere *) properties;
	tvector3d from_anchor = v_sub(origin, sph->anchor);
	tscalar b = v_dot_product(direction, from_anchor);
	tscalar C = v_dot_product(from_anchor, from_anchor) - sph->radius2;
	tscalar d = b * b - C;
	tscalar aux = 4.0 * d;	
	
	if (ZERO(aux))
	{
		*count = 1;
		distances[0] = -b;
	}
	else if (d > 0.0)
	{
		*count = 2;
		aux = sqrtl(d);
		
		distances[0] = -b - aux;
		distances[1] = -b + aux;
	}
	else
	{
		*count = 0;
	}
}

tvector3d tsphere_normal (void* properties, tvector3d point)
{
	return v_scale(v_sub(point, ((tsphere *) properties)->anchor), 1.0 / ((tsphere *) properties)->radius);
}

// cylinder functions
tcylinder * tcylinder_new()
{
	tcylinder *cyl = (tcylinder *) malloc(sizeof(tcylinder));
	
	if (cyl)
	{
		*cyl = tcylinder_init(DEFAULT_POINT, DEFAULT_DIRECTION, DEFAULT_OBJECT_RADIUS, FP_NAN, FP_NAN);
	}
	
	return cyl;
}

void tcylinder_with_coeficients(tscalar radius2, tscalar ID, tscalar IX, tscalar DX, tscalar k, tscalar *A, tscalar *b, tscalar *C)
{
	*A = 1.0 - (ID * ID);
	*b = k * (IX - DX * ID);
	*C = (k * k) * (1 - (DX * DX)) - radius2;
}

tcylinder tcylinder_init(tvector3d anchor, tvector3d direction, tscalar radius, tscalar h1, tscalar h2)
{	
	if (!isnanl(h1) && !isnanl(h2) && h2 < h1)
	{
		tscalar aux = h1;
		h1 = h2;
		h2 = aux;
	}
	
	return (tcylinder) { anchor, direction, radius, radius * radius, h1, h2, tcylinder_with_coeficients };
}

int is_contained(tscalar lower, tscalar upper, tscalar value)
{
	return ((isnanl(lower) || lower <= value) && (isnanl(upper) || value <= upper)) ? 1 : 0;	
}

void tcylinder_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tcylinder *cyl = (tcylinder *) properties;
	tscalar dist_to_origin;
	tvector3d to_origin = v_normalize(v_sub(origin,cyl->anchor), &dist_to_origin);
	tscalar ID = v_dot_product(direction, cyl->direction);
	tscalar IX = v_dot_product(direction, to_origin);
	tscalar DX = v_dot_product(cyl->direction, to_origin);
	tscalar A, b, C, d, D, sqrt_d;
	int s;
	
	cyl->coeficientsf(cyl->radius2, ID, IX, DX, dist_to_origin, &A, &b, &C);

	d = b * b - A * C;
	D = 4.0 * d;

	s = SIGN(D);
	
	*count = 0;
	
	if (s == 1)
	{
		sqrt_d = sqrtl(d);
		distances[0] = (-b - sqrt_d) / A;
		distances[1] = (-b + sqrt_d) / A;
		if (is_contained(cyl->h1, cyl->h2, distances[0])) (*count)++;
		if (is_contained(cyl->h1, cyl->h2, distances[1])) (*count)++;
	}
	else if (s == 0)
		distances[0] = -b / A;
		if (is_contained(cyl->h1, cyl->h2, distances[0])) (*count)++;
	else
		*count = 0;
}

tvector3d tcylinder_normal (void* properties, tvector3d point)
{
	tcylinder *cyl = (tcylinder *) properties;
	tscalar dist_from_anchor = v_dot_product(v_sub(point, cyl->anchor), cyl->direction);
    tvector3d point_projection = v_point_at(cyl->anchor, cyl->direction, dist_from_anchor);
    return v_scale(v_sub(point, point_projection), 1.0 / cyl->radius);
}

tscene * tscene_new()
{
	tscene *scn = (tscene *) malloc(sizeof(tscene));
	
	if (scn)
	{
		*scn = (tscene) tscene_init(DEFAULT_SCENE_BKCOLOR, DEFAULT_SCENE_ENV_INTENSITY);
	}
	
	return scn;
}

tscene tscene_init(tvector4d bkcolor, tscalar env_intensity)
{
	return (tscene) { bkcolor, env_intensity, tlist_init(tobject_destroy, NULL, NONREVERSE), tlist_init(free, NULL, NONREVERSE) };
}

void tscene_clear (tscene *scn)
{
	scn->bkcolor = DEFAULT_SCENE_BKCOLOR;
	scn->env_intensity = DEFAULT_SCENE_ENV_INTENSITY;
	tlist_clear(&(scn->objects));
	tlist_clear(&(scn->lights));
}

void tscene_destroy (void * p)
{
	tscene_clear((tscene *) p);
	free(p);
}
