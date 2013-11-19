#include "MovingPart.h"
#include "Animation.h"
#include "../Utils.hpp"
#include "../Renderer.h"

float Read(TiXmlElement *xe, const char *name, const float defaultValue) 
{
	const char *tmp = xe->Attribute(name);
	return (tmp ? fatof(tmp) : defaultValue);
}

void MovingPart::CreateQuad(float width, float height, const std::string &texture) 
{
	std::string::size_type start = 0;
	std::string::size_type end = texture.find(':', start);
	float x = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float y = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float w = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float h = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float offX = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	float offY = fatof(texture.substr(start).c_str());
    
    memset(quad_data, 0, sizeof(quad_data));
    
	quad_data[2] = x / width;       
    quad_data[3] = y / height;
	quad_data[6] = (x + w) / width; 
    quad_data[7] = y / height;
    quad_data[10] = (x + w) / width; 
    quad_data[11] = (y + h) / height;
    quad_data[14] = x / width;       
    quad_data[15] = (y + h) / height;

	x = offX;
	y = offY;
	
    origin[0].x = x;     
    origin[0].y = y;
	origin[1].x = x + w;     
    origin[1].y = y;
	origin[2].x = x + w; 
    origin[2].y = y + h;
	origin[3].x = x; 
    origin[3].y = y + h;
}

MovingPart::~MovingPart() 
{
	for (unsigned int i = 0; i < bones.size(); ++i) 
		delete bones[i];
}

MovingPart::MovingPart(AnimationClip *animation, TiXmlElement *xe, float width, float height) : visible(false)
{
    animation->AddBone(this);

	bone_name =  xe->Attribute("name");

	const char *tmp = xe->Attribute("moving_type");
	if (tmp == NULL || strcmp(tmp, "spline") == 0)
		interpolation_type = MotionValues::M_SLINE;
	else if (strcmp(tmp, "line") == 0)
		interpolation_type = MotionValues::M_LINE;
	else
		interpolation_type = MotionValues::M_DISCONTINUOUS;
	
    order = atoi(xe->Attribute("order"));
    
    const char* cX = xe->Attribute("centerX");
    const char* cY = xe->Attribute("centerY");

    _center.x = cX != NULL ? fatof(cX) : 0;
	_center.y = cY != NULL ? fatof(cY) : 0;
	std::string texture = xe->Attribute("texture"); 

	CreateQuad(width, height, texture);

	TiXmlElement *pos = xe->FirstChildElement("pos");
	if (pos == NULL) 
    {
		_x.AddKey(0.f, 0.f);
		_y.AddKey(0.f, 0.f);
		_scaleX.AddKey(0.f, 1.f);
		_scaleY.AddKey(0.f, 1.f);
		_angle.AddKey(0.f, 0.f);
	}

	while (pos) 
    {
		float time = Read(pos, "time", 0.f);
		_x.AddKey(time, Read(pos, "x", 0.f));
		_y.AddKey(time, Read(pos, "y", 0.f));	
		_scaleX.AddKey(time, Read(pos, "scaleX", 1.f));
		_scaleY.AddKey(time, Read(pos, "scaleY", 1.f));
		_angle.AddKey(time, Read(pos, "angle", 0.f));

		pos = pos->NextSiblingElement("pos");
	}

	_x.SetType(interpolation_type);
	_y.SetType(interpolation_type);	
	_scaleX.SetType(interpolation_type);
	_scaleY.SetType(interpolation_type);
	_angle.SetType(interpolation_type);

	TiXmlElement *element = xe->FirstChildElement();
	while (element) 
    {
		std::string name = element->Value();
		
        if (name == "movingPart") 
        {
			bones.push_back(new MovingPart(animation, element, width, height));
		}
		element = element->NextSiblingElement();
	}
}

void MovingPart::ApplyTransform()
{
    vec2 quad_temp = transform * origin[0]; 
    quad_data[0] = quad_temp.x;
    quad_data[1] = quad_temp.y;
		
    quad_temp = transform * origin[1];
    quad_data[4] = quad_temp.x;
    quad_data[5] = quad_temp.y;

    quad_temp = transform * origin[2];
    quad_data[8] = quad_temp.x;
    quad_data[9] = quad_temp.y;

    quad_temp = transform * origin[3];
    quad_data[12] = quad_temp.x;
    quad_data[13] = quad_temp.y;
}

void MovingPart::ComputeTransform(float p, std::vector<mat3> &stack) 
{	
	float localT;
	int index = _x.Value(p, localT);
	if (visible = (index >= 0)) 
    {
        transform = stack.back();

        transform = GLTranslation(_x.GetFrame(index, localT), _y.GetFrame(index, localT)) * transform;
        transform = GLRotation(math_degrees * _angle.GetFrame(index, localT)) * transform;
		transform = GLScale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT)) * transform;

		transform = GLTranslation(-_center.x, -_center.y) * transform;
        stack.push_back(transform);

        transform = transpose(transform);	

		for (unsigned int i = 0; i < bones.size(); ++i)
			bones[i]->ComputeTransform(p, stack);
		
		stack.pop_back();
	}
}

void MovingPart::Draw() 
{
	if (visible) 
    {
        Renderer* render = Renderer::GetInstance();
        ShaderProgram* shader = render->GetCurrentShaderProgram();

        render->EnableBlend(BT_ALPHA_BLEND);
        // No culling of back faces
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_TRUE,  4 * sizeof(GLfloat), quad_data);
        glEnableVertexAttribArray(shader->attribute_locations.position);
        
        const float colors[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
        glVertexAttribPointer(shader->attribute_locations.color, 4, GL_FLOAT, GL_TRUE,  0, colors);
        glEnableVertexAttribArray(shader->attribute_locations.color);

        glVertexAttribPointer(shader->attribute_locations.texcoords, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), &(quad_data[2]));
        glEnableVertexAttribArray(shader->attribute_locations.texcoords);

        render->DrawArrays(GL_TRIANGLE_FAN, 0, 4);
        render->DisableBlend();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) 
{
    return one->order < two->order;
}
