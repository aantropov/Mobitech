#include "Animation.h"
#include "TextureLoad.h"

//
// ���������� - ������� ��� ��������� �������
// (�������� ��������� ������ �������� ��������, 
// �.�. �� ����� �������� ����� ���� ��������� ������ ��������)
//
Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

//
// ����������� - ��������� �������� �������� �� XML 
// ����� ������������ ����� ������,
// ������� ����� ���������� ���� � ������ ��������,
// ����� ����������� TinyXml
//
Animation::Animation(TiXmlElement *xe, GLuint hTexture, float width, float height)
{
	_time = fatof(xe->Attribute("time"));
	_pivotPos.x = fatof(xe->Attribute("pivotX"));
	_pivotPos.y = fatof(xe->Attribute("pivotY"));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		_bones.push_back(new MovingPart(this, element, width, height));
		element = element->NextSiblingElement();
	}
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
    _hTexture = hTexture;
    
    std::sort(_renderList.begin(), _renderList.end(), CmpBoneOrder);
}

//
// ���������� ��������� �������� �� ������, 
// "pos" - ��������� �������� �� ������, 
// ����� ���������� ��������� - ������ "mirror" � "true".
// ����� ����� ����� �������� ����� ������ ������� Draw()
// ���������� ����� ����, ���� ����� �������� �� ������ ����� ����
//
void Animation::SetPos(const Point2D &pos, bool mirror) {
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	if (mirror) {
		_subPosition.Scale(-1.f, 1.f);
	}
	_subPosition.Move(pos.x, pos.y);
}

//
// ���������� �������� �� ������ 
// "position" - �������� ������� ��� �������� �� ��������� [0, 1]
//
void Animation::Draw(float position) {
    glBindTexture(GL_TEXTURE_2D, _hTexture);

	_matrixsStack.clear();
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->_visible = false;
	}
    
	// ��� ������� ��������� ��� �������� ��������� ���� ���������� "����" ������
	_matrixsStack.push_back(_subPosition);
	for(unsigned int i = 0; i < _bones.size();i++) {
		_bones[i]->PreDraw(position, _matrixsStack);
	}
    
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->Draw();
	}
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

//
// ����� ������� ����� �������� 
// ��� ������� ����� ������������ � Update ��� ������� 
// �������� "position" ������ Draw
// ��������, ��� ���������� �������� ����� ������� ���:
// progress += deltaTime / anim->Time();
// if(progress >= 1.f) progress = 0.f;
//
float Animation::Time() const {
	return _time;
}

void Animation::AddBone(MovingPart *bone) {
    _renderList.push_back(bone);
}


/* ���������� ������� ��������� ��������� */

// ��������� ���� ��������
// � ������ ������ - ������ "true"
// ����� ��������� ��������� ������ - �������� "���������������" ��������
//
// ����� � ����� ������������ HGE ��� �������� � �������� �������
// ��� ����� ���� ����� �������� ��� ������������ �� ������ ������ 
//
bool AnimationManager::Load(const std::string &fileName) {
    
    float width;
    float height;
	
    const char * path = MakePath(fileName.c_str());
    std::string fullFileName = std::string(path);
	// ��������� �������� - ���������� ��������� �� ��� � "tex", ������� ������������ �����
	//
	// ������ ����� - ���� ��� ��� ������������ ���� ����������
//	HGE *hge = hgeCreate(HGE_VERSION);
	GLuint tex = TextureLoad((fullFileName.substr(0, fullFileName.length() - 3) + "png").c_str(), width, height);
	if (tex == 0) { // ���� �������� �� ������� - ��������� ��������, ���������� "false"
		return false;
	}
	// ����� �����
	TiXmlDocument doc;
	if (doc.LoadFile(path)) { // ���� ���� ���������� - ������ XML � ��������� ��������
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *animation = root->FirstChildElement("Animation");
		while (animation) {
			const char *id = animation->Attribute("id");
			// �������� - �� ���� � ����� �������� � ����������� id - ����� ��������� ������ ������
			if (_animations.find(id) == _animations.end()) { 
				_animations[id] = new Animation(animation, tex, width, height);
			}
			animation = animation->NextSiblingElement("Animation");
		}
		_textures.push_back(tex); 
        
        printf("file loaded %s\n", path);
		return true;
	} else { 
		// ���� �������� �� ������ ��� ��� �� ������� ���������
		// ��������� ��������, ���������� "false"
		//
		// ������ ����� - ���� ��� ��� ������������ ���� ����������
		//hge->Texture_Free(tex);
		// ����� �����

		printf("file not found: %s\n", path);
		return false;
	}
}

//
// ��������� ��� �������� � �������� �� ������
//
void AnimationManager::UnloadAll() {
	for (AnimationMap::iterator it = _animations.begin(); it != _animations.end(); it++) {
		delete (*it).second;
	}
    glDeleteTextures(_textures.size(), &(_textures[0]));
	_animations.clear();
	_textures.clear();
}

//
// �������� ��������� �� �������� �� animationId - ��� ��� �������� � ��������� 
// (����� ������ �������� delete ��� ���!)
//
Animation *AnimationManager::getAnimation(const std::string &animationId) {
	AnimationMap::iterator it_find = _animations.find(animationId);
	if (it_find != _animations.end()) {
		return (*it_find).second;
	}
	printf("animation %s not found\n", animationId.c_str());
	return NULL;
}

AnimationManager::AnimationMap AnimationManager::_animations;
std::vector<GLuint> AnimationManager::_textures;
