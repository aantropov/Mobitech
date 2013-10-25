#pragma once

#include "UComponent.h"
#include "..\Math\UMath.h"

class UMaterial;

class UGameObject : public UNode
{
protected:
    std::vector<UComponent*> components;

public:
    
    std::string name;

    ::transform local;
    ::transform world;
    
    std::vector<UGameObject*> children;
    UGameObject *parentObject;

    void GetBounds() const;
    void AddComponent(UComponent *component);

    void Render(UMaterial *m);
    void Render(URENDER_TYPE type);
    virtual void Update(double delta);

    UGameObject();
    UGameObject(UComponent *component);
    ~UGameObject();    
};

