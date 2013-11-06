#include "MovingPart.h"
#include "Animation.h"

//
// Прочитать атрибут их нода XML 
// если атрибут существует - он преобразуется к float,
// если нет - возвражается defaultValue
//
float Read(TiXmlElement *xe, const char *name, const float defaultValue) {
	const char *tmp = xe->Attribute(name);
	return (tmp ? fatof(tmp) : defaultValue);
}

// 
// Заполнение hgeQuad
// тут вычисляются текстурные координаты для спрайта,
// данные об его экранном размере читаются из строки "texture"
//
//
void MovingPart::CreateQuad(float width, float height, const std::string &texture) {

	// начало блока - этот код зависит от используемого движка

	// конец блока

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
    
    //{0,0, 1,0, 1,1, 0,1};
    memset(_quad, 0, sizeof(_quad));
#define m 4

	_quad[m * 0 + 2] = x / width;       _quad[m * 0 + 3] = y / height;
	_quad[m * 1 + 2] = (x + w) / width; _quad[m * 1 + 3] = y / height;
    _quad[m * 2 + 2] = (x + w) / width; _quad[m * 2 + 3] = (y + h) / height;
    _quad[m * 3 + 2] = x / width;       _quad[m * 3 + 3] = (y + h) / height;
	// конец блока

	x = offX;
	y = offY;
	_origin[0].x = x;     _origin[0].y = y;
	_origin[1].x = x + w;     _origin[1].y = y;
	_origin[2].x = x + w; _origin[2].y = y + h;
	_origin[3].x = x; _origin[3].y = y + h;
    
}

//
// Деструктор - освобождаем рекурсивно память
//
MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _bones.size(); ++i) 
		delete _bones[i];
}


//
// Конструктор - читаем данные спрайта из XML
// запоминаем текстуру, спрайты из которой рисуем
//
MovingPart::MovingPart(Animation *animation, TiXmlElement *xe, float width, float height)
: _visible(false)
{
    
    animation->AddBone(this);
    
	// начало блока - этот код зависит от используемого движка
	// конец блока

	boneName =  xe->Attribute("name");

	const char *tmp = xe->Attribute("moving_type");
	if (tmp == NULL || strcmp(tmp, "spline") == 0) {
		_movingType = MotionValues::M_SLINE;
	} else if (strcmp(tmp, "line") == 0) {
		_movingType = MotionValues::M_LINE;
	} else {
		_movingType = MotionValues::M_DISCONTINUOUS;
	}
	_order = atoi(xe->Attribute("order"));
	_center.x = fatof(xe->Attribute("centerX"));
	_center.y = fatof(xe->Attribute("centerY"));
	std::string texture = xe->Attribute("texture"); 

	CreateQuad(width, height, texture);

	//
	// вот это очень важный блок - тут читаются данные 
	// о положении подвижного спрайта на экране и его движении 
	// во времени
	//
	TiXmlElement *pos = xe->FirstChildElement("pos");
	if (pos == NULL) {
		_x.AddKey(0.f, 0.f);
		_y.AddKey(0.f, 0.f);
		_scaleX.AddKey(0.f, 1.f);
		_scaleY.AddKey(0.f, 1.f);
		_angle.AddKey(0.f, 0.f);
	}
	while (pos) {
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

	// 
	// если есть - создаем дочерние подвижные спрайты
	//
	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(animation, element, width, height) );
		}
		element = element->NextSiblingElement();
	}
}

//
// Выводим спрайт на экран
// положение во времени анимации - p, [ 0<= p <= 1 ]
// положение на экране - stack
//
void MovingPart::PreDraw(float p, std::vector<Matrix &stack) {
	
	float localT;
	int index = _x.Value(p, localT);

	// если функция определена для значения "p" - рисуем
	// тут проверка только для "_x" - т.к. у остальных функци движения область определения такая же
	if (_visible = (index >= 0)) {
		stack.push_back(stack.back());
		stack.back().Move(_x.GetFrame(index, localT), _y.GetFrame(index, localT));
		stack.back().Rotate(_angle.GetFrame(index, localT));
		stack.back().Scale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT));

		stack.back().Move(-_center.x, -_center.y);

		stack.back().Mul(_origin[0].x, _origin[0].y, _quad[m * 0], _quad[m * 0 + 1]);
		stack.back().Mul(_origin[1].x, _origin[1].y, _quad[m * 1], _quad[m * 1 + 1]);
		stack.back().Mul(_origin[2].x, _origin[2].y, _quad[m * 2], _quad[m * 2 + 1]);
		stack.back().Mul(_origin[3].x, _origin[3].y, _quad[m * 3], _quad[m * 3 + 1]);
        
		// конец блока

		for (unsigned int i = 0; i < _bones.size(); ++i)
			_bones[i]->PreDraw(p, stack);
		
		stack.pop_back();
	}
}

void MovingPart::Draw() {
	if (_visible) {
		// начало блока - этот код зависит от используемого движка
        
        glVertexPointer(2, GL_FLOAT, m*sizeof(GLfloat), _quad);
        glTexCoordPointer(2, GL_FLOAT, m*sizeof(GLfloat), &(_quad[2]));
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) {
    return one->_order < two->_order;
}
