#include "Renderer.h"

vec3 Camera::GetPosition() const { return position; }
vec3 Camera::GetRotation() const { return rotation; }

mat4 Camera::GetProjection() const { return projection; }
mat4 Camera::GetView() const { return GLRotation(rotation.x, rotation.y, rotation.z) * GLTranslation(-position); }

void Camera::UpdateFrustum()
{
    frustumPlanes.extract(GetView(), GetProjection());
}

void Camera::Create(float x, float y, float z)
{
    position   = vec3(x, y, z);
    projection = mat4_identity;
}

void Camera::Perspective(float fov, float aspect, float zNear, float zFar)
{
    projection = GLPerspective(fov, aspect, zNear, zFar);
}

void Camera::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void Camera::Rotate(float x, float y, float z)
{
    rotation += vec3(x, y, z);
}

void Camera::Move( float x, float y, float z)
{
    vec3 move = transpose(mat3(GLRotation(rotation.x,rotation.y,rotation.z))) * vec3(x, y, z);
    //vec3 move = vec3(x,y,z);
    position += move;
}

void Camera::LookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
    rotation = GLToEuler(GLLookAt(position, center, up));
    this->position = position;
}
    
Camera::Camera(void){ rotation = vec3_zero; }
Camera:: ~Camera(void){}
