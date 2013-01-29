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
	return (tlight) { anchor, color, intensity, c1, c2, c3 };
}

// tcutplane functions
tcutplane * tcutplane_new()
{
	tcutplane *cplane = (tcutplane *) malloc(sizeof(tcutplane));
	
	if (cplane) *cplane = tcutplane_init(NULL, 0);
	
	return cplane;
}

tcutplane tcutplane_init (tplane *plane, int side)
{
	return (tcutplane) { plane, side };
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
	return (tobject) { properties, color, env_k, difuse_k, specular_k, specular_n, tlist_init(free, NULL, NONREVERSE), intersections, normal, free_properties };
}

void tobject_destroy (void *obj)
{
	tobject *o = (tobject*) obj;
	(o->free_properties)(o->properties);
	tlist_clear(&o->planes);
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
		*cyl = tcylinder_init(DEFAULT_POINT, DEFAULT_DIRECTION, DEFAULT_OBJECT_RADIUS, -INFINITY, INFINITY);
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

void tcylinder_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tcylinder *cyl = (tcylinder *) properties;
	tscalar dist_to_origin;
	tvector3d to_origin = v_normalize(v_sub(origin,cyl->anchor), &dist_to_origin);
	tscalar ID = v_dot_product(direction, cyl->direction);
	tscalar IX = v_dot_product(direction, to_origin);
	tscalar DX = v_dot_product(cyl->direction, to_origin);
	tscalar A, b, C, d, D, sqrt_d, kDX;
	int s;
	long i;
	
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
		
		*count = 2;
	}
	else if (s == 0)
	{	
		distances[0] = -b / A;
		*count = 1;
	}
	else
	{
		*count = 0;
		return;
	}
	
	kDX = dist_to_origin * DX;	
	
	d = kDX + distances[0] * ID;
	
	if (is_contained(cyl->h1, cyl->h2, d))
	{
		if (*count == 2)
		{
			d = kDX + distances[1] * ID;
			if (!is_contained(cyl->h1, cyl->h2, d)) (*count)--;
		}
	}
	else
	{
		(*count)--;
		
		if (*count == 1)
		{			
			d = kDX + distances[1] * ID;
			if (!is_contained(cyl->h1, cyl->h2, d))
			{
				(*count)--;
			}
			else
			{
				distances[0] = distances[1];
			}
		}
	}
}

tvector3d tcylinder_normal (void* properties, tvector3d point)
{
	tcylinder *cyl = (tcylinder *) properties;
	tscalar dist_from_anchor = v_dot_product(v_sub(point, cyl->anchor), cyl->direction);
    tvector3d point_projection = v_point_at(cyl->anchor, cyl->direction, dist_from_anchor);
    return v_scale(v_sub(point, point_projection), 1.0 / cyl->radius);
}

// cone functions
tcone * tcone_new()
{
	tcone *cn = (tcone *) malloc(sizeof(tcone));
	
	if (cn)
	{
		*cn = tcone_init(DEFAULT_POINT, DEFAULT_DIRECTION, DEFAULT_OBJECT_RATIO, -INFINITY, INFINITY);
	}
	
	return cn;
}

void tcone_with_coeficients(tscalar ratio2, tscalar ID, tscalar IX, tscalar DX, tscalar k, tscalar *A, tscalar *b, tscalar *C)
{
	ratio2 += 1.0;
	
	*A = 1.0 - ratio2 * (ID * ID);
	*b = k * (IX - ratio2 * (DX * ID));
	*C = (k * k) * (1.0 - ratio2 * (DX * DX));
}

tcone tcone_init(tvector3d anchor, tvector3d direction, tscalar ratio, tscalar h1, tscalar h2)
{	
	if (!isnanl(h1) && !isnanl(h2) && h2 < h1)
	{
		tscalar aux = h1;
		h1 = h2;
		h2 = aux;
	}
	
	return (tcone) { anchor, direction, ratio, ratio * ratio, h1, h2, tcone_with_coeficients };
}

void tcone_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tcylinder_intersections(properties, origin, direction, count, distances);
}

tvector3d tcone_normal (void* properties, tvector3d point)
{
	tcone *cn = (tcone *) properties;
	tscalar dist_from_anchor = v_dot_product(v_sub(point, cn->anchor), cn->direction);
    tvector3d point_projection = v_point_at(cn->anchor, cn->direction, dist_from_anchor);
    return v_normalize(v_sub(point, point_projection), NULL);
}

// scene functions
tplane * tplane_new()
{
	tplane *pla = (tplane *) malloc(sizeof(tplane));
	
	if (pla) *pla = tplane_init(DEFAULT_POINT, DEFAULT_DIRECTION);
	
	return pla;
}

tplane tplane_init (tvector3d anchor, tvector3d direction)
{
	tvector3d normalized = v_normalize(direction, NULL);
	tscalar d = -v_dot_product(anchor, normalized);
	return (tplane) { normalized.x, normalized.y, normalized.z, d};
}

int tplane_point_side (tplane pla, tvector3d point)
{
	return SIGN(v_dot_product(*((tvector3d *) &pla), point) + pla.w);
}

void tplane_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	static long c = 0;
	
	tplane *pla = (tplane *) properties;
	tscalar ND = v_dot_product(direction, *((tvector3d *) properties));
	
	if (ZERO(ND))
	{
		*count = 0;
	}
	else
	{
		distances[0] = -(pla->w + v_dot_product(origin, *((tvector3d *) properties))) / ND;
		*count = 1;
	}
}

tvector3d tplane_normal (void* properties, tvector3d point)
{
	return *((tvector3d *) properties);
}

// triangle functions
ttriangle * ttriangle_new()
{
	ttriangle *tri = (ttriangle *) malloc(sizeof(ttriangle));
	
	if (tri) *tri = (ttriangle) { (tplane) { 0.0, 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 },
								  (tvector3d) { 0.0, 0.0, 0.0 } };
	
	return tri;
}

ttriangle ttriangle_init (tvector3d a, tvector3d b, tvector3d c, einside_method method)
{
	ttriangle tri;
	
	tri.a = a;
	tri.b = b;
	tri.c = c;
	tri.ab = v_sub(b, a);
	tri.ac = v_sub(c, a);
	
	tri.plane = tplane_init(a, v_cross_product(tri.ab, tri.ac));
	
	if (method == IM_SAME_SIDE)
	{
		tri.bc = v_sub(c, b);
		tri.method = ttriangle_inside_same_side_method;
	}
	else
	{
		tri.inv_denom = 1.0 / (tri.ac.x * tri.ab.y - tri.ac.y * tri.ab.x);
		tri.method = ttriangle_inside_barycentric_coords_method;
	}	
	
	return tri;
}

int ttriangle_inside_same_side_method(void *properties, tvector3d p)
{
	ttriangle *tri = (ttriangle *) properties;
	return v_same_side(p, tri->a, tri->b, tri->bc) &&
		   v_same_side(p, tri->b, tri->a, tri->ac) &&
		   v_same_side(p, tri->c, tri->a, tri->ab);
}

int ttriangle_inside_barycentric_coords_method(void *properties, tvector3d p)
{
	ttriangle *tri = (ttriangle *) properties;
	tvector3d ap = v_sub(p, tri->a);
	tscalar u = (tri->ac.x * ap.y - tri->ac.y * ap.x) * tri->inv_denom;
	tscalar v = (ap.x * tri->ab.y - ap.y * tri->ab.x) * tri->inv_denom;
	tscalar diff = u + v - 1.0;
	
	return NONNEGATIVE(u) && NONNEGATIVE(v) && NONPOSITIVE(diff);
}

void ttriangle_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	ttriangle *tri = (ttriangle *) properties;
	
	tplane_intersections(&(tri->plane), origin, direction, count, distances);

	if (*count > 0)
	{
		if (NONPOSITIVE(distances[0]))
		{
			*count = 0;
			return; 
		}
		else
		{
			/* The intersection point. */
			tvector3d p;
			
			p = v_point_at(origin, direction, distances[0]);
						
			/*
			 * Using the "same side" method to determine wheter or not the point is inside the triangle.
			 * */
			*count = (tri->method (properties, p)) ? 1 : 0;
		}
	}
}
tvector3d ttriangle_normal (void* properties, tvector3d point)
{
	return *((tvector3d *) &((ttriangle *) properties)->plane);
}

// polygon functions
tpolygon * tpolygon_new()
{
	tpolygon *pol = (tpolygon *) malloc(sizeof(tpolygon));
	
	if (pol) *pol = (tpolygon) { (tplane) { 0.0, 0.0, 0.0, 0.0 }, NULL, 0, -1, -1 };
	
	return pol;
}

tpolygon tpolygon_init (tvector3d *points, long num_points)
{
	assert((points));
	assert(num_points > 2);
	
	tvector3d direction = v_cross_product(v_sub(points[1], points[0]), v_sub(points[2], points[0]));
	tvector2d *points2d = (tvector2d *) malloc(num_points * sizeof(tvector2d));
	tplane pla = tplane_init(points[0], direction);
	int u, v, excluded;
	
	excluded = (+pla.x > +pla.y) ? ((+pla.x > +pla.z) ? X : Z) : ((+pla.y > +pla.z) ? Y : Z);
	u = (excluded == X) ? Y : X;
	v = (excluded == Z) ? Y : Z;
	
	assert(u != v);
	
	if (points2d)
	{
		long i;		
		
		for (i = 0; i < num_points; i++)
		{
			points2d[i].x = (u == X) ? points[i].x : points[i].y;
			points2d[i].y = (v == Y) ? points[i].y : points[i].z;
		}		
	}
	
	return (tpolygon) { pla, points2d, num_points, u, v };
}

void tpolygon_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tpolygon *pol = (tpolygon *) properties;
	
	tplane_intersections(&(pol->plane), origin, direction, count, distances);

	if (*count > 0)
	{
		/*
		 * In order to improve performance, positive distance checking is done here,
		 * before several calculations are performed.
		 * */
		if (NONPOSITIVE(distances[0]))
		{
			*count = 0;
			return; 
		}
		else
		{
			/* The intersection point. */
			tvector3d p3d;
			/* The intersection point, in dimensions (U, V). */
			tvector2d p2d;
			/* The polygon's points, translated by -p2d. */
			tvector2d translated[pol->num_points];
			/*
			 * i: index of the actual point being analized.
			 * j: index of the actual next point being analized.
			 * k: counter of intersections with edges from origin
			 * */
			long i, j, k;
			/*
			 * sh: sign of actual point's Y axis coordinate.
			 * nsh: sign of actual next point's Y axis coordinate.
			 * shx: sign of actual point's X axis coordinate.
			 * nshx: sign of actual next point's X axis coordinate.
			 * */
			int sh,nsh;
			int shx,nshx;
			
			p3d = v_point_at(origin, direction, distances[0]);
			p2d.x = (pol->u_axis == X) ? p3d.x : p3d.y;
			p2d.y = (pol->v_axis == Y) ? p3d.y : p3d.z;
			
			for (i = 0; i < pol->num_points; i++)
			{
				translated[i].x = pol->points[i].x - p2d.x;
				translated[i].y = pol->points[i].y - p2d.y;
			}
			
			k = 0;
			/* Starting with the last point with no additional changes
			 * code produces 'negative' of the polygon, limited by its
			 * minimum and maximum Y coordinate values, and its
			 * maximum X coordinate value.*/
			sh = SIGN(translated[0].y);			
			for (i = 0, j = 1; i < pol->num_points; i++, j++)
			{
				/* The end of the array has been reached, so the next point should be the first one. */
				if (j == pol->num_points) j = 0;
				
				nsh = SIGN(translated[j].y);

				if (sh != nsh)
				{
					shx = SIGN(translated[i].x);
					nshx = SIGN(translated[j].x);
					
					// If both points are located in positive X coordinates.
					if (shx == 1 && nshx == 1)
					{
						k++;
					}
					else if (shx == 1 || nshx == 1)
					{
						if (POSITIVE(translated[i].x -
									 translated[i].y * (translated[j].x - translated[i].x) /
													   (translated[j].y - translated[i].y)))
						{
							k++;
						}
					}
				}
				sh = nsh;
			}
			
			*count = (k & 1) ? 1 : 0;
		}
	}
}

tvector3d tpolygon_normal (void* properties, tvector3d point)
{
	return tplane_normal(&(((tpolygon *) properties)->plane), point);
}

void tpolygon_destroy (void* p)
{	
	free(((tpolygon *) p)->points);
	free(p);
}

// disc functions
tdisc * tdisc_new()
{
	tdisc *dsc = (tdisc *) malloc(sizeof(tdisc));
	
	if (dsc)
	{
		*dsc = tdisc_init(DEFAULT_POINT, DEFAULT_DIRECTION, 0.0, DEFAULT_OBJECT_RADIUS);
	}
	
	return dsc;
}

tdisc tdisc_init (tvector3d anchor, tvector3d direction, tscalar rad1, tscalar rad2)
{
	if (+rad1 > +rad2)
	{
		tscalar aux = rad1;
		rad1 = rad2;
		rad2 = aux;
	}
	
	return (tdisc) { anchor, tplane_init(anchor, direction), +rad1, +rad2, rad1 * rad1, rad2 * rad2 };
}

void tdisc_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tdisc *dsc = (tdisc *) properties;
	
	tplane_intersections(&(dsc->plane), origin, direction, count, distances);

	if (*count > 0)
	{
		if (NONPOSITIVE(distances[0]))
		{
			*count = 0;
			return; 
		}
		else
		{
			/* The intersection point. */
			tvector3d p;
			/* Vector whose origin is the disc's anchor and whose end is 'p'. */
			tvector3d ap;
			/* The square of the distance from disc's anchor to point 'p' */
			tscalar sqrdist;
			
			p = v_point_at(origin, direction, distances[0]);
			ap = v_sub(p, dsc->anchor);
			sqrdist = v_dot_product(ap, ap);
						
			*count = (dsc->sqrrad1 <= sqrdist && sqrdist <= dsc->sqrrad2) ? 1 : 0;
		}
	}
}

tvector3d tdisc_normal (void* properties, tvector3d point)
{
	return *((tvector3d *) &((tdisc *) properties)->plane);
}

// ellipse functions
tellipse * tellipse_new()
{
	tellipse *elp = (tellipse *) malloc(sizeof(tellipse));
	
	if (elp)
	{
		*elp = tellipse_init(DEFAULT_POINT, DEFAULT_POINT, DEFAULT_DIRECTION, 0.0, DEFAULT_OBJECT_RADIUS);
	}
	
	return elp;
}

tellipse tellipse_init (tvector3d focus1, tvector3d focus2, tvector3d direction, tscalar k1, tscalar k2)
{
	if (+k1 > +k2)
	{
		tscalar aux = k1;
		k1 = k2;
		k2 = aux;
	}
	// TODO: Adjust focus2 to ensure it lies in the same plane as focus1 with the given normal ('direction')
	return (tellipse) { focus1, focus2, tplane_init(focus1, direction), +k1, +k2, k1 * k1, k2 * k2 };
}

void tellipse_intersections (void* properties, tvector3d origin, tvector3d direction, long *count, tscalar *distances)
{
	tellipse *elp = (tellipse *) properties;
	
	tplane_intersections(&(elp->plane), origin, direction, count, distances);

	if (*count > 0)
	{
		if (NONPOSITIVE(distances[0]))
		{
			*count = 0;
			return; 
		}
		else
		{
			/* The intersection point. */
			tvector3d p;
			/* Vector whose origin is the ellipse's focus1 and whose end is 'p'. */
			tvector3d f1p;
			/* Vector whose origin is the ellipse's focus2 and whose end is 'p'. */
			tvector3d f2p;
			/* The sum of the square of the distances from ellipse's focuses to point 'p' */
			tscalar dist;
			
			p = v_point_at(origin, direction, distances[0]);
			f1p = v_sub(p, elp->focus1);
			f2p = v_sub(p, elp->focus2);
			dist = v_norm(f1p) +  v_norm(f2p);
						
			*count = (elp->k1 <= dist && dist <= elp->k2) ? 1 : 0;
		}
	}
}

tvector3d tellipse_normal (void* properties, tvector3d point)
{
	return *((tvector3d *) &((tellipse *) properties)->plane);
}

// scene functions
tscene * tscene_new()
{
	tscene *scn = (tscene *) malloc(sizeof(tscene));
	
	if (scn) *scn = (tscene) tscene_init(DEFAULT_SCENE_BKCOLOR, DEFAULT_SCENE_ENV_INTENSITY);
	
	return scn;
}

tscene tscene_init(tvector4d bkcolor, tscalar env_intensity)
{
	return (tscene) { bkcolor, env_intensity, tlist_init(tobject_destroy, NULL, NONREVERSE), tlist_init(free, NULL, NONREVERSE), tlist_init(free, NULL, NONREVERSE) };
}

void tscene_clear (tscene *scn)
{
	scn->bkcolor = DEFAULT_SCENE_BKCOLOR;
	scn->env_intensity = DEFAULT_SCENE_ENV_INTENSITY;
	tlist_clear(&(scn->objects));
	tlist_clear(&(scn->lights));
	tlist_clear(&(scn->planes));
}

void tscene_destroy (void * p)
{
	tscene_clear((tscene *) p);
	free(p);
}
