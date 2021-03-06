#ifndef _BOUNDS_H_
#define _BOUNDS_H_

#include "vec2.h"
#include "vec3.h"
#include "transform.h"

struct sphere
{
    vec3 center;
    float radius;

    sphere(): center(0.0f, 0.0f, 0.0f), radius(0.0f) {}
    sphere(vec3 c, float r): center(c), radius(r) {}
};

class VertexBuffer;
struct AABB
{
	vec2 top_left;
	vec2 bottom_right;
    	
	void Calculate(VertexBuffer* p, mat4 model);
};

bool IntersectConvexShape(VertexBuffer* a, ::transform model_a, VertexBuffer* b, ::transform model_b, vec3 &contact_point, vec3 &contact_normal);
bool IntersectAABB(AABB *a, AABB *b);
bool IntersectConvexShapePoint(VertexBuffer* buffer, mat4 model, vec3* point);
bool IntersectLine(const vec3* a1, const vec3* a2, const vec3* b1, const vec3* b2, vec3* c);

#endif //__H_