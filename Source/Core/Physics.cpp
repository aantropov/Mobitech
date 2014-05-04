#include "Physics.h"
#include "math\bounds.h"

RigidBody:: RigidBody(double mass, PO_State pst)
{
    rotation = angular_force_summ = 0.0;
    this->mass =  mass;
    inertion = mass * mass *2.0;
    velocity = force_summ = vec2_zero;
    state = pst;
    elasticity = 0.5;
		
    anti_mass = 1.0 / mass;
    anti_inertion = 1.0 / inertion;
    shape = NULL;
    
    Physics::GetInstance()->RegisterRigidBody(this);
}

RigidBody:: ~RigidBody()
{
    Physics::GetInstance()->UnregisterRigidBody(this);
}

void RigidBody:: UpdatePosition(double dt)
{
    model.rotation *= quat(GLRotationZ(rotation * dt));
    model.position += vec4(velocity.x, velocity.y, 0.0f, 0.0f) * dt;
}

void RigidBody:: UpdateSpeed(double dt)
{
    velocity += force_summ * anti_mass * dt;
    rotation += angular_force_summ * anti_inertion * dt;
}

void RigidBody:: Tick(double dt)
{    
	UpdateSpeed(dt);
	UpdatePosition(dt);
    aabb.Calculate(shape, model.matrix());
}

void RigidBody:: ApplyImpulse(vec2 point, vec2 normal, double impulse)
{
    vec2 translation = vec2(model.position.x, model.position.y);
    vec2 dir = point - translation; //(x - (shape->x+shape->centre.x), y-(shape->y+shape->centre.y));
 
    velocity += normal * float(impulse * anti_mass);
    rotation += impulse * (normal.y * dir.x - normal.x * dir.y) * anti_inertion;
}

double Physics::CalculateImpulse(vec2 normal, RigidBody *m1, RigidBody *m2, vec2 point)
{
    vec2 dir_m1 = vec2(point.x - m1->model.position.x, point.y - m1->model.position.y);
	vec2 dir_m2 = vec2(point.x - m2->model.position.x, point.y - m2->model.position.y);

	//relative speed
	double vel = normal.x * (m1->velocity.x - m1->rotation * dir_m1.y - m2->velocity.x + m2->rotation * dir_m2.y) + 
				 normal.y * (m1->velocity.y + m1->rotation * dir_m1.x - m2->velocity.y - m2->rotation * dir_m2.x);
	
    double z1 = (normal.y * dir_m1.x - normal.x * dir_m1.y) * (1.0 / m1->inertion);
	double z2 = (normal.y * dir_m2.x - normal.x * dir_m2.y) * (1.0 / m2->inertion);

	double j = normal.x *  (normal.x * (m1->anti_mass) - dir_m1.y * z1 +normal.x * (m2->anti_mass) + dir_m2.y * z2) 
		    +normal.y * (normal.y * (m1->anti_mass) + dir_m1.x * z1 * (m1->anti_inertion)
			+normal.y * (m2->anti_mass) - dir_m2.x * z2 * (m2->anti_inertion));

	return ( 0 - (1 + (m1->elasticity + m2->elasticity)/2) * vel) / j;
}

void Physics::Update(double delta_time)
{
    for(int i = 0; i <  physics_objects.size(); i++ )
		for(int j = i+1; j <  physics_objects.size(); j++ )
        {
            if(IntersectAABB(&physics_objects[i]->aabb,&physics_objects[j]->aabb))
            {
			    vec3 v;
			    double d;
			    vec3  P;
			
                if(IntersectConvexShape(physics_objects[j]->shape, physics_objects[j]->model.matrix(), physics_objects[i]->shape, physics_objects[i]->model.matrix(), P, v, d))
                {
				    if(physics_objects[i]->state == PO_STATIC && physics_objects[j]->state != PO_STATIC)
                        physics_objects[j]->model.position -= v;
				    else if(physics_objects[j]->state == PO_STATIC && physics_objects[i]->state != PO_STATIC)
                        physics_objects[i]->model.position += v;
				    else if(physics_objects[j]->state != PO_STATIC && physics_objects[i]->state != PO_STATIC)
                    {
					    if(physics_objects[i]->anti_mass + physics_objects[j]->anti_mass == 0)
                        {
                            physics_objects[j]->model.position -= v/2.0f;
                            physics_objects[i]->model.position += v/2.0f;
					    }

					    physics_objects[j]->model.position -= v * (physics_objects[j]->anti_mass/(physics_objects[j]->anti_mass+physics_objects[i]->anti_mass));
					    physics_objects[i]->model.position += v * (physics_objects[j]->anti_mass/(physics_objects[j]->anti_mass+physics_objects[i]->anti_mass));
                    }

				    double impulse = CalculateImpulse(v, physics_objects[j], physics_objects[i], P);

					physics_objects[i]->velocity = vec3_zero;
					
                    if(physics_objects[i]->state != PO_STATIC)
						physics_objects[i]->ApplyImpulse(P, v, -impulse);
					if(physics_objects[j]->state != PO_STATIC)					
						physics_objects[j]->ApplyImpulse(P, v, impulse);

					physics_objects[i]->OnCollide(physics_objects[j]);
					physics_objects[j]->OnCollide(physics_objects[i]);				
			}
		}
	}

        for(int i = 0; i < physics_objects.size(); i++)
            physics_objects[i]->Tick(delta_time);
}