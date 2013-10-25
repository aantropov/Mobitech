#pragma once
#include "..\math\UMath.h"

class UVertex
{
    vec3 pos;    
    vec3 normal;    
    vec2 texcoord;
    vec3 binormal;
    
    vec4 bone_index;
    vec4 bone_weight;

public:

    static const int offsetPos = sizeof(vec3) + sizeof(vec3) + sizeof(vec2) + sizeof(vec3) + sizeof(vec4) + sizeof(vec4);

    void SetPosition(vec3 p){  pos = p; }
    vec3 GetPosition() const { return pos; }

    void SetNormal(vec3 p){  normal = p; }
    vec3 GetNormal() const { return pos; }
    
    void SetTexcoord(vec2 t){  texcoord = t; }
    vec2 GetTexcoord() const { return texcoord; }

    void SetBinormal(vec3 bn){  binormal = bn; }
    vec3 GetBinormal() const { return binormal; }

    void SetBoneIndex(vec4 bn){  bone_index = bn; }
    vec3 GetBoneIndex() const { return bone_index; }

    void SetBoneWeight(vec4 bn){  bone_weight = bn; }
    vec3 GetBoneWeight() const { return bone_weight; }

    UVertex(vec3 p): pos(p), normal(vec3_zero), texcoord(vec2_zero), binormal(vec3_zero) {}    
    UVertex(vec3 p, vec3 n): pos(p), normal(n), texcoord(vec2_zero), binormal(vec3_zero) {}    
    UVertex(vec3 p, vec3 n, vec2 tc): pos(p), normal(n), texcoord(tc), binormal(vec3_zero) {}    
    UVertex(vec3 p, vec3 n, vec2 tc, vec3 bin): pos(p), normal(n), texcoord(tc), binormal(bin) {}
    UVertex(vec3 p, vec3 n, vec2 tc, vec3 bin, vec4 b_id, vec4 b_w): pos(p), normal(n), texcoord(tc), binormal(bin), bone_index(b_id), bone_weight(b_w) {}
    UVertex(void);
    ~UVertex(void){}
};