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

	~MovingPart();
	MovingPart(AnimationClip *animation, TiXmlElement * xe, float width, float height);

	void PreDraw(float p, std::vector<mat3>& stack);
	void Draw();

private:

	std::string boneName;
	typedef std::vector<MovingPart *> List;
	
	vec2 _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _scaleX;
	MotionValues _scaleY;
	MotionValues::Motion _movingType;

	List _bones;
	int _order;
    bool _visible;

	void CreateQuad(float width, float height, const std::string &texture);
    GLfloat _quad[16];
	vec2 _origin[4];
    friend bool CmpBoneOrder(MovingPart *one, MovingPart *two);
    friend class AnimationClip;
};

bool CmpBoneOrder(MovingPart *one, MovingPart *two);

#endif//MOVINGPART_H