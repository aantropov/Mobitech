// Basic ofthe framework

#pragma once

#pragma warning (disable: 4356)

#include <string>
#include <stdio.h>
#include <windows.h>

using namespace std;

#include "Basic\UNode.h"
//#include "UGLObject.h"

#include "Renderer\UVertex.h"
#include "Renderer\UVertexBuffer.h"
#include "Renderer\UIndexBuffer.h"

#include "Resources\UResource.h"

#include "Renderer\UWindow.h"

#include "Utils\enum.h"

#include "Basic\URegistry.hpp"
#include "Basic\USingleton.hpp"
#include "Resources\UResourceFactory.h"

#include "Utils\ULogger.h"
#include "Resources\UXMLFile.h"
#include "Utils\UConfig.h"
#include "Renderer\UShader.h"
#include "Renderer\UShaderProgram.h"
#include "Resources\UTexture.h"

#include "math\umath.h"
#include "Basic\UCamera.h"

#include "Renderer\URenderer.h"
//#include "UPlane.h"
#include "Renderer\UMesh.h"
#include "Basic\UMatrix.h"
#include "Basic\UMatrixRotation.h"
#include "Basic\ULight.h"
#include "Resources\UMaterial.h"
#include "Resources\UModel.h"
#include "Renderer\UFrameBufferObject.h"
#include "Renderer\URenderManager.h"
#include "Basic\UIRenderable.h"

class UEngine 
{
    UScene* currentScene;
    URenderManager* renderManager;
	
    unsigned int fps;
    float elapsedTime;

public:
    
    static UResourceFactory  rf;

    void SetCurrentScene(UScene* _currentScene)
    {
        this->currentScene = _currentScene;
        URenderer::GetInstance()->SetCurrentScene(currentScene);
    }

    bool Initialize();
    void Run();
    void Stop();
    void Release();
};
