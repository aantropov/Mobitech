#pragma once
#include "unode.h"
#include "..\Renderer\urenderer.h"
#include "UIRenderable.h"
#include "..\Resources\UTexture.h"
#include "UCamera.h"
#include "..\GameMechanics\UGameMechanics.h"

class UResourceFactory;
class UModel;
class UShaderProgram;

class ULight : public UGameObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;

    vec3 spotDirection;
    float spotCosCutoff;
    float spotExponent;

    UModel* model;
    
    std::vector<UTexture*> depthTextures;
    std::vector<UCamera> cameras;

public:
    
    bool castShadows;

    std::vector<UTexture*> GetDepthTextures();
    std::vector<UCamera> GetCameras();

    vec4 GetAmbient(){ return ambient; }
    vec4 GetDiffuse(){ return diffuse; }
    vec4 GetSpecular(){ return specular; }
    vec3 GetAttenuation(){ return attenuation; }

    vec3 GetSpotDirection(){ return spotDirection; }
    float GetSpotCosCutoff(){ return spotCosCutoff; }
    float GetSpotExponent(){ return spotExponent; }

    void SetAmbient(vec4 v){ ambient = v; }
    void SetDiffuse(vec4 v){ diffuse = v; }
    void SetSpecular(vec4 v){ specular = v; }
    void SetAttenuation(vec3 v){ attenuation = v; }

    void SetSpotDirection(vec4 v){ spotDirection = v; }
    void SetSpotCosCutoff(float degrees){ spotCosCutoff = cosf(degrees*math_radians); }
    void SetSpotExponent(float v){ spotExponent = v; }

    virtual mat4 GetLightTransform();
    virtual void SetLightTransform(string light);
    virtual void SetShadowTexture(unsigned int location, int i);
    
    // Deprecated!
    virtual void SetShaderParameters(int numberOfLight);

    virtual void Update(double delta);

    void InitModel(UResourceFactory* rf);
    
    ULight();
    ULight(UResourceFactory* rf,  vec4 pos);
    ULight(UResourceFactory* rf,  vec4 pos, bool _castShadows);
    ~ULight(void);
};