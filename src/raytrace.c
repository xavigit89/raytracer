#include "raytrace.h"
#include <stdio.h>

tlist get_intersections (tlist objects, tvector3d origin, tvector3d direction) {
	tlistnode *node = objects.first;
	tobject *obj;
	tscalar distances[MAX_INTERSECTIONS];
	tintersection *inter;
	long count, i;
			
	tlist intersections = tlist_init(free, tintersection_compare, NONREVERSE);
	
	while (node) {
		obj = (tobject *) node->value;
		assert((obj->properties));
		obj->intersections(obj->properties, origin, direction, &count, (tscalar *) distances);
		
		for (i = 0; i < count; i++) {
			
			if (POSITIVE(distances[i])) {
				inter = tintersection_new(distances[i], obj);
				
				if (inter) {
					tlist_insert_sorted(&intersections, inter);
				}
			}
		}
		
		node = node->next;
	}

	return intersections;
}

tvector4d get_color (tscene scn, tvector3d origin, tvector3d direction) {
	static long count = 0;
	
	tlist intersections = get_intersections(scn.objects, origin, direction);	
	assert(intersections.size >= 0);
	
	if (intersections.size > 0) {		
		assert((intersections.first));
		tintersection * inter = intersections.first->value;
		tobject * obj = inter->object;
		tvector3d point = v_point_at (origin, direction, inter->distance);
		printf("Count: %ld\n", ++count);
		assert(obj);
		assert(obj->normal);
		assert(obj->properties);
		tvector3d normal = (obj->normal)(obj->properties, point);		
		tvector3d anti_direction = v_scale(direction, -1.0);
		tvector3d light_reflex;
		tvector3d to_light;
		tvector3d dif_color,spec_color;
		tscalar dif_ilum = 0.0, spec_ilum = 0.0, light_dist, normal_to_light, attenuation, att_intensity, reflex_anti_direction, rad_power;
		tlistnode * lightnode = scn.lights.first;
		tlight *light;
		tlist obstacles;
		long i;
		
		if (NONNEGATIVE(v_dot_product(normal,direction))) {
			normal = v_scale(normal, -1.0);
		}
				
		while (lightnode) {
			assert((lightnode->value));
			light = (tlight *) lightnode->value;
			to_light = v_normalize(v_sub(light->anchor, point), &light_dist);
			normal_to_light = v_dot_product(normal, to_light);
			// Including shadows
			obstacles = get_intersections(scn.objects, point, to_light);
			
			if (POSITIVE(normal_to_light) && (obstacles.size == 0 || ((tintersection *) obstacles.first->value)->distance > light_dist)) {
				attenuation = 1.0 / (light->c1 + light->c2 * light_dist + light->c3 * light_dist * light_dist);
				att_intensity = (attenuation > 1.0) ? light->intensity : (attenuation * light->intensity);
				
				dif_ilum += normal_to_light * obj->difuse_k * att_intensity;
				light_reflex = v_scale(v_sub(v_scale(normal, v_dot_product(normal, to_light)), to_light), 2.0);
				reflex_anti_direction = v_dot_product(light_reflex, anti_direction);
				
				if (POSITIVE(reflex_anti_direction)) {
					rad_power = reflex_anti_direction;
					
					for (i = 1; i < obj->specular_n; i++) {
						rad_power *= reflex_anti_direction;
					}
					spec_ilum += rad_power * obj->specular_k * att_intensity;
				}
			}
			
			tlist_clear(&obstacles);
			
			lightnode = lightnode->next;
		}
		
		tlist_clear(&intersections);
		
		dif_ilum += scn.env_intensity * obj->env_k;
		dif_ilum = (dif_ilum > 1.0) ? 1.0 : dif_ilum;
		spec_ilum = (spec_ilum > 1.0) ? 1.0 : spec_ilum;
		
		dif_color = v_scale(*(tvector3d *) &(obj->color), dif_ilum);
		spec_color = v_add(dif_color, v_scale(v_sub((tvector3d) { 1.0, 1.0, 1.0 },  dif_color), spec_ilum));
		
		return (tvector4d) { ((spec_color.x > 1.0) ? 1.0 : spec_color.x),
							 ((spec_color.y > 1.0) ? 1.0 : spec_color.y),
							 ((spec_color.z > 1.0) ? 1.0 : spec_color.z),
							 obj->color.w };
	}
	else {
		return scn.bkcolor;
	}	
}

void raytrace (twindow wnd, tscene scn, tvector3d origin, tframe fra) {
	long i,j;
	unsigned char *bytecolor;
	tvector3d p,direction;
	tvector4d scalarcolor;
	tscalar width_ratio, height_ratio;
	
	p.z = 0.0;
	
	width_ratio = wnd.width / fra.width;
	height_ratio = wnd.height / fra.height;
	
	bytecolor = fra.bytes;
	assert(bytecolor != NULL);
	
	for (j = fra.height - 1; j >= 0; j--) {
		for (i = 0; i < fra.width; i++) {
			p.x = (i + 0.5) * width_ratio + wnd.origin.x;
			p.y = (j + 0.5) * height_ratio + wnd.origin.y;
			
			direction = v_normalize(v_sub(p, origin), NULL);		
			scalarcolor = get_color(scn, origin, direction);
			
			assert(0 <= scalarcolor.x && scalarcolor.x <= 1.0);
			assert(0 <= scalarcolor.y && scalarcolor.y <= 1.0);
			assert(0 <= scalarcolor.z && scalarcolor.z <= 1.0);
					
			bytecolor[A] = (unsigned char) (scalarcolor.w * 255);			
			bytecolor[R] = (unsigned char) (scalarcolor.x * 255);
			bytecolor[G] = (unsigned char) (scalarcolor.y * 255);
			bytecolor[B] = (unsigned char) (scalarcolor.z * 255);
			
			bytecolor += COLOR_BYTES;
		}
	}
}
