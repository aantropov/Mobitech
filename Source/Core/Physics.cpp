#include "Physics.h"
#include "math\bounds.h"

RigidBody:: RigidBody(double mass, PHYSICS_OBJECT_STATE pst)
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
    vec2 dir = point - model.position;
    velocity += normal * float(impulse * anti_mass);
    rotation += (impulse * (normal.y * dir.x - normal.x * dir.y) * anti_inertion);
}

Physics* Physics::GetInstance()
{
    if(instance == NULL)
        instance = new Physics();        
    return instance;
}

double Physics::CalculateImpulse(vec2 normal, RigidBody *m1, RigidBody *m2, vec2 point)
{    
    vec2 dir_m1 = vec2(point.x - m1->model.position.x, point.y - m1->model.position.y);
	vec2 dir_m2 = vec2(point.x - m2->model.position.x, point.y - m2->model.position.y);

	//relative speed
	double vel = normal.x * (m1->velocity.x - m1->rotation * dir_m1.y - m2->velocity.x + m2->rotation * dir_m2.y) + 
				 normal.y * (m1->velocity.y + m1->rotation * dir_m1.x - m2->velocity.y - m2->rotation * dir_m2.x);
	
    double z1 = (normal.y * dir_m1.x - normal.x * dir_m1.y) * m1->anti_inertion;
	double z2 = (normal.y * dir_m2.x - normal.x * dir_m2.y) * m2->anti_inertion;

	double j = normal.x *  (normal.x * (m1->anti_mass) - dir_m1.y * z1 +normal.x * (m2->anti_mass) + dir_m2.y * z2) 
		    +normal.y * (normal.y * (m1->anti_mass) + dir_m1.x * z1 * (m1->anti_inertion)
			+normal.y * (m2->anti_mass) - dir_m2.x * z2 * (m2->anti_inertion));

	return (0 - (1.0 + (m1->elasticity + m2->elasticity)/2.0) * vel) / j;
}

void Physics::Update(double delta_time)
{
    last_physics_update += delta_time;

    if(last_physics_update < update_time)
        return;

    for(int i = 0; i <  physics_objects.size(); i++ )
		for(int j = i+1; j <  physics_objects.size(); j++ )
        {
            if(IntersectAABB(&physics_objects[i]->aabb, &physics_objects[j]->aabb))
            {
			    vec3 contact_normal;
			    vec3 contact_point;
			
                if(IntersectConvexShape(physics_objects[j]->shape, physics_objects[j]->model, physics_objects[i]->shape, physics_objects[i]->model, contact_point, contact_normal))
                {
				    if(physics_objects[i]->state == PO_STATIC && physics_objects[j]->state != PO_STATIC)
                        physics_objects[j]->model.position -= contact_normal;
				    else if(physics_objects[j]->state == PO_STATIC && physics_objects[i]->state != PO_STATIC)
                        physics_objects[i]->model.position += contact_normal;
				    else if(physics_objects[j]->state != PO_STATIC && physics_objects[i]->state != PO_STATIC)
                    {
                        if(abs(physics_objects[i]->anti_mass + physics_objects[j]->anti_mass) < math_epsilon)
                        {
                            physics_objects[j]->model.position -= contact_normal/2.0f;
                            physics_objects[i]->model.position += contact_normal/2.0f;
					    }
                        else
                        {
                            float summ_antimass = physics_objects[j]->anti_mass + physics_objects[i]->anti_mass;
					        physics_objects[j]->model.position -= contact_normal * (physics_objects[j]->anti_mass / summ_antimass);
					        physics_objects[i]->model.position += contact_normal * (physics_objects[i]->anti_mass / summ_antimass);
                        }
                    }

                    double impulse = CalculateImpulse(contact_normal, physics_objects[j], physics_objects[i], contact_point);

                    if(physics_objects[i]->state != PO_STATIC)
						physics_objects[i]->ApplyImpulse(contact_point, contact_normal, -impulse);
					if(physics_objects[j]->state != PO_STATIC)	
						physics_objects[j]->ApplyImpulse(contact_point, contact_normal, impulse);

					physics_objects[i]->OnCollide(physics_objects[j]);
					physics_objects[j]->OnCollide(physics_objects[i]);
			    }
		    }
	    }

    for(int i = 0; i < physics_objects.size(); i++)
        physics_objects[i]->Tick(last_physics_update);

    last_physics_update = 0;
}