#pragma once
#include "UResource.h"
#include "..\Utils\enum.h"
#include "..\Utils\ExportDef.h"

class UShaderProgram;

class UResourceFactory 
{
protected:

    class UElement
    {
    protected:    

        std::string id;
        UResource* resource;

    public:

        UElement(UResource* r, std::string path) : resource(r), id(path) {}

        UResource* GetResource(){ return resource;}
        void SetResource( UResource* r ){ resource = r; }


        std::string GetId() { return id;}
        void SetId(std::string id) { this->id = id; }

        void Free(){ delete resource;}
        ~UElement() { Free(); }

    };

    vector<UElement*> resources;
    int unique_id;

public:

    UResourceFactory(){ unique_id = 0;}
    ~UResourceFactory(){ ReleaseAll(); }

    //Load resource from hard disk
    UResource* Get(std::string path);
    bool Add(std::string path, UResource* res);
    UResource *Load(std::string path, URESOURCE_TYPE type);
    UShaderProgram *Load(std::string vp, std::string pp);
    UResource *Create(URESOURCE_TYPE type);

    void Release(std::string path);    
    void Release(UResource *resource);
    void ReleaseAll();
};