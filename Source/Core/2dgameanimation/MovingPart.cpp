#include "MovingPart.h"
#include "Animation.h"
#include "../Utils.hpp"
#include "../Renderer.h"

float Read(TiXmlElement *xe, const char *name, const float defaultValue) {
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

MovingPart::MovingPart(Animation *animation, TiXmlElement *xe, float width, float height) : _visible(false)
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
	_center.x = fatof(xe->Attribute("centerX"));
	_center.y = fatof(xe->Attribute("centerY"));
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

void MovingPart::PreDraw(float p, std::vector<mat4> &stack) {
	
	float localT;
	int index = _x.Value(p, localT);
    const int m = 4;
	if (_visible = (index >= 0)) 
    {
		stack.push_back(stack.back());
        stack.back() *= GLTranslation(_x.GetFrame(index, localT), _y.GetFrame(index, localT), 0.0f);
        stack.back() *= GLRotationZ(math_degrees * _angle.GetFrame(index, localT));
		stack.back() *= GLScale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT), 1.0f);

		stack.back() *= GLTranslation(-_center.x, -_center.y, 1.0f);

		vec4 quad_temp = stack.back() * vec4(_origin[0].x, _origin[0].y, 0.0f, 0.0f); 
        _quad[m * 0] = quad_temp.x;
        _quad[m * 0 + 1] = quad_temp.y;
		
        quad_temp = stack.back() * vec4(_origin[1].x, _origin[1].y, 0.0f, 0.0f); 
        _quad[m * 1] = quad_temp.x;
        _quad[m * 1 + 1] = quad_temp.y;

        quad_temp = stack.back() * vec4(_origin[2].x, _origin[2].y, 0.0f, 0.0f); 
        _quad[m * 2] = quad_temp.x;
        _quad[m * 2 + 1] = quad_temp.y;

        quad_temp = stack.back() * vec4(_origin[3].x, _origin[3].y, 0.0f, 0.0f); 
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

        const int m = 4;
        glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_FALSE,  m * sizeof(GLfloat), _quad);
        glEnableVertexAttribArray(shader->attribute_locations.position);
        
        glVertexAttribPointer(shader->attribute_locations.texcoords, 2, GL_FLOAT, GL_FALSE, m * sizeof(GLfloat), &(_quad[2]));
        glEnableVertexAttribArray(shader->attribute_locations.texcoords);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) 
{
    return one->_order < two->_order;
}
