#include "Animation.h"
#include "../Resources.h"
#include "../Utils.hpp"
#include "../math/math3d.h"
#include "../Mobitech.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
using namespace std;

void AnimationClip::Load(TiXmlElement *xe, Texture *tex)
{
    texture = tex;
    time = fatof(xe->Attribute("time"));
    pivot_pos.x = fatof(xe->Attribute("pivotX"));
    pivot_pos.y = fatof(xe->Attribute("pivotY"));

    TiXmlElement *element = xe->FirstChildElement();
    while (element) 
    {
        bones.push_back(new MovingPart(this, element, texture->GetWidth(), texture->GetHeight()));
        element = element->NextSiblingElement();
    }
    sub_position = mat4_identity;
    sub_position *= GLTranslation(-pivot_pos.x, -pivot_pos.y, 0.0f);

    std::sort(render_list.begin(), render_list.end(), CmpBoneOrder);
}

void AnimationClip::SetModel(mat4 model, bool mirror) 
{
    sub_position = mat4_identity;
    sub_position *= GLTranslation(-pivot_pos.x, -pivot_pos.y, 0.0f);
    if (mirror) 
    {
        sub_position*= GLScale(-1.0f, 1.0f, 0.0f);
    }
    sub_position *= model;
}

void AnimationClip::Draw(float position) 
{
    Renderer::GetInstance()->BindTexture(texture, 0);

    matrix_stack.clear();
    for(unsigned int i = 0; i < render_list.size();i++) 
        render_list[i]->visible = false;

    matrix_stack.push_back(sub_position);
    for(unsigned int i = 0; i < bones.size();i++)
        bones[i]->ComputeTransform(position, matrix_stack);

    for(unsigned int i = 0; i < render_list.size();i++)
    {
        render_list[i]->ApplyTransform();
        render_list[i]->Draw();
    }
    //glBindTexture(GL_TEXTURE_2D, 0);
}

void AnimationClip::Draw(float position, float weight, AnimationState blend_clip)
{
    Renderer::GetInstance()->BindTexture(texture, 0);

    matrix_stack.clear();
    blend_clip.clip->matrix_stack.clear();

    for(unsigned int i = 0; i < render_list.size();i++) 
        render_list[i]->visible = false;
    
    matrix_stack.push_back(sub_position);
    blend_clip.clip->matrix_stack.push_back(sub_position);
    for(unsigned int i = 0; i < bones.size(); i++)
    {
        bones[i]->ComputeTransform(position, matrix_stack);
        blend_clip.clip->bones[i]->ComputeTransform(blend_clip.progress, blend_clip.clip->matrix_stack);
    }

    for(unsigned int i = 0; i < render_list.size();i++)
    {
        render_list[i]->transform = render_list[i]->transform * weight + blend_clip.clip->render_list[i]->transform * (1.0f - weight);
        render_list[i]->ApplyTransform();
        render_list[i]->Draw();
    }
    //glBindTexture(GL_TEXTURE_2D, 0);
}

float AnimationClip::Time() const 
{
    return time;
}

void AnimationClip::AddBone(MovingPart *bone) 
{
    render_list.push_back(bone);
}


bool Animation::Load(const string path)
{
    Texture *texture = dynamic_cast<Texture*>(resource_factory->Load((path.substr(0, path.length() - 3) + "png").c_str(), RT_TEXTURE));
    
    if (texture == NULL)
    {
        Logger::Message(LT_ERROR, "Cannot find animation atlas texture");
        return false;
    }
    texture->name = "texture";
    
    AssetFile file(resource_factory, path.c_str());

    unsigned int file_size = file.GetFileSize();
    char* buffer = new char[file_size];
    file.Read(buffer, sizeof(char), file_size);
    file.Close();
    
    TiXmlDocument doc;
    doc.Parse(buffer, 0, TIXML_ENCODING_UTF8);
    
    delete[] buffer;

    if (file_size > 0) 
    {
        TiXmlElement *root = doc.RootElement();
        TiXmlElement *animation = root->FirstChildElement("Animation");
        while(animation)
        {
            string id = string(animation->Attribute("id"));
            if(animation_clips.find(id) == animation_clips.end()) 
            {
                AnimationClip *animation_clip = new AnimationClip();
                animation_clip->name = id;
                animation_clip->Load(animation, texture);
                animation_clips[id] = animation_clip;
            }
            animation = animation->NextSiblingElement("Animation");
        }
        return true;
    }
    return false;
}

void Animation::Play(string name) 
{
    animation_states.clear();
        
    AnimationState state;
    state.name = name;
    state.progress = 0.0f;
    state.speed = 1.0f;
    state.start_time = Engine::GetTimeMS();
    state.weight = 1.0f;
    state.clip = GetAnimationClip(name);

    animation_states.push_back(state);
}