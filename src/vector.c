#include "vector.h"

int is_contained(tscalar lower, tscalar upper, tscalar value) {
	return (lower <= value && value <= upper);
}

tvector3d v_from_values (tscalar x, tscalar y, tscalar z) {
	return (tvector3d) { x, y, z};
}

tvector3d v_add (tvector3d v, tvector3d w) {
	return (tvector3d) { v.x + w.x, v.y + w.y, v.z + w.z };
}

tvector3d v_sub (tvector3d v, tvector3d w) {
	return (tvector3d) { v.x - w.x, v.y - w.y, v.z - w.z };
}

tvector3d v_scale (tvector3d v, tscalar k) {
	return (tvector3d) { k * v.x, k * v.y, k * v.z };
}

tvector3d v_cross_product (tvector3d v, tvector3d w) {
	return (tvector3d) { v.y * w.z - v.z * w.y, v.z * w.x - v.x * w.z, v.x * w.y - v.y * w.x };
}

tvector3d v_normalize (tvector3d v, tscalar * norm) {
	tscalar l = v_norm(v);
	
	if (ZERO(l)) {
		if (norm) *norm = 0.0;
		return (tvector3d) { 0.0, 0.0, 0.0 };
	}
	else {
		if (norm) *norm = l;
		return (tvector3d) { v.x / l, v.y / l, v.z / l };
	}
}

tvector3d v_point_at (tvector3d anchor, tvector3d direction, tscalar distance) {
	return v_add(anchor, v_scale(direction, distance));
}

tscalar v_dot_product (tvector3d v, tvector3d w) {
	return v.x * w.x + v.y * w.y + v.z * w.z;
}

tscalar v_norm (tvector3d v) {
	return sqrtl(v.x * v.x + v.y * v.y + v.z * v.z);
}
