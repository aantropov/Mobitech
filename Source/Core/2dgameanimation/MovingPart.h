#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include <vector>
#include "../tinyxml/tinyxml.h"
#include "../math/math3d.h"
#include "../Utils.hpp"
#include "MotionValues.h"

using namespace std;

class AnimationClip;
class MovingPart
{
public:

    string bone_name;
	mat3 transform;

	~MovingPart();
	MovingPart(AnimationClip *animation, TiXmlElement * xe, float width, float height);

	void ComputeTransform(float interpolation, std::vector<mat3>& stack);
	void ApplyTransform();
    void Draw();

private:
	
    vec2 _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _scaleX;
	MotionValues _scaleY;
	MotionValues::Motion interpolation_type;

	std::vector<MovingPart*> bones;
	int order;
    bool visible;

	void CreateQuad(float width, float height, const std::string &texture);
    float quad_data[16];
	vec2 origin[4];
    friend bool CmpBoneOrder(MovingPart *one, MovingPart *two);
    friend class AnimationClip;
};

bool CmpBoneOrder(MovingPart *one, MovingPart *two);

#endif//MOVINGPART_H