#include "bounds.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "../Renderer.h"

void AABB:: Calculate(VertexBuffer* p, mat4 model)
{
    vec3 point = model * ((Vertex*)p->GetPointer())[0].pos;
    top_left = vec2(point.x, point.y);
    bottom_right = top_left;

    for(unsigned int i = 1; i < p->GetNum(); i++)
    {      
        point = model * ((Vertex*)p->GetPointer())[i].pos;       

        if(point.x < top_left.x)
			top_left.x = point.x;
		if(point.x > bottom_right.x)
			bottom_right.x = point.x;
		if(point.y > top_left.y)
			top_left.y = point.y;
		if(point.y < bottom_right.y)
			bottom_right.y = point.y;
    }
}

inline double Determinant(double a11, double a12, double a21, double a22)
{
    return a11 * a22 - a12 * a21;
}

bool IntersectLine(const vec3* a1, const vec3* a2, const vec3* b1, const vec3* b2, vec3* c)
{
    float ady = a1->y - a2->y;
    float adx = a2->x - a1->x;
    float bdy = b1->y - b2->y;
    float bdx = b2->x - b1->x;
    float D = Determinant(ady, adx, bdy, bdx);

    if (fabs(D) < math_epsilon)
	    return true;

    float aC = adx * a1->y + ady * a1->x;
    float bC = bdx * b1->y + bdy * b1->x;
    float DX = Determinant(aC, adx, bC, bdx);
    float DY = Determinant(ady, aC, bdy, bC);
    
    c->x = DX / D;
    c->y = DY / D;
  
    return false;
}

bool IntersectConvexShapePoint(VertexBuffer* buffer, mat4 model, vec3* point)
{
    vec3 model_space_point = inverse(model) * (*point);
    vec3 a = ((Vertex*)buffer->GetPointer())[buffer->GetNum()-1].pos;
	vec3 b;
	
    int intersections = 0;
	for(unsigned int i = 0; i< buffer->GetNum(); i++)
    {
        b = ((Vertex*)buffer->GetPointer())[i].pos;

        vec3 temp = vec3(model_space_point.x + 1000, model_space_point.y, 0.0f);
        vec3 temp1 = vec3_zero;

        intersections += IntersectLine(&b, &a, &model_space_point, &temp, &temp1);
		a = b;	
	}

    return (intersections % 2) == 1;
}

bool IntersectAABB(AABB *a, AABB *b)
{
    if(a->top_left.x > b->top_left.x)
        return IntersectAABB(b, a);

    return a->bottom_right.x > b->top_left.x  && !(a->bottom_right.y > b->top_left.y || b->bottom_right.y > a->top_left.y);
}

// [res.x,res.y] - interval of projection Poly to Vector
vec3 Projection(vector<vec3> &poly, vec3 point, vec3& min_point, vec3& max_point)
{
	vec3 res;
    res.y = res.x = dot(point, poly[0]);
    for(unsigned int i = 1; i < poly.size(); i++)
    {
		float temp =  dot(point, poly[i]);
		
        if(temp < res.x)
        {
			res.x = temp;
            min_point = poly[i];
        }

        if(temp > res.y)
        {
			res.y = temp;
            max_point = poly[i];
        }
	}
	return res;
}

//Separeting Axis Theorem
bool IntersectConvexShape(VertexBuffer* a, ::transform model_a, VertexBuffer* b, ::transform model_b, vec3 &contact_point, vec3 &contact_normal)
{
	// potential separating axis
	vector<vec3> psa; 
    
	vector<vec3> transformed_a;    
    for(unsigned int i = 0; i < a->GetNum(); i++)
    {
        vec3 pos = model_a.matrix() * ((Vertex*)a->GetPointer())[i].pos;
        transformed_a.push_back(pos);
    }

    vector<vec3> transformed_b;
    for(unsigned int i = 0; i < b->GetNum(); i++)
    {
        vec3 pos = model_b.matrix() * ((Vertex*)b->GetPointer())[i].pos;
        transformed_b.push_back(pos);
    }
       
    for(unsigned int i = 0; i < transformed_a.size() - 1; i++)
    {
		vec3 temp = transformed_a[i+1] - transformed_a[i];
        psa.push_back(normalize(vec3(-temp.y, temp.x, 0.0f)));
	}

    vec3 last = transformed_a[0] - transformed_a[transformed_a.size()-1];
	psa.push_back(normalize(vec3(-last.y, last.x, 0.0f)));
	
    for(unsigned int i = 0; i < transformed_b.size()- 1; i++)
    {
		vec3 temp = transformed_b[i+1] - transformed_b[i];
        psa.push_back(normalize(vec3(-temp.y, temp.x, 0.0f)));
	}

    last = transformed_b[0] - transformed_b[transformed_b.size()-1];
	psa.push_back(normalize(vec3(-last.y, last.x, 0.0f)));

	//check axies
	int min_index = -1;
	double min = 0.0;   

    vec3 min_max_points[4];
    for(unsigned int i = 0; i < psa.size(); i++)
    {
		vec3 r1 = Projection(transformed_a, psa[i], min_max_points[0], min_max_points[1]);
		vec3 r2 = Projection(transformed_b, psa[i], min_max_points[2], min_max_points[3]);
		
		if(r1.y < r2.x)
			return false;
		
        if(r2.y < r1.x)
			return false;
        
        if(r2.x < r1.y && r1.y < r2.y)
        {
            if((min_index == -1 && r1.y - r2.x > 0) || (min > r1.y - r2.x && r1.y - r2.x > 0))
            {
				min = r1.y - r2.x;
				min_index = i;
			}
		}

		if(r1.x < r2.y && r2.y < r1.y)
        {
			if((min_index == -1 && r2.y - r1.x < 0) || (min > r2.y - r1.x && r2.y - r1.x < 0))
            {
				min = r2.y - r1.x;
				min_index = i;
			}
		}
	}

    if(min_index == -1)
        return false;

    //result vector, lenght = intersection
    float contact_normal_lenght = (float)fabs(min);
    contact_normal = -psa[min_index] * contact_normal_lenght;
    
    //initialize
    contact_point = (model_a.position + model_b.position) * 0.5f;

	bool a_intersect = false;
	vector<vec3> *intersected_obj = &transformed_a;
	vector<vec3> *intersect_obj = &transformed_b;

    if((unsigned int)min_index >= transformed_a.size())
    {
		a_intersect = true;
		intersected_obj = &transformed_b;
		intersect_obj   = &transformed_a;
		min_index -= transformed_a.size();
	}
	
	// intersect edge
	vec3 p1, p2;
	if(min_index == intersected_obj->size() - 1)
    {
        p1 = (*intersected_obj)[intersected_obj->size()-1];	
        p2 = (*intersected_obj)[0];
    }
    else
    {
        p1 = (*intersected_obj)[min_index];	
        p2 = (*intersected_obj)[min_index+1];
    }

    float error = 999999.0f;
	
    // intersect point
	p2 = p2 - p1;
    double l = length(p2);

	for(unsigned int i = 0; i < intersect_obj->size(); i++)
    {
	    vec3 temp = (*intersect_obj)[i];
		temp = vec2(temp.x - p1.x, temp.y - p1.y);
		double temp_proj = projection(temp, p2);
        double dist = fabs(distance_to_line(temp, vec3_zero, p2));
		float current_error = (float)fabs(dist - contact_normal_lenght);
        
        float eps = 0.2f;
        //if( -eps <= temp_proj && temp_proj <= l+eps && dist <= contact_normal_lenght + eps && dist >= contact_normal_lenght - eps)
        if(current_error <= error && -math_epsilon < temp_proj && temp_proj < l + math_epsilon)
        {
            contact_point = (*intersect_obj)[i];
            error = current_error;
        }
	}

    return true;
}