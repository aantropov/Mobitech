#include "../Core/Mobitech.h"
float vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 
                     1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f };
float texcoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

class Asteroid : public RigidBody
{
    IndexBuffer index_buffer;
    Texture* texture;
    ShaderProgram* shader;

public:
    
    Asteroid() : RigidBody(200.0f, PO_DYNAMIC)
    {
        shape = new VertexBuffer();
        unsigned int count = unirand(3, 7);
        shape->Create(count);
        float size = unirand(50.0f, 50.0f);
        float dispertion = 0.1f;

        for(int i = 0; i < count; i++)
        {            
            float angle = float(i) * (360.0f/float(count)) * math_radians;
            ((Vertex*)shape->GetPointer())[i].pos = vec3(cosf(angle) + unirand(-dispertion, dispertion), sinf(angle) + unirand(-dispertion, dispertion), 0.0f) * size;
            ((Vertex*)shape->GetPointer())[i].color = vec3_one;//vec3(unirand(0.5f, 1.0f), unirand(0.5f, 1.0f), unirand(0.5f, 1.0f));
            ((Vertex*)shape->GetPointer())[i].texcoord = vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle)* 0.5f + 0.5f);
        }
        shape->Instantiate();

        index_buffer.Create(count-2);
        index_buffer.Fill(GL_TRIANGLE_FAN);
        index_buffer.Instantiate();

        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\asteroid.png", RT_TEXTURE));

        OPENGL_CHECK_FOR_ERRORS();
    }
    
    void Draw()
    {
        Renderer *render = Renderer::GetInstance();
        
        render->SetupCameraForShaderProgram(shader, model.matrix());
        render->BindTexture(texture, 0);

        render->BindBuffer(shape);
        render->BindBuffer(&index_buffer);
        render->DrawBuffer(&index_buffer);
        render->UnbindBuffer(true);
        render->UnbindBuffer(false);
    }

    ~Asteroid() { delete shape; }

    virtual void OnCollide(RigidBody *body) {}
};

#define ASTEROIDS_COUNT 2
class GameScene : public Scene, IInputListener
{
    ShaderProgram* shader;
    ShaderProgram* background;

    vec4 colors[6];
    Texture* test_texture;

    bool touch_pressed;
    vec2 prev_mouse_pos;
    Camera camera;
    Camera test_camera;
    float angle;
    float background_rotation;
    Animation *test_animation;
    RenderTexture rt;
    BMFont* font;
    Texture *nebula_tile_1;
    Texture *nebula_tile_2;
    Texture *background_stars;

    Asteroid test_asteroid[ASTEROIDS_COUNT];

public:

    GameScene()
    {         
        angle = 0.0f;
        background_rotation = 0.0f;
        Renderer *render = Renderer::GetInstance();

        float h = 600.0f;
        float w = (h * render->GetWidth()) /render->GetHeight();
        camera.Create(-render->GetWidth()/2.0f, -render->GetHeight()/2.0f, 0.0f);
        camera.Ortho(0.0f, w, 0.0f, h, 0.0f, 1000.0f);
                
        test_camera.Create(0.0f, 0.0f, 0.0f);
        test_camera.Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
        
        touch_pressed = false;

        font = dynamic_cast<BMFont*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Fonts\\Font_plain.txt", RT_BM_FONT));
        test_animation = dynamic_cast<Animation*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Animations\\111.aml", RT_ANIMATION));
        shader = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        background = Engine::main_resource_factory.Load(ASSETS_ROOT + "Shaders\\background.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");

        test_texture = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\Noise.png", RT_TEXTURE));
        nebula_tile_1 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile1.png", RT_TEXTURE));
        nebula_tile_2 = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\nebula_tile2.png", RT_TEXTURE));
        background_stars = dynamic_cast<Texture*>(Engine::main_resource_factory.Load(ASSETS_ROOT + "Textures\\stars.png", RT_TEXTURE));
        Input::GetInstance()->Register(this);

        /*vertices[0] = vec2(0.0f, 0.5f);
        vertices[1] = vec2(-0.5f, -0.5f);
        vertices[3] = vec2(0.5f, -0.5f);*/

        colors[0] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[1] = vec4(0.7f, 1.0f, 1.0f, 1.0f);
        colors[2] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[3] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[4] = vec4(1.0f, 1.0f, 0.7f, 1.0f);
        colors[5] = vec4(1.0f, 1.0f, 1.0f, 1.0f);

        render->SetCurrentCamera(&camera);
          
        rt.Initialize(render->GetWidth(), render->GetHeight());
        rt.GetTexture()->name = "text";
        
        render->ClearColor(vec4(0.3f, 0.3f, 0.3f, 0.0f));

        for(int i = 0; i < ASTEROIDS_COUNT; i++)
        {
            float angle = float(i) * (360.0f/float(ASTEROIDS_COUNT)) * math_radians;

            //test_asteroid[i].velocity = -vec2(cosf(angle), sinf(angle)) * unirand(500.0f, 700.0f);
            //test_asteroid[i].rotation = 35.0f;
            //test_asteroid[i].model.rotation = GLRotationZ(79.0f);
            test_asteroid[i].model.position = vec3(cosf(angle), sinf(angle), 0.0f) * 300.0f;
            test_asteroid[i].elasticity = 0.0f;
        }

        test_asteroid[0].model.position = vec4_x * 30.0f;
        test_asteroid[1].model.position = vec4_x * (-30.0f);
    }

    ~GameScene() { Input::GetInstance()->Unregister(this); }

    virtual void Update(double dt)
    {
        angle += dt;
        if(angle >= 1.0f)
            angle = 0.0f;

        background_rotation += dt * 2.0f;//* 0.1f;
        if(background_rotation >= 360.0f)
            background_rotation = 0.0f;
    }
    // [res.x,res.y] - interval of projection Poly to Vector
vec3 Projection(vector<vec3> &poly, vec3 point, vec3& min_point, vec3& max_point){
	vec3 res;
    res.y = res.x = projection(poly[0], point);
    for(int i = 1; i < poly.size() ; i++){
		double temp = projection(poly[i], point);
		if(temp < res.x)
        {
			res.x = temp;
            min_point = poly[i];
        }
        if(temp > res.y)
        {
			res.y = temp;
            max_point = poly[i];
        }
	}
	return res;
}


    bool IntersectConvexShape(VertexBuffer* a, mat4 model_a, VertexBuffer* b, mat4 model_b, vec3 &contact_point, vec3 &contact_normal, vec3& p1, vec3& p2)
{
    Renderer *render = Renderer::GetInstance();
	// potential separating axis
	vector<vec3> psa; 

	vector<vec3> transformed_a;
    for(int i = 0; i< a->GetNum(); i++)
        transformed_a.push_back(model_a * ((Vertex*)a->GetPointer())[i].pos);
	
    vector<vec3> transformed_b;
    for(int i = 0; i< b->GetNum(); i++)
        transformed_b.push_back(model_b * ((Vertex*)b->GetPointer())[i].pos);
	
    for(int i = 0; i < transformed_a.size()- 1; i++)
    {
		vec3 temp = transformed_a[i+1] - transformed_a[i];
        psa.push_back(GLRotationZ(90.0f) * temp);
	}

    vec3 last = transformed_a[0] - transformed_a[transformed_a.size()-1];
	psa.push_back(normalize(GLRotationZ(90.0f) * last));
	
    for(int i = 0; i < transformed_b.size()- 1; i++)
    {
		vec3 temp = transformed_b[i+1] - transformed_b[i];
        psa.push_back(normalize(GLRotationZ(90.0f) * temp));
	}

    last = transformed_b[0] - transformed_b[transformed_b.size()-1];
	psa.push_back(GLRotationZ(90.0f) * last * 1.0);

	//check axies
	int min_index = -1;
	double min = 0.0;   

    vec3 min_max_points[4];

    for(int i = 0; i < psa.size(); i++)
    {
		vec3 r1 = Projection(transformed_a, psa[i], min_max_points[0], min_max_points[1]);
		vec3 r2 = Projection(transformed_b, psa[i], min_max_points[2], min_max_points[3]);
		
		if(r1.y < r2.x)
			return false;
		
        if(r2.y < r1.x)
			return false;
		
        if(r2.x < r1.y && r1.y < r2.y)
        {
			if((min_index == -1 && r1.y - r2.x > 0) || (min > r1.y - r2.x && r1.y - r2.x > 0))
            {
				min = r1.y - r2.x;
				min_index = i;
			}
		}

		if(r1.x < r2.y && r2.y < r1.y)
        {
			if((min_index == -1 && r2.y - r1.x < 0) || (min > r2.y - r1.x && r2.y - r1.x < 0))
            {
				min = r2.y - r1.x;
				min_index = i;
			}
		}		
	}

//    if(min_index == -1)
  //      return false;

    Projection(transformed_a, psa[min_index], min_max_points[0], min_max_points[1]);
    Projection(transformed_b, psa[min_index], min_max_points[2], min_max_points[3]);

    //result vector, lenght = intersection
    float contact_normal_lenght = fabs(min);
    contact_normal = normalize(psa[min_index]) * contact_normal_lenght;
    
	bool a_intersect = false;
	vector<vec3> *intersected_obj = &transformed_a;
	vector<vec3> *intersect_obj = &transformed_b;

    if(min_index >= transformed_a.size())
    {
		a_intersect = true;
		intersected_obj = &transformed_b;
		intersect_obj   = &transformed_a;
		min_index -= transformed_a.size();
	}
	
	// intersect edge
	p1 , p2;
	if(min_index == intersected_obj->size() - 1)
    {
        p1 = (*intersected_obj)[intersected_obj->size()-1];	
        p2 = (*intersected_obj)[0];	
    }
    else
    {
        p1 = (*intersected_obj)[min_index];	
        p2 = (*intersected_obj)[min_index+1];	
    }       

	// intersect point
	p2 = p2 - p1;
    double l = length(p2);
    float error = 999999.0f;
 
    for(int i = 0; i < 4; i++)
    {
        vec3 temp = min_max_points[i] - p1;
        double temp_proj = projection(temp, p2);
        double dist = fabs(distance_to_line(min_max_points[i], p1, p2));
        float current_error = fabs(dist - contact_normal_lenght);
        if(current_error <= error)
        {
            contact_point = min_max_points[i];
            error = current_error;
        }
    }
    
    return true;
}

    virtual void DrawFrame()
    {        
        Renderer *render = Renderer::GetInstance();

        mat4 model = mat4_identity;
        //model = GLRotationZ(angle);
        angle = clamp(angle, 0.0f, 1.0f);
        background_rotation = clamp(background_rotation, 0.0f, 360.0f);

        rt.Begin();
        render->Clear();
        
        render->SetCurrentCamera(&camera);
        render->BindShaderProgram(shader);
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        
        test_animation->GetAnimationClip("banana_level2")->SetModel(GLScale(1.0f, -1.0f, 1.0f), false);
        test_animation->GetAnimationClip("banana_level2")->Draw(angle);

        
        for(int i = 0; i < ASTEROIDS_COUNT; i++)
            test_asteroid[i].Draw();        
        render->SetupCameraForShaderProgram(shader, mat4_identity);
        vec3 a,b, p1, p2;
        IntersectConvexShape(test_asteroid[1].shape, (test_asteroid[1].model.matrix()),    test_asteroid[0].shape, (test_asteroid[0].model.matrix()), a, b, p1, p2);
                render->DebugDrawLine(a, a+b, vec3_x);

        render->EnableBlend(BT_ALPHA_BLEND);        
        //font->Print(0, 0, "ololo");
        render->DisableBlend();

        rt.End();
        
        render->SetCurrentCamera(&test_camera);
        render->BindShaderProgram(background);        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation));

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(nebula_tile_1, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);
        
        render->SetupCameraForShaderProgram(font->shader, GLScale(1.5f, 1.5f, 1.5f) * GLRotationZ(background_rotation * -2.0f));
        render->EnableBlend(BT_MULTIPLY);
        render->BindTexture(nebula_tile_2, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);

        render->BindShaderProgram(font->shader);
        render->SetupCameraForShaderProgram(font->shader, mat4_identity);
        render->EnableBlend(BT_ADDITIVE);
        render->BindTexture(background_stars, 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);

        render->EnableBlend(BT_ALPHA_BLEND);
        render->BindTexture(rt.GetTexture(), 0);
        render->DrawTriangles(vertices, colors, texcoords, 6);
        
        //font->Print(-300.0f, 0.0f, GLScale(1.0f/render->GetWidth(), 1.0f/render->GetHeight(), 1.0f), "This is a different font, centered.");
        render->DisableBlend();
    }

    virtual void OnTouchDown(int x, int y, unsigned int touch_id = 0) { touch_pressed = true; prev_mouse_pos = vec2((float)x, (float)y); }
    virtual void OnTouchUp(int x, int y, unsigned int touch_id = 0) 
    { 
        touch_pressed = false; 
    }

    virtual void OnMove(int x, int y, unsigned int touch_id = 0)
    {
        if(!touch_pressed)
            return;

        vec3 pos = camera.GetPosition();
        pos.x += prev_mouse_pos.x - x;
        pos.y -= prev_mouse_pos.y - y;
        camera.SetPosition(vec3(pos.x, pos.y, 0.0f));

        prev_mouse_pos = vec2((float)x, (float)y);
    }
};

void GameMain()
{
    Engine *engine = Engine::GetInstance();
    engine->Initialize();

    std::tr1::shared_ptr<GameScene> game_scene(new GameScene());
    engine->SetScene(game_scene);

    engine->Run();
}

