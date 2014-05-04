#ifndef _BOUNDS_H_
#define _BOUNDS_H_

#include "vec3.h"

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
	vec2 right_down;
    	
	void Calculate(VertexBuffer* p, mat4 model);
};

bool IntersectConvexShape(VertexBuffer* a, mat4 model_a, VertexBuffer* b, mat4 model_b, vec3 &contact, vec3 &c, double &d);
bool IntersectAABB(AABB *a, AABB *b);
bool IntersectConvexShapePoint(VertexBuffer* buffer, mat4 model, vec3* point);
bool IntersectLine(vec3* a1, vec3* a2, vec3* b1, vec3* b2,vec3* c);

#endif //__H_