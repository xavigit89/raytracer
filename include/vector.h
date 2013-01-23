#ifndef __VECTOR_H
#define __VECTOR_H

#include <math.h>

#define X 0
#define Y 1
#define Z 2
#define W 3
#define DIM3D 3
#define DIM4D 4
#define EPSILON 0.0005

// Macros used to compare floating-point values
/**
 * @def ZERO(X)
 * Determines whether or not a value X is in the range [-EPSILON, EPSILON]
 * */
#define ZERO(X) (-EPSILON <= (X) && (X) <= EPSILON)	
/**
 * @def POSITIVE(X)
 * Determines whether or not a value X is greather than EPSILON
 * */
#define POSITIVE(X) ((X) > EPSILON)
/**
 * @def NEGATIVE(X)
 * Determines whether or not a value X is less than -EPSILON
 * */
#define NEGATIVE(X) ((X) < -EPSILON)
/**
 * @def NONPOSITIVE(X)
 * Determines whether or not a value X is less than or equal to EPSILON
 * */
#define NONPOSITIVE(X) ((X) <= EPSILON)
/**
 * @def NONNEGATIVE(X)
 * Determines whether or not a value X is greather than or equal to -EPSILON
 * */
#define NONNEGATIVE(X) ((X) >= -EPSILON)
/**
 * @def SIGN(X)
 * Determines the sign of a value X.
 * 
 * @retval 0	If X is satisfies ZERO(X)
 * @retval 1	If X is satisfies POSITIVE(X)
 * @retval -1	If X is satisfies NEGATIVE(X)
 * */
#define SIGN(X) (ZERO(X) ? 0 : (POSITIVE(X) ? 1 : -1))

typedef long double tscalar;
typedef tscalar* tvector;
typedef struct { tscalar x, y, z; } tvector3d;
typedef struct { tscalar x, y, z, w; } tvector4d;

// Vector related functions
tvector3d v_from_values (tscalar x, tscalar y, tscalar z);
tvector3d v_add (tvector3d v, tvector3d w);
tvector3d v_sub (tvector3d v, tvector3d w);
tvector3d v_scale (tvector3d v, tscalar k);
tvector3d v_cross_product (tvector3d v, tvector3d w);
tvector3d v_normalize (tvector3d v, tscalar * norm);
tvector3d v_point_at (tvector3d anchor, tvector3d direction, tscalar distance);
tscalar v_dot_product (tvector3d v, tvector3d w);
tscalar v_norm (tvector3d v);

#endif
