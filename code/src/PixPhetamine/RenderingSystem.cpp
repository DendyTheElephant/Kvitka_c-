#include <PixPhetamine/RenderingSystem.h>
#include <DendyCommon/Logger.h>

#include <iostream>

PixPhetamine::CRenderingSystem::CRenderingSystem(bool isInDebugState):
m_IsInDebugState(isInDebugState),
m_IsRunning(true)
{
    LOG_CALLSTACK_PUSH(__FILE__,__LINE__,__PRETTY_FUNCTION__);

    // GLFW
    if (!glfwInit())
    {
        LOG_CRITICAL_ERROR("Failed to initialize GLFW");
    }

    // OpenGL
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // const char* FramebufferVertexShaderSource = R"(#version 460 core
    // layout (location = 0) in vec3 pos;
    // layout (location = 1) in vec2 uvs;
    // out vec2 UVs;
    // void main()
    // {
    //     gl_Position = vec4(2.0 * pos.x, 2.0* pos.y, 2.0*pos.z, 1.000);
    //     UVs = uvs;
    // })";

    m_pMainWindow = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Kvitka - V0.00.01", NULL, NULL);
    if (m_pMainWindow == nullptr)
    {
        LOG_CRITICAL_ERROR("Failed to open GLFW window");
    }
    glfwMakeContextCurrent(m_pMainWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CRITICAL_ERROR("Failed to initialize GL with Glad");
    }


    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    //glfwSetFramebufferSizeCallback(pMainWindow, _FramebufferSizeCallback);

    // Retrieve the GPU - OpenGL Current specs for the platform --> Log file
    std::cerr << "=============[ PixPhetamine log-file ]=========================" << std::endl;
    std::cerr << ">Graphics Successfully Initialized !" << std::endl;
    std::cerr << "OpenGL Info" << std::endl;
    std::cerr << "    Version: " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "     Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cerr << "   Renderer: " << glGetString(GL_RENDERER) << std::endl;
    int OpenGLVersionMajor, OpenGLVersionMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersionMinor);
    std::cerr << "     OpenGL: " << OpenGLVersionMajor << "." << OpenGLVersionMinor << std::endl;
    std::cerr << "    Shading: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cerr << "----------------------------------------------------------------" << std::endl;
    std::cerr << ">GPU Specifications for modern GLSL:" << std::endl;
    int uboBindings, uboSize, uboVertex, uboFragment, uboGeometry;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &uboBindings);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboSize);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &uboVertex);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &uboFragment);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &uboGeometry);
    std::cerr << "  Max uniform block bindings: " << uboBindings << std::endl;
    std::cerr << "      Max uniform block size: " << uboSize << std::endl;
    std::cerr << "    Max uniform block vertex: " << uboVertex << std::endl;
    std::cerr << "  Max uniform block fragment: " << uboFragment << std::endl;
    std::cerr << "  Max uniform block geometry: " << uboGeometry << std::endl;
    std::cerr << "----------------------------------------------------------------" << std::endl;
    std::cerr << ">Texture specifications:" << std::endl;
    int texUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texUnits);
    //GL_MAX_COLOR_ATTACHMENTS
    //
    std::cerr << "     Max texture image units: " << uboBindings << std::endl;
    std::cerr << "----------------------------------------------------------------" << std::endl;


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    /* ============================================== */
    /* Insert names of shaders to load in ShaderNames */
    /* ============================================== */
    m_ShaderNamesVec.push_back("basic");
    // m_ShaderNamesVec.push_back("postprocess");
    // m_ShaderNamesVec.push_back("downsampling");
    // m_ShaderNamesVec.push_back("blurH");
    // m_ShaderNamesVec.push_back("blurV");
    // m_ShaderNamesVec.push_back("rgbsplit");
    /* ============================================== */

    /* =========================================== */
    /* Insert names of meshes to load in MeshNames */
    /* =========================================== */
    //m_MeshNamesVec.push_back("lionhead");
    m_MeshNamesVec.push_back("Cube");
    m_MeshNamesVec.push_back("Quad");
    m_MeshNamesVec.push_back("Triangle");
    //m_MeshNamesVec.push_back("sphere");
    /* =========================================== */

    std::cout << "Loading Shaders" << std::endl;
    _LoadShaders();
    std::cout << "Loading Shaders [COMPLETE]" << std::endl;

    AssertOpenGLErrors();

    std::cout << "Loading Meshes" << std::endl;
    _LoadMeshes();
    std::cout << "Loading Meshes [COMPLETE]" << std::endl;

    AssertOpenGLErrors();

    m_ShaderMapByName["basic"]->DeclareUniformVariableName("u_ModelViewProjectionMatrix");
    m_ShaderMapByName["basic"]->DeclareUniformVariableName("u_Color");


    m_pMainCamera = new PixPhetamine::CCamera(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    

    // STACK_MESSAGE("Creation of FrameBuffers");
    // m_GBufferMS = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::WITH_DEPTH_MULTISAMPLED);
    // m_GBufferAA = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::WITH_DEPTH);
    // m_DownSampled = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
    // m_RGBSplitted = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::NORMAL);
    // m_BufferBlurPartial = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
    // m_BufferBlur = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
    // STACK_MESSAGE("Creation of FrameBuffers [COMPLETE]");

    // STACK_MESSAGE("Checking OpenGL errors");
    // Utility::UErrorHandler::checkOpenGLErrors();

    // STACK_MESSAGE("Initialisation of FrameBuffers");
    // // Texture attachment
    // m_GBufferMS->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // m_GBufferMS->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 1);
    // m_GBufferMS->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 2);

    // m_GBufferAA->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // m_GBufferAA->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 1);
    // m_GBufferAA->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 2);
    // m_GBufferAA->addTexture("blurredColorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 3);

    // m_DownSampled->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // m_RGBSplitted->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // m_BufferBlurPartial->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // m_BufferBlur->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
    // STACK_MESSAGE("Initialisation of FrameBuffers [COMPLETE]");

    // STACK_MESSAGE("Checking OpenGL errors");
    // Utility::UErrorHandler::checkOpenGLErrors();

    // STACK_MESSAGE("Setup of Post Process passes");
    // // Creation
    // m_DownSamplingPass = new CPostProcessPass(m_ShaderList["downsampling"], m_DownSampled);
    // m_BlurPassPartI = new CPostProcessPass(m_ShaderList["blurH"], m_BufferBlurPartial);
    // m_BlurPassPartII = new CPostProcessPass(m_ShaderList["blurV"], m_BufferBlur);
    // m_RGBSplitPass = new CPostProcessPass(m_ShaderList["rgbsplit"], m_RGBSplitted);
    // m_DeferredShadingPass = new CPostProcessPass(m_ShaderList["postprocess"]);
    // // Downsampling pass set-up
    // m_DownSamplingPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "image", 0);
    // m_DownSamplingPass->bindVariableName("scale");
    // // Blur pass I pass set-up
    // m_BlurPassPartI->bindTexture(m_DownSampled->getTexture("processedTexture"), "image", 0);
    // // Blur pass II pass set-up
    // m_BlurPassPartII->bindTexture(m_BufferBlurPartial->getTexture("processedTexture"), "image", 0);
    // // RGB Split pass set-up
    // m_RGBSplitPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "image", 0);
    // m_RGBSplitPass->bindVariableName("split");
    // // Deferred shading pass set-up
    // m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "color_map", 0);
    // m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("normalTexture"), "normal_map", 1);
    // m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("typeTexture"), "type_map", 2);
    // m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("blurredColorTexture"), "blurred_color_map", 3);
    // m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("depth_texture"), "depth_map", 4);
    // m_DeferredShadingPass->bindVariableName("sun_direction");
    // STACK_MESSAGE("Setup of Post Process passes [COMPLETE]");

    // STACK_MESSAGE("Checking OpenGL errors");
    // Utility::UErrorHandler::checkOpenGLErrors();

    // std::vector<std::string> skyboxTextures;
    // skyboxTextures.push_back("textures/skyboxRight.png");
    // skyboxTextures.push_back("textures/skyboxLeft.png");
    // skyboxTextures.push_back("textures/skyboxTop.png");
    // skyboxTextures.push_back("textures/skyboxBottom.png");
    // skyboxTextures.push_back("textures/skyboxFront.png");
    // skyboxTextures.push_back("textures/skyboxBack.png");
    // m_skyBox = new PixPhetamine::SceneRendering::CSkybox("textures/skybox");	

    // STACK_MESSAGE("Checking OpenGL errors");
    // Utility::UErrorHandler::checkOpenGLErrors();


    LOG_CALLSTACK_POP();
}

PixPhetamine::CRenderingSystem::~CRenderingSystem()
{
    //PixPhetamine::Display::shutdownSDL_GL(m_SDLWindow, m_GLContext);

    for (auto const &it_shaderName : m_ShaderNamesVec) {
        //delete m_ShaderMapByName[it_shaderName];
    }

    for (auto const &it_meshName : m_MeshNamesVec) {
        //delete m_MeshMap[it_meshName];
    }

    glfwDestroyWindow(m_pMainWindow);
    glfwTerminate();
}

void PixPhetamine::CRenderingSystem::_LoadShaders()
{
    LOG_CALLSTACK_PUSH(__FILE__,__LINE__,__PRETTY_FUNCTION__);

    for (auto const &ShaderName : m_ShaderNamesVec)
    {
        std::string VertexShaderFullPath = "shaders\\" + ShaderName + ".vs";//"G:\\DyCode\\Kvitka_Cpp\\shaders\\" + ShaderName + ".vs";
        std::string FragmentShaderFullPath = "shaders\\" + ShaderName + ".fs";
        m_ShaderMapByName[ShaderName] = new PixPhetamine::CShader(VertexShaderFullPath.c_str(), FragmentShaderFullPath.c_str());
    }

    LOG_CALLSTACK_POP();
}

void PixPhetamine::CRenderingSystem::_ReloadShaders()
{
    // STACK_TRACE;
    // for (auto const &it_shaderName : m_ShaderNames) {
    //     m_ShaderList[it_shaderName]->reload();
    // }
    // UNSTACK_TRACE;
}

void PixPhetamine::CRenderingSystem::_LoadMeshes()
{
    LOG_CALLSTACK_PUSH(__FILE__,__LINE__,__PRETTY_FUNCTION__);

    for (auto const &MeshName : m_MeshNamesVec)
    {
        if (MeshName == "Triangle")
        {
            m_MeshMapByName[MeshName] = new CMesh(MeshName, CMesh::BasicMeshes::Triangle);
            m_MeshMapByName[MeshName]->LoadToGPU();
        }
        else if (MeshName == "Cube")
        {
            m_MeshMapByName[MeshName] = new CMesh(MeshName, CMesh::BasicMeshes::Cube);
            m_MeshMapByName[MeshName]->LoadToGPU();
        }
        else if (MeshName == "Quad")
        {
            m_MeshMapByName[MeshName] = new CMesh(MeshName, CMesh::BasicMeshes::Quad);
            m_MeshMapByName[MeshName]->LoadToGPU();
        }
        else
        {
            // std::string vertexShader = "G:\\DyCode\\Kvitka_Cpp\\shaders\\" + MeshName + ".vs";
            // std::string fragmentShader = "G:\\DyCode\\Kvitka_Cpp\\shaders\\" + MeshName + ".fs";
            // m_ShaderMapByName[it_shaderName] = new PixPhetamine::CShader(vertexShader.c_str(), fragmentShader.c_str());
        }
    }
    
    LOG_CALLSTACK_POP();
}






void PixPhetamine::CRenderingSystem::Render()
{
    LOG_CALLSTACK_PUSH(__FILE__,__LINE__,__PRETTY_FUNCTION__);

    // m_secondTimer.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_IsInWireframe;
    
    /* =========================================================================================== */
    /* ==== Draw the Scene ======================================================================= */
    /* =========================================================================================== */
    
    // GLenum gBufferTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_DEPTH_ATTACHMENT };
    // PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["basic"]->id(), m_GBufferMS->getId(), gBufferTargets, 3);

    /* Draw LionHeads */
    //pxFloat type_fox[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

    //m_pMainCamera->SetPosition(glm::vec3(0.0f, V*100.0f, 1.0f));
    m_pMainCamera->SetPosition(glm::vec3(0.0f, 10.0f, 0.1f));
    m_pMainCamera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    m_ViewProjectionMatrix = m_pMainCamera->GetViewProjectionMatrix();

    static float V = 0;
    V = V+0.01f;
    if (V > 1.0f)
        V = 0.0f;

    

    //glEnable(GL_DEPTH_TEST);
    
    // glClear(GL_COLOR_BUFFER_BIT);

    CShader* pCurrentShader = m_ShaderMapByName["basic"];
    CMesh* pCurrentMesh = m_MeshMapByName["Cube"];

    glUseProgram(pCurrentShader->GetId());
    glBindVertexArray(pCurrentMesh->GetVAO());
    

    for (size_t i = 0; i < 1000; ++i)
    {
        glm::mat4 ModelMatrix = glm::mat4(1);
        //pxVec3f rotateY(0.0f, 1.0f, 0.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3((float)(i/100)*3.0f, 0.0f, (float)(i%100)*3.0f));
        //M = glm::rotate(M, 90.0f, rotateY);
        //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

        m_ModelViewProjectionMatrix = m_ViewProjectionMatrix * ModelMatrix;

        
        pCurrentShader->SendUniformVariable("u_ModelViewProjectionMatrix", m_ModelViewProjectionMatrix);
        pCurrentShader->SendUniformVariable("u_Color", glm::vec3(V,1.0f-V,i/1000.0));

        glDrawElements(GL_TRIANGLES, pCurrentMesh->GetTriangleCount(), GL_UNSIGNED_INT, (void *)0);
    }

    AssertOpenGLErrors();

    // OpenGL
    glfwSwapBuffers(m_pMainWindow);
    
    LOG_CALLSTACK_POP();
}


void PixPhetamine::CRenderingSystem::Render(IMesh* pMeshToRender)
{
    LOG_CALLSTACK_PUSH(__FILE__,__LINE__,__PRETTY_FUNCTION__);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_IsInWireframe;

    static float V = 0;
    V = V+0.001f;
    if (V > 1.0f)
        V = 0.0f;
    
    m_pMainCamera->SetPosition(glm::vec3(0.0f, 10.0f+V*1000.0f, 0.1f+V*2000.0f));
    m_pMainCamera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    m_ViewProjectionMatrix = m_pMainCamera->GetViewProjectionMatrix();

    

    CShader* pCurrentShader = m_ShaderMapByName["basic"];

    glUseProgram(pCurrentShader->GetId());
    glBindVertexArray(pMeshToRender->GetVAO());
    

    
    glm::mat4 ModelMatrix = glm::mat4(1);
    m_ModelViewProjectionMatrix = m_ViewProjectionMatrix * ModelMatrix;

        
    pCurrentShader->SendUniformVariable("u_ModelViewProjectionMatrix", m_ModelViewProjectionMatrix);
    pCurrentShader->SendUniformVariable("u_Color", glm::vec3(V,1.0f-V,0.0f));

    glDrawElements(GL_TRIANGLES, pMeshToRender->GetTriangleCount(), GL_UNSIGNED_INT, (void *)0);

    AssertOpenGLErrors();

    // OpenGL
    glfwSwapBuffers(m_pMainWindow);
    LOG_CALLSTACK_POP();
}




void PixPhetamine::CRenderingSystem::AssertOpenGLErrors()
{
    // if (m_IsInDebugState == false)
    //     return;
	GLenum status = glGetError();
	if (status != GL_NO_ERROR)
    {
		switch (status)
        {
		case GL_INVALID_ENUM:
            LOG_CRITICAL_ERROR("OpenGL error: GL_INVALID_ENUM");
			break;
		case GL_INVALID_OPERATION:
            LOG_CRITICAL_ERROR("OpenGL error: GL_INVALID_OPERATION");
			break;
		case GL_INVALID_VALUE:
            LOG_CRITICAL_ERROR("OpenGL error: GL_INVALID_VALUE");
			break;
		case GL_STACK_OVERFLOW:
            LOG_CRITICAL_ERROR("OpenGL error: GL_STACK_OVERFLOW");
			break;
		case GL_STACK_UNDERFLOW:
            LOG_CRITICAL_ERROR("OpenGL error: GL_STACK_UNDERFLOW");
			break;
		case GL_OUT_OF_MEMORY:
            LOG_CRITICAL_ERROR("OpenGL error: GL_OUT_OF_MEMORY");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
            LOG_CRITICAL_ERROR("OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		default:
            LOG_CRITICAL_ERROR("OpenGL error: Unknown error!");
			break;
		}
	}
}