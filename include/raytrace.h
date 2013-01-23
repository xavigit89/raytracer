#ifndef __RAYTRACE_H
#define __RAYTRACE_H

#include <assert.h>
#include "objects.h"

#define A 0
#define R 1
#define G 2
#define B 3

#define MAX_INTERSECTIONS 5

/**
 * @name get_intersections
 * @brief Obtains the intersections of the given ray ('origin', 'direction') with the objects of the scene.
 * 
 * For each object on the scene, this function collects pairs of [distance, object]
 * representing an intersection of the ray with the object at that distance. Only distances
 * satisfying the POSITIVE expression are considered. The collection is sorted in ascending
 * order by the distance value.
 * 
 * @param [IN]	objects		The objects whose intersections with the ray we want to obtain.
 * @param [IN]	origin		The ray origin point.
 * @param [IN]	direction	The ray direction vector.
 * 
 * @returns A list of intersections (pairs of [distance, object]) sorted in ascending order by distance value.
 */
tlist get_intersections (tlist objects, tvector3d origin, tvector3d direction);

/**
 * @name get_color
 * @brief Obtains the color being seen at the scene, from the 'origin' point, in a given 'direction'.
 * 
 * Given a ray (represented by 'origin' and 'direction'), this function
 * take the objects intersecting that ray, and calculates the color
 * being seen at the scene in that direction, by agregating the colors
 * of the intersecting objects, depending on object attributes like
 * its color, transparence, and others,  and the amount of light received
 * by each object at the intersection point. The intersections are analyzed
 * from closest to the farthest but, depending on the visual effects being
 * applied, a farther object may be analyzed first.
 *   
 * @param [IN]	scn			The scene to be traced.
 * @param [IN]	origin		The ray origin point.
 * @param [IN]	direction	The ray direction vector.
 * 
 * @returns A four-dimensional vector representing a RGBA color.
 * @retval scn.bkcolor If the ray doesn't intersects any object of the scene.
 */
tvector4d get_color (tscene scn, tvector3d origin, tvector3d direction);

/**
 * @name raytrace
 * @brief Raytraces an scene from an 'origin', given a window, into a given frame.
 * 
 * For every pixel in the frame, this functions determines the corresponding color
 * asociated with the scene. For doing so, rays are traced from the 'origin' point
 * to the scene, and the color saw at the ray's direction is assigned to the pixel.
 *  
 * @param [IN]		wnd		The window throught which the scene is visible.
 * @param [IN]		scn		The scene to be traced.
 * @param [IN]		origin	The point the scene is traced from.
 * @param [INOUT]	fra		The frame where the traced scene is going to be stored.
 */
void raytrace (twindow wnd, tscene scn, tvector3d origin, tframe fra);

#endif

