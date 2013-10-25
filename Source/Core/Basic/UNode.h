#pragma once
#include "UIUpdateble.h"

class UNode : public UIUpdateble
{    
public:
    
    UNode();
    virtual void Update(double delta){};
    virtual void Free(){};
    virtual ~UNode();
};