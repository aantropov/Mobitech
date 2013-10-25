#pragma once
#include "..\Basic\UNode.h"
#include "..\Basic\UIAnimatable.h"

class UGameObject;

class UComponent : public UNode 
{
public:

    std::string name;
    UGameObject *gameObject;

    virtual void Update(double delta)
    {
        auto anim = dynamic_cast<UIAnimatable*>(this);
        if(anim != nullptr)
            anim->UpdateAnimation(delta);
    }

    UComponent(){}
    virtual ~UComponent(){}
};

