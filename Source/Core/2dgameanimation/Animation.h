#ifndef ANIMATION_H
#define ANIMATION_H

#include "../tinyxml/tinyxml.h"
#include <string>
#include <map>
#include "MovingPart.h"
#include "../Resources.h"

using namespace std;

class Animation : public Resource
{
public:
    string name;

    Animation();
    virtual ~Animation() { for(unsigned int i = 0; i < _bones.size(); ++i) delete _bones[i]; }

    Texture* texture;

	void SetModel(mat4 model, bool mirror);
	void Draw(float position);
	float Time()const;

    virtual bool Instantiate() { return true; }
    virtual void Free() {}
    virtual bool Load(string path) { return true; }
    void Load(TiXmlElement *xe, Texture *tex);

private:
    void AddBone(MovingPart *bone);
	float _time;
	vec2 _pivotPos;
	std::vector<MovingPart *> _bones;
	std::vector<MovingPart *> _renderList;
	mat4 _subPosition;
    
	vector<mat4> _matrixsStack;
    friend class MovingPart;
};

#endif//ANIMATION_H
