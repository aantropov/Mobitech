#pragma once
#include "..\math\umath.h"

class UCamera
{
    vec3 position;
    vec3 rotation;
    mat4 projection;

    frustum frustumPlanes;

public:

    void UpdateFrustum();

    vec3 GetPosition() const;
    vec3 GetRotation()  const;
    const frustum GetFrustum() const { return frustumPlanes; }

    void SetPosition(vec3 p) { position = p; }
    void SetRotation(vec3 r) { rotation = r; }
    
    mat4 GetProjection()  const;
    mat4 GetView()  const;

    void Create(float x, float y, float z);
    void Perspective(float fov, float aspect, float zNear, float zFar);
    void Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    void LookAt(const vec3 &position, const vec3 &center, const vec3 &up);        
    void Rotate(float x, float y, float z);
    void Move( float x, float y, float z);
        
    UCamera(void);
    ~UCamera(void);
};

