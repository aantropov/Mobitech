#ifndef ANIMATION_H
#define ANIMATION_H

#include "../tinyxml/tinyxml.h"
#include <string>
#include <map>
#include "Matrix.h"
#include "MovingPart.h"

using namespace std;

class Animation
{
public:

    Texture* texture;

	void SetPos(const Point2D &pos, bool mirror);
	void Draw(float position);
	float Time()const;

private:
    void AddBone(MovingPart *bone);
	float _time;
	Point2D _pivotPos;
	std::vector<MovingPart *> _bones;
	std::vector<MovingPart *> _renderList;
	Matrix _subPosition;
    
	// этот тот самый вектор, который будет выполнять роль стека матриц
	vector<mat4> _matrixsStack;
    friend class MovingPart;
};

#endif//ANIMATION_H
