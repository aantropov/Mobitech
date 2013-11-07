#include "Animation.h"
#include "../Resources.h"
#include "../Utils.hpp"
#include "../math/math3d.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
using namespace std;

void Animation:: Load(TiXmlElement *xe, Texture *tex)
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

void Animation::SetModel(mat4 model, bool mirror) 
{
    _subPosition = mat4_identity;
    _subPosition *= GLTranslation(-_pivotPos.x, -_pivotPos.y, 0.0f);
    if (mirror) 
    {
        _subPosition*= GLScale(-1.0f, 1.0f, 0.0f);
    }
    _subPosition *= model;
}

void Animation::Draw(float position) 
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

float Animation::Time() const 
{
    return _time;
}

void Animation::AddBone(MovingPart *bone) 
{
    _renderList.push_back(bone);
}
