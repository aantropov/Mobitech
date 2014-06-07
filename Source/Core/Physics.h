#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Resources.h"
#include "math\mathgl.h"
#include "math\bounds.h"
#include "math\transform.h"
//#include <hash_map>
#include <map>
#include "Utils.hpp"
#include <algorithm>

using namespace std;

class VertexBuffer;
class RigidBody
{
public:
   
    AABB aabb;
    ::transform model;

	vec2 velocity;
    double rotation;
    double mass;
    double inertion;
		
    double anti_mass;
    double anti_inertion;

    vec2 force_summ;
    double angular_force_summ;
    double elasticity;
		
    VertexBuffer* shape;
    PHYSICS_OBJECT_STATE state;

    //RigidBody() { }
    RigidBody(double mass, PHYSICS_OBJECT_STATE pst);
    virtual ~RigidBody();

    void UpdatePosition(double dt);
    void UpdateSpeed(double dt);
    void Tick(double dt);
    void ApplyImpulse(vec2 point, vec2 normal, double impulse);

    virtual void OnCollide(RigidBody* rigidbody) = 0;
};

class Physics : public Singleton<Physics>
{
    std::vector<RigidBody*> physics_objects;
    
    std::list<RigidBody*> register_queue;
    std::list<RigidBody*> unregister_queue;
    
    double CalculateImpulse(vec2 normal, RigidBody *m1, RigidBody *m2, vec2 point);    
    double last_physics_update;

public:
    
    double update_time;

    static Physics* GetInstance();

    Physics(): last_physics_update(0.0), update_time(0.001), physics_objects() {}
    ~Physics() {}

	void Update(double delta_time);	
    void RegisterRigidBody(RigidBody *rb) { register_queue.push_back(rb); }//physics_objects.push_back(rb); }
    void UnregisterRigidBody(RigidBody *rb) { unregister_queue.push_back(rb); }//std::remove(physics_objects.begin(), physics_objects.end(), rb); }
};

#endif //_PHYSICS_H_
