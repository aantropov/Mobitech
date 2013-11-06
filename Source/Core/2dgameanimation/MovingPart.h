#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include "../tinyxml/tinyxml.h"
#include "../math/math3d.h"
#include "MotionValues.h"

#define fatof(a) static_cast<float>(atof(a))

class Animation;
class MovingPart
{
public:

	~MovingPart();
	MovingPart(Animation *animation, TiXmlElement * xe, float width, float height);

	void PreDraw(float p, std::vector<Matrix>& stack);
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

	//
	// Этот блок использует типы движка HGE, подробности в начале файла
	//
	void CreateQuad(float width, float height, const std::string &texture);
    GLfloat _quad[16];
	vec2 _origin[4];
    friend bool CmpBoneOrder(MovingPart *one, MovingPart *two);
    friend class Animation;

};

bool CmpBoneOrder(MovingPart *one, MovingPart *two);

#endif//MOVINGPART_H