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
    
    memset(_quad, 0, sizeof(_quad));
    
    const int m = 4;    
	_quad[m * 0 + 2] = x / width;       _quad[m * 0 + 3] = y / height;
	_quad[m * 1 + 2] = (x + w) / width; _quad[m * 1 + 3] = y / height;
    _quad[m * 2 + 2] = (x + w) / width; _quad[m * 2 + 3] = (y + h) / height;
    _quad[m * 3 + 2] = x / width;       _quad[m * 3 + 3] = (y + h) / height;

	x = offX;
	y = offY;
	
    _origin[0].x = x;     _origin[0].y = y;
	_origin[1].x = x + w;     _origin[1].y = y;
	_origin[2].x = x + w; _origin[2].y = y + h;
	_origin[3].x = x; _origin[3].y = y + h;
}

MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _bones.size(); ++i) 
		delete _bones[i];
}

MovingPart::MovingPart(AnimationClip *animation, TiXmlElement *xe, float width, float height) : _visible(false)
{
    animation->AddBone(this);

	boneName =  xe->Attribute("name");

	const char *tmp = xe->Attribute("moving_type");
	if (tmp == NULL || strcmp(tmp, "spline") == 0)
		_movingType = MotionValues::M_SLINE;
	else if (strcmp(tmp, "line") == 0)
		_movingType = MotionValues::M_LINE;
	else
		_movingType = MotionValues::M_DISCONTINUOUS;
	
    _order = atoi(xe->Attribute("order"));
    
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

	_x.SetType(_movingType);
	_y.SetType(_movingType);	
	_scaleX.SetType(_movingType);
	_scaleY.SetType(_movingType);
	_angle.SetType(_movingType);

	TiXmlElement *element = xe->FirstChildElement();
	while (element) 
    {
		std::string name = element->Value();
		
        if (name == "movingPart") 
        {
			_bones.push_back( new MovingPart(animation, element, width, height) );
		}
		element = element->NextSiblingElement();
	}
}

void MovingPart::PreDraw(float p, std::vector<mat3> &stack) {
	
	float localT;
	int index = _x.Value(p, localT);
    const int m = 4;
	if (_visible = (index >= 0)) 
    {
        mat3 transform = stack.back();

        transform = GLTranslation(_x.GetFrame(index, localT), _y.GetFrame(index, localT)) * transform;
        transform = GLRotation(math_degrees * _angle.GetFrame(index, localT)) * transform;
		transform = GLScale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT)) * transform;

		transform = GLTranslation(-_center.x, -_center.y) * transform;
        stack.push_back(transform);

        transform = transpose(transform);

		vec2 quad_temp = transform * _origin[0]; 
        _quad[m * 0] = quad_temp.x;
        _quad[m * 0 + 1] = quad_temp.y;
		
        quad_temp = transform * _origin[1];
        _quad[m * 1] = quad_temp.x;
        _quad[m * 1 + 1] = quad_temp.y;

        quad_temp = transform * _origin[2];
        _quad[m * 2] = quad_temp.x;
        _quad[m * 2 + 1] = quad_temp.y;

        quad_temp = transform * _origin[3];
        _quad[m * 3] = quad_temp.x;
        _quad[m * 3 + 1] = quad_temp.y;

		for (unsigned int i = 0; i < _bones.size(); ++i)
			_bones[i]->PreDraw(p, stack);
		
		stack.pop_back();
	}
}

void MovingPart::Draw() 
{
	if (_visible) 
    {
        Renderer* render = Renderer::GetInstance();
        ShaderProgram* shader = render->GetCurrentShaderProgram();

        render->EnableBlend(BT_ALPHA_BLEND);
        // No culling of back faces
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        const int m = 4;
        glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_TRUE,  m * sizeof(GLfloat), _quad);
        glEnableVertexAttribArray(shader->attribute_locations.position);
        
        glVertexAttribPointer(shader->attribute_locations.texcoords, 2, GL_FLOAT, GL_FALSE, m * sizeof(GLfloat), &(_quad[2]));
        glEnableVertexAttribArray(shader->attribute_locations.texcoords);

        render->DrawArrays(GL_TRIANGLE_FAN, 0, 4);
        render->DisableBlend();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) 
{
    return one->_order < two->_order;
}
