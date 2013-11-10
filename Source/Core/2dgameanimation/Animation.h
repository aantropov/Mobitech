#ifndef ANIMATION_H
#define ANIMATION_H

#include "../tinyxml/tinyxml.h"
#include <string>
#include <map>
#include "MovingPart.h"
#include "../Resources.h"

using namespace std;

class MovingPart;
class AnimationClip
{
public:
    string name;

    AnimationClip() {}
    virtual ~AnimationClip() { for(unsigned int i = 0; i < _bones.size(); ++i) delete _bones[i]; }

    Texture* texture;

	void SetModel(mat4 model, bool mirror);
	void Draw(float position);
	float Time() const;
    void Load(TiXmlElement *xe, Texture *tex);

private:

    void AddBone(MovingPart *bone);
	float _time;
	vec2 _pivotPos;
	std::vector<MovingPart *> _bones;
	std::vector<MovingPart *> _renderList;
	mat4 _subPosition;
    
	vector<mat3> _matrixsStack;
    friend class MovingPart;
};

class Animation: public Resource
{
    map<string, AnimationClip*> animation_clips;

public:

    Animation() {}
    virtual ~Animation() {}

    AnimationClip *GetAnimationClip(string name) const { return animation_clips.at(name); }

    virtual bool Instantiate() { return true; }
    virtual void Free() 
    {  
        for (std::map<string, AnimationClip*>::iterator it = animation_clips.begin(); it != animation_clips.end(); ++it)
            delete it->second;
        animation_clips.clear();
    }

    virtual bool Load(const string path);
};


#endif//ANIMATION_H
