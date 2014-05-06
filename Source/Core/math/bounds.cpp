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

    for(int i = 1; i < p->GetNum(); i++)
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

bool IntersectLine(vec3* a1, vec3* a2, vec3* b1, vec3* b2,vec3* c)
{
    double ady = a1->y - a2->y;
    double adx = a2->x - a1->x;
    double bdy = b1->y - b2->y;
    double bdx = b2->x - b1->x;
    double D = Determinant(ady, adx, bdy, bdx);

    if (fabs(D) < math_epsilon)
	    return true;

    double aC = adx * a1->y + ady * a1->x;
    double bC = bdx * b1->y + bdy * b1->x;
    double DX = Determinant(aC, adx, bC, bdx);
    double DY = Determinant(ady, aC, bdy, bC);
    
    c->x = DX / D;
    c->y = DY / D;
  
    return false;
}

bool IntersectConvexShapePoint(VertexBuffer* buffer, mat4 model, vec3* point)
{
    vec3 model_space_point = inverse(model) * (*point);
    vec3 a = ((Vertex*)buffer->GetPointer())[0].pos - model_space_point;
	vec3 b;
	
    double summ = 0;
	for(int i = 1; i< buffer->GetNum() ; i++)
    {
        b = ((Vertex*)buffer->GetPointer())[i].pos - model_space_point;
        summ += dot(a,b);
		a = b;	
	}

	return summ < 360;
}

bool IntersectAABB(AABB *a, AABB *b)
{
    if(a->top_left.x > b->top_left.x)
        return IntersectAABB(b, a);

    return a->bottom_right.x > b->top_left.x  && !(a->bottom_right.y > b->top_left.y || b->bottom_right.y > a->top_left.y);
}

// [res.x,res.y] - interval of projection Poly to Vector
vec3 Projection(vector<vec3> &poly, vec3 point, vec3& min_point, vec3& max_point){
	vec3 res;
    res.y = res.x = projection(poly[0], point);
    for(int i = 1; i < poly.size() ; i++){
		double temp = projection(poly[i], point);
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
bool IntersectConvexShape(VertexBuffer* a, mat4 model_a, VertexBuffer* b, mat4 model_b, vec3 &contact_point, vec3 &contact_normal)
{
	// potential separating axis
	vector<vec3> psa; 

	vector<vec3> transformed_a;
    for(int i = 0; i< a->GetNum(); i++)
        transformed_a.push_back(model_a * ((Vertex*)a->GetPointer())[i].pos);
	
    vector<vec3> transformed_b;
    for(int i = 0; i< b->GetNum(); i++)
        transformed_b.push_back(model_b * ((Vertex*)b->GetPointer())[i].pos);
	
    for(int i = 0; i < transformed_a.size()- 1; i++)
    {
		vec3 temp = transformed_a[i+1] - transformed_a[i];
        psa.push_back(GLRotationZ(90.0f) * temp);
	}

    vec3 last = transformed_a[0] - transformed_a[transformed_a.size()-1];
	psa.push_back(normalize(GLRotationZ(90.0f) * last));
	
    for(int i = 0; i < transformed_b.size()- 1; i++)
    {
		vec3 temp = transformed_b[i+1] - transformed_b[i];
        psa.push_back(normalize(GLRotationZ(90.0f) * temp));
	}

    last = transformed_b[0] - transformed_b[transformed_b.size()-1];
	psa.push_back(GLRotationZ(90.0f) * last * -1.0);

	//check axies
	int min_index = -1;
	double min = 0.0;   

    vec3 min_max_points[4];

    for(int i = 0; i < psa.size(); i++)
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

//    if(min_index == -1)
  //      return false;

    Projection(transformed_a, psa[min_index], min_max_points[0], min_max_points[1]);
    Projection(transformed_b, psa[min_index], min_max_points[2], min_max_points[3]);

    //result vector, lenght = intersection
    float contact_normal_lenght = fabs(min);
    contact_normal = normalize(psa[min_index]) * contact_normal_lenght;
    
	bool a_intersect = false;
	vector<vec3> *intersected_obj = &transformed_a;
	vector<vec3> *intersect_obj = &transformed_b;

    if(min_index >= transformed_a.size())
    {
		a_intersect = true;
		intersected_obj = &transformed_b;
		intersect_obj   = &transformed_a;
		min_index -= transformed_a.size();
	}
	
	// intersect edge
	vec3 p1 , p2;
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

	// intersect point
	p2 = p2 - p1;
    double l = length(p2);
    float error = 999999.0f;
 
    for(int i = 0; i < 4; i++)
    {
        vec3 temp = min_max_points[i] - p1;
        double temp_proj = projection(temp, p2);
        double dist = fabs(distance_to_line(min_max_points[i], p1, p2));
        float current_error = fabs(temp_proj - l) + fabs(dist - contact_normal_lenght);
        if(current_error <= error)
        {
            contact_point = min_max_points[i];
            error = current_error;
        }
    }
    
    return true;
}