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
    PO_State state;

    RigidBody(double mass, PO_State pst);
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

public:

    Physics(): physics_objects() {}
    ~Physics() {}	

	void Update(double delta_time);
	double CalculateImpulse(vec2 normal, RigidBody *m1, RigidBody *m2, vec2 point);
    void RegisterRigidBody(RigidBody *rb) { physics_objects.push_back(rb); }
    void UnregisterRigidBody(RigidBody *rb) { std::remove(physics_objects.begin(), physics_objects.end(), rb); }
};

#endif //_PHYSICS_H_
