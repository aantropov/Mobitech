#include "Animation.h"
#include "../Resources.h"
#include "../Utils.hpp"
#include "../math/math3d.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
using namespace std;

void AnimationClip::Load(TiXmlElement *xe, Texture *tex)
{
    texture = tex;
    _time = fatof(xe->Attribute("time"));
    _pivotPos.x = fatof(xe->Attribute("pivotX"));
    _pivotPos.y = fatof(xe->Attribute("pivotY"));

    TiXmlElement *element = xe->FirstChildElement();
    while (element) 
    {
        _bones.push_back(new MovingPart(this, element, texture->GetWidth(), texture->GetHeight()));
        element = element->NextSiblingElement();
    }
    _subPosition = mat4_identity;
    _subPosition *= GLTranslation(-_pivotPos.x, -_pivotPos.y, 0.0f);

    std::sort(_renderList.begin(), _renderList.end(), CmpBoneOrder);
}

void AnimationClip::SetModel(mat4 model, bool mirror) 
{
    _subPosition = mat4_identity;
    _subPosition *= GLTranslation(-_pivotPos.x, -_pivotPos.y, 0.0f);
    if (mirror) 
    {
        _subPosition*= GLScale(-1.0f, 1.0f, 0.0f);
    }
    _subPosition *= model;
}

void AnimationClip::Draw(float position) 
{
    Renderer::GetInstance()->BindTexture(texture, 0);

    _matrixsStack.clear();
    for(unsigned int i = 0; i < _renderList.size();i++) 
        _renderList[i]->_visible = false;

    _matrixsStack.push_back(_subPosition);
    for(unsigned int i = 0; i < _bones.size();i++)
        _bones[i]->PreDraw(position, _matrixsStack);

    for(unsigned int i = 0; i < _renderList.size();i++)
        _renderList[i]->Draw();

    //glBindTexture(GL_TEXTURE_2D, 0);
}

float AnimationClip::Time() const 
{
    return _time;
}

void AnimationClip::AddBone(MovingPart *bone) 
{
    _renderList.push_back(bone);
}


bool Animation::Load(string path)
{
    Texture *texture = dynamic_cast<Texture*>(resource_factory->Load((path.substr(0, path.length() - 3) + "png").c_str(), RT_TEXTURE));
    
    if (texture == NULL)
    {
        Logger::Message(LT_ERROR, "Cannot find animation atlas texture");
        return false;
    }
    texture->name = "texture";

    TiXmlDocument doc(path.c_str());
    if (doc.LoadFile(TIXML_ENCODING_UTF8)) 
    {
        TiXmlElement *root = doc.RootElement();
        TiXmlElement *animation = root->FirstChildElement("Animation");
        while(animation)
        {
            string id = string(animation->Attribute("id"));
            if(animation_clips.find(id) == animation_clips.end()) 
            {
                AnimationClip *animation_clip = new AnimationClip();
                animation_clip->name = id;
                animation_clip->Load(animation, texture);
                animation_clips[id] = animation_clip;
            }
            animation = animation->NextSiblingElement("Animation");
        }
        return true;
     }
    return false;
}