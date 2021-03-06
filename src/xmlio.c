#include "xmlio.h"

void parse_simple(xmlDocPtr doc, xmlNodePtr cur, char *format, void *value)
{
	xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	sscanf(key, format, value);
	xmlFree(key);
}

int parse_int(xmlDocPtr doc, xmlNodePtr cur)
{
	
	int value;
	
	xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	sscanf(key, "%d", &value);
	xmlFree(key);
	
	return value;
}

unsigned long parse_ulong(xmlDocPtr doc, xmlNodePtr cur)
{
	
	unsigned long value;
	
	xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	sscanf(key, "%lu", &value);
	xmlFree(key);
	
	return value;
}

tscalar parse_tscalar(xmlDocPtr doc, xmlNodePtr cur)
{
	
	tscalar value;
	
	xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	sscanf(key, "%LF", &value);
	xmlFree(key);
	
	return value;
}

tvector3d parse_tvector3d(xmlDocPtr doc, xmlNodePtr cur)
{
	
	tscalar x = 0.0;
	tscalar y = 0.0;
	tscalar z = 0.0;
	
	cur = cur->xmlChildrenNode;
	while (cur)
	{
		if (!xmlStrcmp(cur->name,"X"))
		{
			x = parse_tscalar(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"Y"))
		{
			y = parse_tscalar(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"Z"))
		{
			z = parse_tscalar(doc, cur);
		}

		cur = cur->next;
	}

	return (tvector3d) { x, y , z };
}

tvector4d parse_color(xmlDocPtr doc, xmlNodePtr cur)
{
	tscalar r = 1.0;
	tscalar g = 1.0;
	tscalar b = 1.0;
	tscalar a = 1.0;
	
	cur = cur->xmlChildrenNode;
	
	while (cur)
	{
		if (!xmlStrcmp(cur->name,"R"))
		{
			r = parse_int(doc, cur) / 255.0;
		}
		else if (!xmlStrcmp(cur->name,"G"))
		{
			g = parse_int(doc, cur) / 255.0;
		}
		else if (!xmlStrcmp(cur->name,"B"))
		{
			b = parse_int(doc, cur) / 255.0;
		}
		else if (!xmlStrcmp(cur->name,"A"))
		{
			a = parse_int(doc, cur) / 255.0;
		}
		
		cur = cur->next;
	}
	
	return (tvector4d) { r, g, b, a };
}

twindow parse_twindow(xmlDocPtr doc, xmlNodePtr cur)
{
	
	twindow wnd;
	
	cur = cur->xmlChildrenNode;
	
	while (cur)
	{
		if (!xmlStrcmp(cur->name,"origin"))
		{
			wnd.origin = parse_tvector3d(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"width"))
		{
			parse_simple(doc, cur, "%LF", &wnd.width);
		}
		else if (!xmlStrcmp(cur->name,"height"))
		{
			parse_simple(doc, cur, "%LF", &wnd.height);
		}
		
		cur = cur->next;
	}
	
	return wnd;
}

tframe parse_tframe(xmlDocPtr doc, xmlNodePtr cur)
{
	tframe fra;
	
	cur = cur->xmlChildrenNode;
	
	while (cur)
	{
		if (!xmlStrcmp(cur->name,"width"))
		{
			fra.width = parse_ulong(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"height"))
		{
			fra.height = parse_ulong(doc, cur);
		}
		
		cur = cur->next;
	}
	
	if (fra.width > 0 && fra.height > 0)
	{
		fra.bytes = (unsigned char *) malloc(COLOR_BYTES * fra.width * fra.height * sizeof(unsigned char));
	}
	else
	{
		fra.width = 0;
		fra.height = 0;
		fra.bytes = NULL;
	}
	
	return fra;
}

tlight * parse_tlight(xmlDocPtr doc, xmlNodePtr cur)
{
	tlight *light = tlight_new();
	
	if (light)
	{	
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				 light->anchor = parse_tvector3d(doc, cur);
			}			
			else if (!xmlStrcmp(cur->name,"c1"))
			{
				 parse_simple(doc, cur, "%LF", &light->c1);
			}
			else if (!xmlStrcmp(cur->name,"c2"))
			{
				 parse_simple(doc, cur, "%LF", &light->c2);
			}
			else if (!xmlStrcmp(cur->name,"c3"))
			{
				 parse_simple(doc, cur, "%LF", &light->c3);
			}
			else if (!xmlStrcmp(cur->name,"color"))
			{
				 light->color = parse_color(doc, cur);
			}
			else if (!xmlStrcmp(cur->name,"intensity"))
			{
				 parse_simple(doc, cur, "%LF", &light->intensity);
			}
			
			cur = cur->next;
		}
	}
	
	return light;
}

tsphere * parse_sphere(xmlDocPtr doc, xmlNodePtr cur)
{
	tsphere *sph = tsphere_new();
	
	if (sph)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				sph->anchor = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"radius"))
			{
				sph->radius = parse_tscalar(doc,cur);
				sph->radius2 = sph->radius * sph->radius;
			}
			
			cur = cur->next;
		}
	}
	
	return sph;
}

tcylinder * parse_cylinder(xmlDocPtr doc, xmlNodePtr cur)
{
	tcylinder *cyl = tcylinder_new();
	
	if (cyl)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				cyl->anchor = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"direction"))
			{
				// Not normalizing the direction causes deformations in the raytracer cylinder
				cyl->direction = v_normalize(parse_tvector3d(doc,cur), NULL);
			}
			else if (!xmlStrcmp(cur->name,"radius"))
			{
				cyl->radius = parse_tscalar(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"h1"))
			{
				cyl->h1 = parse_tscalar(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"h2"))
			{
				cyl->h2 = parse_tscalar(doc,cur);
			}
			
			cur = cur->next;
		}
		
		*cyl = tcylinder_init(cyl->anchor, cyl->direction, cyl->radius, cyl->h1, cyl->h2);
	}
		
	return cyl;
}

tcone * parse_cone(xmlDocPtr doc, xmlNodePtr cur)
{
	tcone *cn = tcone_new();
	tscalar v = 1.0;
	tscalar u = 1.0;
	
	if (cn)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				cn->anchor = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"direction"))
			{
				// Not normalizing the direction causes deformations in the raytracer cone
				cn->direction = v_normalize(parse_tvector3d(doc,cur), NULL);
			}
			else if (!xmlStrcmp(cur->name,"u"))
			{
				u = parse_tscalar(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"v"))
			{
				v = parse_tscalar(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"h1"))
			{
				cn->h1 = parse_tscalar(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"h2"))
			{
				cn->h2 = parse_tscalar(doc,cur);
			}
			
			cur = cur->next;
		}
		
		*cn = tcone_init(cn->anchor, cn->direction, v / u, cn->h1, cn->h2);
	}
		
	return cn;
}

tplane * parse_plane(xmlDocPtr doc, xmlNodePtr cur)
{
	tplane *pla = tplane_new();
	tvector3d anchor = DEFAULT_POINT;
	tvector3d direction = DEFAULT_DIRECTION;
	
	if (pla)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				anchor = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"direction"))
			{
				// The init process makes the normalization process
				direction = parse_tvector3d(doc,cur);
			}
			
			cur = cur->next;
		}
		
		*pla = tplane_init(anchor, direction);
	}
		
	return pla;
}

tellipse * parse_ellipse(xmlDocPtr doc, xmlNodePtr cur)
{
	tellipse *elp = tellipse_new();
	tvector3d direction = DEFAULT_DIRECTION;
	
	if (elp)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"focus1"))
			{
				elp->focus1 = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"focus2"))
			{
				elp->focus2 = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"direction"))
			{
				// The init process makes the normalization process
				direction = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"k1"))
			{
				 parse_simple(doc,cur,"%LF",&(elp->k1));
			}
			else if (!xmlStrcmp(cur->name,"k2"))
			{
				 parse_simple(doc,cur,"%LF",&(elp->k2));
			}
			
			cur = cur->next;
		}
		
		*elp = tellipse_init(elp->focus1, elp->focus2, direction, elp->k1, elp->k2);
	}
		
	return elp;
}

tdisc * parse_disc(xmlDocPtr doc, xmlNodePtr cur)
{
	tdisc *dsc = tdisc_new();
	tvector3d direction = DEFAULT_DIRECTION;
	
	if (dsc)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"anchor"))
			{
				dsc->anchor = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"direction"))
			{
				// The init process makes the normalization process
				direction = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"radius1"))
			{
				 parse_simple(doc,cur,"%LF",&(dsc->rad1));
			}
			else if (!xmlStrcmp(cur->name,"radius2"))
			{
				 parse_simple(doc,cur,"%LF",&(dsc->rad2));
			}
			
			cur = cur->next;
		}
		
		*dsc = tdisc_init(dsc->anchor, direction, dsc->rad1, dsc->rad2);
	}
		
	return dsc;
}

ttriangle * parse_triangle(xmlDocPtr doc, xmlNodePtr cur)
{
	ttriangle *tri = ttriangle_new();
	einside_method method = IM_BARYCENTRIC_COORDS;
	
	if (tri)
	{
		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"a"))
			{
				tri->a = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"b"))
			{
				tri->b = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"c"))
			{
				tri->c = parse_tvector3d(doc,cur);
			}
			else if (!xmlStrcmp(cur->name,"method"))
			{
				parse_simple(doc,cur,"%d",&method);
			}
			
			cur = cur->next;
		}
		
		*tri = ttriangle_init(tri->a, tri->b, tri->c, method);
	}
		
	return tri;
}

tpolygon * parse_polygon(xmlDocPtr doc, xmlNodePtr cur)
{
	tpolygon *pol = tpolygon_new();
	tvector3d *points = NULL;

	if (pol)
	{		
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name,"num_points"))
			{
				parse_simple(doc, cur, "%ld", &(pol->num_points));
			}
			else if (!xmlStrcmp(cur->name,"points"))
			{				
				if (pol->num_points > 2)
				{
					points = (tvector3d *) malloc(pol->num_points * sizeof(tvector3d));
					
					if (points)
					{
						
						xmlNodePtr points_cur = cur->xmlChildrenNode;
						long i = 0;
						
						while (points_cur && i < pol->num_points)
						{
							if (!xmlStrcmp(points_cur->name,"point"))
							{								
								points[i++] = parse_tvector3d(doc, points_cur);
							}
							
							points_cur = points_cur->next;
						}
						
						pol->num_points = i;
						
						if (pol->num_points < 3)
						{
							free(points);
							points = NULL;
							pol->num_points = 0;
						}
					}
				}
			}
			
			cur = cur->next;
		}
		
		if (points)
		{			
			*pol = tpolygon_init(points, pol->num_points);
		}
		else
		{
			free(pol);
			pol = NULL;
		}
	}
		
	return pol;
}

void * parse_properties(xmlDocPtr doc, xmlNodePtr cur, eobject *type)
{
	
	void *properties = NULL;
	
	*type = UNKNOWN;
	xmlNodePtr typeptr = cur->xmlChildrenNode;
	
	while (typeptr)
	{
		if (!xmlStrcmp(typeptr->name,"type"))
		{
			xmlChar *key = xmlNodeListGetString(doc, typeptr->children, 1);
			
			if (key)
			{				
				if (!xmlStrcmp(key,"sphere"))			*type = SPHERE;
				else if (!xmlStrcmp(key,"cylinder"))	*type = CYLINDER;
				else if (!xmlStrcmp(key,"cone"))		*type = CONE;
				else if (!xmlStrcmp(key,"plane"))		*type = PLANE;
				else if (!xmlStrcmp(key,"triangle"))	*type = TRIANGLE;
				else if (!xmlStrcmp(key,"polygon"))		*type = POLYGON;
				else if (!xmlStrcmp(key,"disc"))		*type = DISC;
				else if (!xmlStrcmp(key,"ellipse"))		*type = ELLIPSE;
				/** TODO: Add here other types */
				
				xmlFree(key);
			}
			break;
		}
		
		typeptr = typeptr->next;
	}
	
	if		(*type == SPHERE) 		properties = parse_sphere(doc, cur);
	else if (*type == CYLINDER) 	properties = parse_cylinder(doc, cur);
	else if (*type == CONE) 		properties = parse_cone(doc, cur);
	else if (*type == PLANE) 		properties = parse_plane(doc, cur);
	else if (*type == TRIANGLE) 	properties = parse_triangle(doc, cur);
	else if (*type == POLYGON) 		properties = parse_polygon(doc, cur);
	else if (*type == DISC) 		properties = parse_disc(doc, cur);
	else if (*type == ELLIPSE) 		properties = parse_ellipse(doc, cur);
	/** TODO: Add here other types */
	
	return properties;
}

tcutplane *parse_cutplane(xmlDocPtr doc, xmlNodePtr cur, tlist *planes)
{
	/** TODO implement cut planes' parsing */
	return NULL;
}

tobject* parse_tobject(xmlDocPtr doc, xmlNodePtr cur, tscene *scn)
{
	tobject *object = tobject_new();
	tcutplane *cplane = NULL;
	
	if (object)
	{	
		cur = cur->xmlChildrenNode;
		
		while (cur)
		{
			if (!xmlStrcmp(cur->name, "color"))
			{
				object->color = parse_color(doc, cur);
			}
			else if (!xmlStrcmp(cur->name, "environment_coeficient"))
			{
				parse_simple(doc, cur, "%LF", &object->env_k);
			}
			else if (!xmlStrcmp(cur->name, "difuse_coeficient"))
			{
				parse_simple(doc, cur, "%LF", &object->difuse_k);
			}
			else if (!xmlStrcmp(cur->name, "specular_coeficient"))
			{
				parse_simple(doc, cur, "%LF", &object->specular_k);
			}
			else if (!xmlStrcmp(cur->name, "specular_exponent"))
			{
				parse_simple(doc, cur, "%LF", &object->specular_n);
			}
			else if (!xmlStrcmp(cur->name, "plane"))
			{
				cplane = parse_cutplane(doc, cur, &scn->planes);
				
				if (cplane)
				{
					tlist_insert_last(&object->planes, cplane);
				}
			}
			else if (!xmlStrcmp(cur->name, "properties"))
			{
				eobject type;
				object->properties = parse_properties(doc, cur, &type);
				
				if (object->properties)
				{
				
					if (type == SPHERE)
					{
						object->intersections = tsphere_intersections;
						object->normal = tsphere_normal;
						object->free_properties = free;
						
						tsphere *sph = (tsphere *) object->properties;
							
						printf("Sphere:\n");
						printf("\tAnchor: (%.2LF, %.2LF, %.2LF)\n", sph->anchor.x, sph->anchor.y, sph->anchor.z);
						printf("\tRadius: %.2LF\n", sph->radius);
					}
					else if (type == CYLINDER)
					{
						object->intersections = tcylinder_intersections;
						object->normal = tcylinder_normal;
						object->free_properties = free;
						
						tcylinder *cyl = (tcylinder *) object->properties;
							
						printf("Cylinder:\n");
						printf("\tAnchor: (%.2LF, %.2LF, %.2LF)\n", cyl->anchor.x, cyl->anchor.y, cyl->anchor.z);
						printf("\tDirection: (%.2LF, %.2LF, %.2LF)\n", cyl->direction.x, cyl->direction.y, cyl->direction.z);
						printf("\tRadius: %.2LF\n", cyl->radius);
						printf("\tH1: %.2LF\n", cyl->h1);
						printf("\tH2: %.2LF\n", cyl->h2);
					}
					else if (type == CONE)
					{
						object->intersections = tcone_intersections;
						object->normal = tcone_normal;
						object->free_properties = free;
						
						tcone *cn = (tcone *) object->properties;
							
						printf("Cone:\n");
						printf("\tAnchor: (%.2LF, %.2LF, %.2LF)\n", cn->anchor.x, cn->anchor.y, cn->anchor.z);
						printf("\tDirection: (%.2LF, %.2LF, %.2LF)\n", cn->direction.x, cn->direction.y, cn->direction.z);
						printf("\tRatio: %.2LF\n", cn->radius);
						printf("\tH1: %.2LF\n", cn->h1);
						printf("\tH2: %.2LF\n", cn->h2);
					}
					else if (type == PLANE)
					{
						object->intersections = tplane_intersections;
						object->normal = tplane_normal;
						object->free_properties = free;
						
						tplane *pla = (tplane *) object->properties;
							
						printf("Plane:\n");
						printf("\tA: %.2LF\n", pla->x);
						printf("\tB: %.2LF\n", pla->y);
						printf("\tC: %.2LF\n", pla->z);
						printf("\tD: %.2LF\n", pla->w);
					}
					else if (type == TRIANGLE)
					{
						object->intersections = ttriangle_intersections;
						object->normal = ttriangle_normal;
						object->free_properties = free;
						
						ttriangle *tri = (ttriangle *) object->properties;
							
						printf("triangle:\n");
						printf("\tPlane: [%.2LF, %.2LF, %.2LF, %.2LF]\n", tri->plane.x, tri->plane.y, tri->plane.z, tri->plane.w);
						printf("\tA: [%.2LF, %.2LF, %.2LF]\n", tri->a.x, tri->a.y, tri->a.z);
						printf("\tB: [%.2LF, %.2LF, %.2LF]\n", tri->b.x, tri->b.y, tri->b.z);
						printf("\tC: [%.2LF, %.2LF, %.2LF]\n", tri->c.x, tri->c.y, tri->c.z);
						printf("\tVector AB: [%.2LF, %.2LF, %.2LF]\n", tri->ab.x, tri->ab.y, tri->ab.z);
						printf("\tVector AC: [%.2LF, %.2LF, %.2LF]\n", tri->ac.x, tri->ac.y, tri->ac.z);
						if (tri->method == ttriangle_inside_same_side_method)
						{							
							printf("\tVector BC: [%.2LF, %.2LF, %.2LF]\n", tri->bc.x, tri->bc.y, tri->bc.z);
							printf("\tMethod: Same side\n");
						}
						else
						{
							printf("\tInverse denominator: %.2LF\n", tri->inv_denom);
							printf("\tMethod: Barycentric coordinates system\n");
						}
					}
					else if (type == POLYGON)
					{
						object->intersections = tpolygon_intersections;
						object->normal = tpolygon_normal;
						object->free_properties = tpolygon_destroy;
						
						tpolygon *pol = (tpolygon *) object->properties;
							
						printf("Polygon:\n");
						printf("\tPlane: [%.2LF, %.2LF, %.2LF, %.2LF]\n", pol->plane.x,
																		  pol->plane.y,
																		  pol->plane.z,
																		  pol->plane.w);
						printf("\tU coordinate: %c\n", (pol->u_axis == X) ? 'X' : 'Y');
						printf("\tV coordinate: %c\n", (pol->v_axis == Z) ? 'Z' : 'Y');
						printf("\tNumber of points: %ld\n", pol->num_points);
						printf("\tPoints: %ld\n", pol->num_points);
						long i;
						
						for (i = 0; i < pol->num_points; i++)
						{
							printf("\t\t[%.2LF, %.2LF]\n", pol->points[i].x, pol->points[i].y);
						}
					}
					else if (type == DISC)
					{
						object->intersections = tdisc_intersections;
						object->normal = tdisc_normal;
						object->free_properties = free;
						
						tdisc *dsc = (tdisc *) object->properties;
							
						printf("Disc:\n");
						printf("\tAnchor: [%.2LF, %.2LF, %.2LF]\n", dsc->anchor.x, dsc->anchor.y, dsc->anchor.z);
						printf("\tPlane: [%.2LF, %.2LF, %.2LF, %.2LF]\n", dsc->plane.x, dsc->plane.y, dsc->plane.z, dsc->plane.w);
						printf("\tRadius 1: %.2LF\n", dsc->rad1);
						printf("\tRadius 1 (square): %.2LF\n", dsc->sqrrad1);						
						printf("\tRadius 2: %.2LF\n", dsc->rad2);
						printf("\tRadius 2 (square): %.2LF\n", dsc->sqrrad2);
					}
					else if (type == ELLIPSE)
					{
						object->intersections = tellipse_intersections;
						object->normal = tellipse_normal;
						object->free_properties = free;
						
						tellipse *elp = (tellipse *) object->properties;
							
						printf("ellipse:\n");
						printf("\tFocus1: [%.2LF, %.2LF, %.2LF]\n", elp->focus1.x, elp->focus1.y, elp->focus1.z);
						printf("\tFocus2: [%.2LF, %.2LF, %.2LF]\n", elp->focus2.x, elp->focus2.y, elp->focus2.z);
						printf("\tPlane: [%.2LF, %.2LF, %.2LF, %.2LF]\n", elp->plane.x, elp->plane.y, elp->plane.z, elp->plane.w);
						printf("\tK 1: %.2LF\n", elp->k1);
						printf("\tK 1 (square): %.2LF\n", elp->sqrk1);						
						printf("\tK 2: %.2LF\n", elp->k2);
						printf("\tK 2 (square): %.2LF\n", elp->sqrk2);
					}
					/** TODO: Add more objects here*/
				}
				else
				{
					printf("Cannot read properties.\n");
				}
			}
						
			cur = cur->next;
		}
	}
	
	if (!object->properties)
	{
		free(object);
		object = NULL;
	}

	return object;
}

tscene parse_tscene(xmlDocPtr doc, xmlNodePtr cur)
{
	
	tscene scn;
	tlight *light;
	tobject *object;
	tplane *plane;
	
	scn = tscene_init(DEFAULT_SCENE_BKCOLOR, DEFAULT_SCENE_ENV_INTENSITY );
	
	cur = cur->xmlChildrenNode;
	
	while (cur)
	{
		if (!xmlStrcmp(cur->name,"color"))
		{
			scn.bkcolor = parse_color(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"environment_intensity"))
		{
			scn.env_intensity = parse_tscalar(doc, cur);
		}
		else if (!xmlStrcmp(cur->name,"light"))
		{			
			light = parse_tlight(doc, cur);
			
			if (light)
			{
				tlist_insert_first(&scn.lights, light);
				printf("Light %ld:\n", scn.lights.size);
				printf("\torigin = (%.2LF, %.2LF, %.2LF)\n", light->anchor.x, light->anchor.y, light->anchor.z);
				printf("\tcolor = (%.2LF, %.2LF, %.2LF, %.2LF)\n", light->color.x, light->color.y, light->color.z, light->color.w);
				printf("\tintensity = %.2LF\n", light->intensity);
			}
		}
		else if (!xmlStrcmp(cur->name,"object"))
		{
			object = parse_tobject(doc, cur, &scn);
			
			if (object)
			{
				tlist_insert_first(&scn.objects, object);
				printf("Object %ld:\n", scn.objects.size);
				printf("\tcolor = (%.2LF, %.2LF, %.2LF, %.2LF)\n", object->color.x, object->color.y, object->color.z, object->color.w);
				printf("\tenvironment coeficient: %.2LF\n", object->env_k);
				printf("\tdifuse coeficient: %.2LF\n", object->difuse_k);
				printf("\tspecular coeficient: %.2LF\n", object->specular_k);
				printf("\tspecular exponent: %.2LF\n", object->specular_n);
			}
		}
		else if (!xmlStrcmp(cur->name,"plane"))
		{
			plane = parse_plane(doc, cur);
			
			if (plane)
			{
				tlist_insert_first(&scn.planes, plane);
				printf("Plane %ld:\n", scn.planes.size);
				printf("\tA = %.2LF\n", plane->x);
				printf("\tB = %.2LF\n", plane->y);
				printf("\tC = %.2LF\n", plane->z);
				printf("\tD = %.2LF\n", plane->w);
			}
		}
		
		cur = cur->next;
	}	
	
	return scn;
}

int parse_model(const char *docname, tvector3d *origin, twindow *wnd, tframe *fra, tscene *scn)
{
	
	xmlDocPtr doc;
	xmlNodePtr cur;
	
	doc = xmlParseFile(docname);
	
	if (doc)
	{
		cur = xmlDocGetRootElement(doc);
		
		if (cur && (!xmlStrcmp(cur->name, (const xmlChar *) "model")))
		{
			printf("Root element: %s\n",cur->name);
			cur = cur->xmlChildrenNode;
			
			while (cur)
			{
				if ((!xmlStrcmp(cur->name, (const xmlChar *) "origin")))
				{
					*origin = parse_tvector3d(doc, cur);
					printf("Origin: [%.2LF, %.2LF, %.2LF]\n", origin->x, origin->y, origin->z);
				}
				else if ((!xmlStrcmp(cur->name, (const xmlChar *) "window")))
				{
					*wnd = parse_twindow(doc, cur);
					printf("Window:\n");
					printf("\torigin = [%.2LF, %.2LF, %.2LF]\n", wnd->origin.x, wnd->origin.y, wnd->origin.z);
					printf("\twidth = %.2LF\n", wnd->width);
					printf("\theight = %.2LF\n", wnd->height);
				}
				else if ((!xmlStrcmp(cur->name, (const xmlChar *) "frame")))
				{
					*fra = parse_tframe(doc, cur);
					printf("Frame:\n");
					printf("\twidth = %lu\n", fra->width);
					printf("\theight = %lu\n", fra->height);
				}
				else if ((!xmlStrcmp(cur->name, (const xmlChar *) "scene")))
				{
					*scn = parse_tscene(doc, cur);
					printf("Scene:\n");
					printf("\tcolor = (%.2LF, %.2LF, %.2LF, %.2LF)\n", scn->bkcolor.x, scn->bkcolor.y, scn->bkcolor.z, scn->bkcolor.w);
					printf("\tenvironment intensity = %.2LF\n", scn->env_intensity);
				}
				
				cur = cur->next;
			}
		}
		
		xmlFreeDoc(doc);
		return 1;
	}
	return 0;
}
