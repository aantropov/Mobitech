#ifndef ANIMATION_H
#define ANIMATION_H

#include "../tinyxml/tinyxml.h"
#include <string>
#include <map>
#include "MovingPart.h"
#include "../Resources.h"

using namespace std;

class MovingPart;
class AnimationState;
class AnimationClip
{
public:
    string name;

    AnimationClip() {}
    virtual ~AnimationClip() { for(unsigned int i = 0; i < bones.size(); ++i) delete bones[i]; }

    Texture* texture;

	void SetModel(mat4 model, bool mirror);
	void Draw(float position);
    void Draw(float position, float weight, AnimationState clip); 
	float Time() const;
    void Load(TiXmlElement *xe, Texture *tex);

private:

    void AddBone(MovingPart *bone);
	float time;
	vec2 pivot_pos;
	std::vector<MovingPart*> bones;
	std::vector<MovingPart*> render_list;
	mat4 sub_position;
    
	vector<mat3> matrix_stack;
    friend class MovingPart;
};

struct AnimationState
{
    string name;
    AnimationClip* clip;
    float speed;
    double start_time;
    float progress;
    float weight;
};

class Animation: public Resource
{
protected:
    map<string, AnimationClip*> animation_clips;
    list<AnimationState> animation_states;

public:

    Animation() {}
    virtual ~Animation() {}

    AnimationClip *GetAnimationClip(string name) const { return animation_clips.at(name); }

    void Play(string name);
    void Update()
    {
        for(int i = 0; i < animation_states.size(); i++)
        {
//            animation_states[i].name = "sd";
        }
    }

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
