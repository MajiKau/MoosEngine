#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GL/wglew.h>

#include <glm.hpp>
#include <ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/intersect.hpp>

//#include <include/imgui/include/imgui.h>//GUI
#include <imgui_impl_glut.h>

#include "code/headers/Geometry.h"
#include "code/headers/RenderFunctions.h"
#include "code/headers/InputManager.h"

#include "code/headers/Pathfinding.h"
#include "code/headers/Follower.h"
#include "code/headers/Turret.h"

#include "code/headers/Quadtree.h"
#include "code/headers/Octree.h"

#include "code/headers/Vehicle.h"
#include "code/headers/Scene.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


Vector3f ToVec3(Point2 p)
{
    return{ p.x,0,p.y };
}

float totalTime = 0;

Point2 mousePos;
Point2 mousePosWorld; 
Point2 mouseDelta;

Point2 mouseLeft;
Point2 mouseRight;

int screenWidth = 1280;
int screenHeight = 720;
float screenRatio = screenWidth * 1.0f / screenHeight;

bool show_test_window = false;
bool show_another_window = false;
bool show_another_window2 = false;

InputManager* inputManager;

std::vector<Node*> nodes;

Node* startNode;
Node* endNode;

int selectedNode = 0;

float zoom = 100.0f;

BatchRenderer* renderer;

struct Wall
{
    Point2 v1;
    Point2 v2;
    Point2 v3;
    Point2 v4;
};
std::vector<Wall> walls;

glm::mat4 Projection;
glm::mat4 View;
glm::vec3 PlaneRot;

Material CustomMaterial;

//Pathfinding
std::vector<Follower*> followers;
std::vector<Turret*> turrets;

float score = 500.0f;
int followerAmount = 1000;
int followerUpdateIterator = 0;
int pathfindingUpdates = 50;
int nodenum = 1000;
int wallnum = 1000;

glm::vec3 cursor;
bool showTurret = false;
//

bool warped = false;
bool mouselock = true;

bool rotateLight = true;
float lightDistance = 200.0f;
int pLightID = 0;

bool renderPlane = false;

bool showdebug = false;

Vehicle tank;

Scene MainScene;

bool IsKeySpecial(unsigned char key)
{
    if (key == 127	//Delete
        || key == 8		//Backspace
        || key == 13	//Enter
        || key == 27	//Escape
        || key == 1		//ctrl - A
        || key == 3		//ctrl - C
        || key == 22	//ctrl - V
        || key == 24	//ctrl - X
        || key == 25	//ctrl - Y
        || key == 26	//ctrl - Z
        )
        return true;
    return false;
}
void Keyboard(unsigned char key, int x, int y)
{
    inputManager->_Press(key);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(key);

    if (IsKeySpecial(key))
        io.KeysDown[key] = true;

}
void KeyboardUp(unsigned char key, int x, int y)
{
    inputManager->_Release(key);

    if (IsKeySpecial(key))
    {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[key] = false;
    }
}
void KeyboardSpecial(int key, int x, int y)
{
    inputManager->_PressSpecial(key);

    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = true;

}
void KeyboardSpecialUp(int key, int x, int y)
{
    inputManager->_ReleaseSpecial(key);

    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;
}
void MouseMove(int x, int y)
{
    mouseDelta += {-x + screenWidth / 2.0f, -y + screenHeight / 2.0f};
    if (mouselock)
    {
        if (!warped)
        {
            glutWarpPointer(screenWidth / 2, screenHeight / 2);
            warped = true;
        }
        else
        {
            warped = false;
        }
    }
    //glutPostRedisplay();

    inputManager->_MousePos(x, y);

    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);

}
void Mouse(int button, int state, int x, int y)
{
    inputManager->_Mouse(button, state, x, y);

    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        io.MouseDown[0] = !state;
        break;
    case GLUT_MIDDLE_BUTTON:
        break;
    case GLUT_RIGHT_BUTTON:
        io.MouseDown[1] = !state;
        break;
    case 3:
        //printf("Scroll Up\n");
        break;
    case 4:
        //printf("Scroll Down\n");
        break;
    default:
        printf("Unknown Mouse Button:%d\n", button);
        break;
    }
}

// Initialize OpenGL and ImGui
void init()
{
    glClearColor(0.447f, 0.565f, 0.604f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGLUT_Init();

    ImGui::StyleColorsDark();

}

void drawGUI()
{
	ImGui_ImplGLUT_NewFrame(screenWidth, screenHeight);
    // Show a simple window
    {
        ImGui::Value("Money:", score);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("Debug"))
            showdebug ^= 1;
        if (showdebug)
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::Value("MouseX:", inputManager->GetMousePosition()[0]);
            ImGui::Value("MouseY:", inputManager->GetMousePosition()[1]);

            ImGui::InputInt("PointLight", &pLightID, 1);

            ImGui::SliderFloat("Light.R", &(renderer->m_point_light[pLightID].Color.r), 0, 1);
            ImGui::SliderFloat("Light.G", &(renderer->m_point_light[pLightID].Color.g), 0, 1);
            ImGui::SliderFloat("Light.B", &(renderer->m_point_light[pLightID].Color.b), 0, 1);

            ImGui::SliderFloat("Light.Ambient", &(renderer->m_point_light[pLightID].AmbientIntensity), 0, 1);
            ImGui::SliderFloat("Light.Diffuse", &(renderer->m_point_light[pLightID].DiffuseIntensity), 0, 1);

            ImGui::SliderFloat("Light.X", &(renderer->m_point_light[pLightID].Position.x), -200, 200);
            ImGui::SliderFloat("Light.Y", &(renderer->m_point_light[pLightID].Position.y), -200, 200);
            ImGui::SliderFloat("Light.Z", &(renderer->m_point_light[pLightID].Position.z), -200, 200);

            ImGui::SliderFloat("LightD.R", &(renderer->m_directional_light.Color.r), 0, 1);
            ImGui::SliderFloat("LightD.G", &(renderer->m_directional_light.Color.g), 0, 1);
            ImGui::SliderFloat("LightD.B", &(renderer->m_directional_light.Color.b), 0, 1);

            ImGui::SliderFloat("LightD.Ambient", &(renderer->m_directional_light.AmbientIntensity), 0, 1);
            ImGui::SliderFloat("LightD.Diffuse", &(renderer->m_directional_light.DiffuseIntensity), 0, 1);

            ImGui::SliderFloat("LightD.X", &(renderer->m_directional_light.Direction.x), -1, 1);
            ImGui::SliderFloat("LightD.Y", &(renderer->m_directional_light.Direction.y), -1, 1);
            ImGui::SliderFloat("LightD.Z", &(renderer->m_directional_light.Direction.z), -1, 1);

            ImGui::SliderFloat("Shininess", &(CustomMaterial.shininess), 0, 1024);

            ImGui::Checkbox("Rotate Light", &rotateLight);
            ImGui::SliderFloat("Distance", &lightDistance, 0, 200);

            if (renderer->Blinn)
            {
                if (ImGui::Button("Blinn-Phong"))
                {
                    renderer->Blinn = false;
                }
            }
            else
            {
                if (ImGui::Button("Phong"))
                {
                    renderer->Blinn = true;
                }
            }

        }
    }

	ImGui::Render();
}
void CalculateNodeNeighbours()
{
    for each (auto node in nodes)
    {
        node->neighbours.clear();
        for each (auto node2 in nodes)
        {
            if (node == node2) continue;
            if (DistanceSquared({ node->x,node->y }, { node2->x,node2->y }) < 400)
            {
                LineSegment2 line(node->x, node->y, node2->x, node2->y);
                bool intersect = false;
                for each (Wall wall in walls)
                {
                    if (DoLineSegmentsIntersect(line, LineSegment2(wall.v1,wall.v2)))
                    {
                        intersect = true;
                    }
                    else if (DoLineSegmentsIntersect(line, LineSegment2(wall.v2, wall.v3)))
                    {
                        intersect = true;
                    }
                    else if (DoLineSegmentsIntersect(line, LineSegment2(wall.v3, wall.v4)))
                    {
                        intersect = true;
                    }
                    else if (DoLineSegmentsIntersect(line, LineSegment2(wall.v4, wall.v1)))
                    {
                        intersect = true;
                    }
                }
                if (!intersect)
                    node->AddNeighbour(node2);
            }
        }
    }
}

Node* RandomEdgeNode()
{
    int wh = (int)sqrt(nodenum);
    int x;
    int y;
    int s = rand() % 4;
    switch (s)
    {
    case 0:
        x = rand() % 10;
        y = rand() % (wh - 1);
        break;
    case 1:
        x = wh - rand() % 10;
        y = rand() % (wh - 1);
        break;
    case 2:
        x = rand() % wh;
        y = rand() % 10;
        break;
    case 3:
        x = rand() % wh;
        y = (wh - 1) - rand() % 10;
        break;
    default:
        printf("Error 4");
        break;
    }
    return nodes[y*wh + x];
}

void reset()
{
    score = 500.0f;
    walls.clear();
    int s = (int)sqrt(wallnum);
    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < s; j++)
        {
            Point2 p1((float)(i * 40 - 20 * s), (float)(j * 40 - 20 * s));
            Point2 p2(p1.x + (float)(rand() % 40 + 0), p1.y + (float)(rand() % 10 - 5));
            Point2 p3(p2.x + (float)(rand() % 10 - 5), p2.y + (float)(rand() % 40 + 0));
            Point2 p4(p3.x + (float)(rand() % 40 - 40), p3.y + (float)(rand() % 10 - 5));
            Wall wall;
            wall.v1 = p1;
            wall.v2 = p2;
            wall.v3 = p3;
            wall.v4 = p4;
            walls.push_back(wall);
        }
    }
	for (size_t i = 0; i < nodes.size(); i++)
	{
		delete nodes[i];
	}
    nodes.clear();
    s = (int)sqrt(nodenum);

    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < s; j++)
        {
            Node* newNode = new Node((float)(j * 10 - 5 * s), (float)(i * 10 - 5 * s));
            nodes.push_back(newNode);//TODO Check if inside a wall!
        }
    }

    CalculateNodeNeighbours();

    startNode = nodes.front();
    endNode = nodes.back();
    selectedNode = 0;

	for (size_t i = 0; i < turrets.size(); i++)
	{
		delete turrets[i];
	}
    turrets.clear();

	for (size_t i = 0; i < followers.size(); i++)
	{
		delete followers[i];
	}
    followers.clear();
    for (int i = 0; i < followerAmount; i++)
    {
        Follower* fol = new Follower();
        int j = rand() % nodes.size();
        Node* randomnode = RandomEdgeNode();
        fol->x = randomnode->x;
        fol->y = randomnode->y;
        fol->m_target = randomnode;

        followers.push_back(fol);
    }
}

void changeSize(int w, int h) 
{
    if (h == 0)
        h = 1;

	float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);

	screenWidth = w;
	screenHeight = h;
	screenRatio = ratio;
	printf("W:%d,H:%d,Ration:%.2f\n", screenWidth, screenHeight, ratio);
}

Octree<Cuboid> test_octree(0, { -500,-500,-500,1000,1000,1000 });
std::vector<Cuboid> test_objects;

Quadtree<Follower> follower_quadtree(0, { -500,-500,1000,1000 });

Entity* fish_entity;
Entity* frame_entity;
Entity* platform_entity;
Entity* left_entity;
Entity* right_entity;

Entity* tank_entity;
Entity* tank_rb_entity;
Entity* tank_bot_entity;
Entity* tank_top_entity;

void GameInit()
{
	renderer = new BatchRenderer(zoom, screenRatio);

    renderer->Camera.Position = { 0,40,0 };
    renderer->Camera.Rotation = { 0,0,0 };

    PointLight light;
    light.AmbientIntensity = 0.05f;
    light.Color = { 1,1,0 };
    light.DiffuseIntensity = 0.25f;
    light.Position = { 200,100,200 };
    
    CustomMaterial.shininess = 256.0f;

    renderer->m_point_light.push_back(light);

    reset();    

	for (int x = -5; x < 5; x++)
	{
		for (int y = -5; y < 5; y++)
		{
			for (int z = -5; z < 5; z++)
			{
				test_objects.push_back({ x * 100.0f - rand() % 10,y * 100.0f - rand() % 10,z * 100.0f - rand() % 10,3,6,4 });
			}
		}
	}
	Animation anim1("anim1");
	anim1.AddKeyFrame(0.0f, Pose(glm::vec3(-30, 20, 0), glm::quat()));
	anim1.AddKeyFrame(8.0f, Pose(glm::vec3(0.0f, 20.0f, 0.0f), glm::quat()));
	anim1.AddKeyFrame(1.6f, Pose(glm::vec3(30, 20, 0), glm::quat()));
	anim1.AddKeyFrame(26.0f, Pose(glm::vec3(40.0f, 40.0f, 40.0f), glm::quat()));
	anim1.AddKeyFrame(4.3f, Pose(glm::vec3(-30, 25, 0), glm::quat()));
	anim1.AddKeyFrame(15.1f, Pose(glm::vec3(-30, 10, 0), glm::quat()));
	anim1.SetLooping(true);

	Animation platform("platform");
	platform.AddKeyFrame(0.0f, Pose(glm::vec3(4, -20, 0), glm::quat()));
	platform.AddKeyFrame(1.5f, Pose(glm::vec3(4, -20, 0), glm::quat()));
	platform.AddKeyFrame(2.0f, Pose(glm::vec3(0, -20, 0), glm::quat()));
	platform.AddKeyFrame(3.0f, Pose(glm::vec3(0, -20, 0), glm::quat()));
	platform.AddKeyFrame(5.0f, Pose(glm::vec3(0, 0, 0), glm::quat()));
	//platform.SetLooping(true);

	Animation left("left");
	left.AddKeyFrame(0.0f, Pose(glm::vec3(0, 0, 0), glm::quat()));
	left.AddKeyFrame(3.0f, Pose(glm::vec3(0, 0, 0), glm::quat()));
	left.AddKeyFrame(5.0f, Pose(glm::vec3(-5, 0, 0), glm::quat()));
	//left.SetLooping(true);

	Animation right("right");
	right.AddKeyFrame(0.0f, Pose(glm::vec3(0, 0, 0), glm::quat()));
	right.AddKeyFrame(3.0f, Pose(glm::vec3(0, 0, 0), glm::quat()));
	right.AddKeyFrame(5.0f, Pose(glm::vec3(5, 0, 0), glm::quat()));
	//right.SetLooping(true);

	Animation tank("tank");
	tank.AddKeyFrame(0.0f, Pose(glm::vec3(0, 1, 0), glm::quat()));
	tank.AddKeyFrame(6.0f, Pose(glm::vec3(0, 1, 0), glm::quat()));
	tank.AddKeyFrame(9.0f, Pose(glm::vec3(0, 1, 20), glm::quat()));


	fish_entity = MainScene.SpawnEntity();
	fish_entity->SetLocalPosition({ 0,30,0 });

	frame_entity = fish_entity->SpawnChild();
	frame_entity->AddMesh("v_platform_frame");
	frame_entity->SetLocalPosition({ 0,0.5f,0 });

	platform_entity = fish_entity->SpawnChild();
	platform_entity->AddMesh("v_platform_left");
	platform_entity->AddMesh("v_platform_right");
	platform_entity->AddAnimation(anim1);
	platform_entity->AddAnimation(platform);

	left_entity = fish_entity->SpawnChild();
	left_entity->AddMesh("v_platform_left");
	left_entity->AddAnimation(left);

	right_entity = fish_entity->SpawnChild();
	right_entity->AddMesh("v_platform_right");
	right_entity->AddAnimation(right);
	//e2->SetLocalPose(glm::translate(glm::vec3(0.0f, 20.0f, 0.0f))*glm::rotate(PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)));

	tank_rb_entity = platform_entity->SpawnChild();
	tank_entity = tank_rb_entity->SpawnChild();
	//tank_entity->AddMesh("fulltank");
	tank_entity->AddAnimation(tank);
	tank_entity->SetLocalPosition({ 0,1,0 });
	tank_bot_entity = tank_entity->SpawnChild();
	tank_bot_entity->AddMesh("tank_bottom");
	tank_top_entity = tank_bot_entity->SpawnChild();
	tank_top_entity->AddMesh("tank_top");
	tank_top_entity->SetLocalRotation(glm::rotate(glm::quat(1,0,0,0), -10.1f, { 0,1,0 }));


}


void renderScene(void)
{
    Projection = glm::perspective(45.0f, (float)screenWidth / screenHeight, 0.1f, 1000.f);
    glm::vec4 front(0, 0, -1, 1);
    glm::vec4 right(1, 0, 0, 1);
    glm::vec4 up(0, 1, 0, 1);
    
    glm::mat4 rotation = glm::eulerAngleY(renderer->Camera.Rotation.x)*glm::eulerAngleX(renderer->Camera.Rotation.y);
    front = rotation * front;
    right = rotation * right;
    up = rotation * up;

    renderer->Camera.Forward = front;
    renderer->Camera.Right = right;
    renderer->Camera.Up = up;

    View = glm::lookAt(glm::vec3{ renderer->Camera.Position.x, renderer->Camera.Position.y, renderer->Camera.Position.z }, glm::vec3{ renderer->Camera.Position.x + front.x, renderer->Camera.Position.y + front.y, renderer->Camera.Position.z + front.z }, glm::vec3{ 0.0f,1.0f,0.0f });
    renderer->m_projection = Projection;
    renderer->m_view = View;


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rendering

	MainScene.Render(renderer);

	test_octree.Render(renderer);
	follower_quadtree.Render(renderer);

	//Draw all nodes as Tetrahedrons
	for each (auto node in nodes)
	{
		Color3f nodecolor{ 1 - 10000 / node->H, 1 - 10000 / node->H, 1 - 10000 / node->H };
        
        //renderer->RenderRegularTetrahedron({ node->x,-2,node->y }, 2, nodecolor);
	}

	//Draw Lines to show connections between nodes
	/*float distance;
	for each (auto node in nodes)
	{
		Color3f nodecolor{ 1 - 10000 / node->H, 1 - 10000 / node->H, 1 - 10000 / node->H };

		for each (auto node2 in node->neighbours)
		{
			LineSegment2 line(node->x, node->y, node2->x, node2->y);
			distance = DistanceSquared(mousePosWorld, ClosestPointOnLineSegment(mousePosWorld, line));
			Color3f color{ 1,1,1 };
			if (distance > 10000.0f)
			{
				color.r = 1 - (1.0f / 10000.0f) * (distance - 10000.0f);
				color.b = 0;
				color.g = 0;
			}
			else if (distance > 2500)
			{
				color.g = 1 - (1.0f / 10000.0f) * (distance - 2500.0f);
				color.b = 0;
			}
			else
			{
				color.b = 1 - (1.0f / 2500.0f) * (distance);
			}

			renderer->RenderLine(node->x, node->y, node2->x, node2->y, color);
		}
	}*/

    for each (Follower* fol in followers)
    {
        fol->Render(renderer);
    }  
    for each (Turret* tur in turrets)
    {
        tur->Render(renderer);
    }    

    //Render walls
    if (walls.size() != 0)
    {
        for (int i = 0; i < walls.size(); i ++)
        {
            std::vector<glm::vec3> points = { ToVec3(walls[i].v1),ToVec3(walls[i].v2),ToVec3(walls[i].v3),ToVec3(walls[i].v4) };
            renderer->RenderExtrudedShape(points, 4, RED);
        }
    }

    if (rotateLight)
    {
        renderer->m_point_light[0].Position.x = lightDistance * sinf(totalTime / 500.0f);
        renderer->m_point_light[0].Position.z = lightDistance * cosf(totalTime / 500.0f);

        renderer->m_point_light[1].Position.x = lightDistance * sinf(PI + totalTime / 500.0f);
        renderer->m_point_light[1].Position.z = lightDistance * cosf(PI + totalTime / 500.0f);
    }

    Material DefaultMaterial;
    DefaultMaterial.ambient = { 1.0f,1.0f,1.0f };//Currently does nothing
    DefaultMaterial.diffuse = { 0.5f,0.5f,0.5f };//Currently does nothing
    DefaultMaterial.specular = { 1.0f,1.0f,1.0f };//Currently does nothing
    DefaultMaterial.shininess = 32.0f;

    Material material = DefaultMaterial;

    glm::mat4 modelMat = glm::translate(glm::vec3(0, 100, -20)); 
	renderer->RenderMesh("fulltank", modelMat, DefaultMaterial);


    /*renderer->RenderMesh("dragon", modelMat, DefaultMaterial);
    //
    modelMat = glm::translate(glm::vec3(20, 100, 0))*glm::rotate(totalTime / 800.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    renderer->RenderMesh("dragon", modelMat, DefaultMaterial);

    modelMat = glm::translate(glm::vec3(0, 100, 0))*glm::rotate(totalTime / 800.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    material.shininess = 128.0f;
    renderer->RenderMesh("dragon", modelMat, material);

    modelMat = glm::translate(glm::vec3(-20, 100, 0))*glm::rotate(totalTime / 800.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    material.shininess = 256.0f;
    renderer->RenderMesh("dragon", modelMat, material);

    modelMat = glm::translate(glm::vec3(-40, 100, -0))*glm::rotate(totalTime / 800.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    renderer->RenderMesh("dragon", modelMat, CustomMaterial);

    modelMat = glm::translate(glm::vec3(-60, 100, 0))*glm::scale(glm::vec3(100, 100, 100));
    renderer->RenderMesh("bunny", modelMat, DefaultMaterial);

    modelMat = glm::translate(glm::vec3(-80, 100, 0));
    renderer->RenderMesh("airboat", modelMat, DefaultMaterial);

    modelMat = glm::translate(glm::vec3(-100, 100, 0));
    renderer->RenderMesh("cruiser", modelMat, DefaultMaterial);

    modelMat = glm::rotate(totalTime/800, glm::vec3(0.0f, 1.0f, 0.0f))*glm::translate(glm::vec3(0, 40+10*sinf(totalTime/1000), 40))*glm::scale(glm::vec3(5, 5, 5));
    renderer->RenderMesh("f16", modelMat, DefaultMaterial);*/

    if (renderPlane)
    {
        PlaneRot = glm::lerp(PlaneRot, renderer->Camera.Rotation, 0.1f);
        modelMat = glm::translate(renderer->Camera.Position + 2.0f*renderer->Camera.Forward - 0.75f*renderer->Camera.Up)*glm::rotate(PlaneRot.x + PI / 2.0f, renderer->Camera.Up)*glm::rotate(PlaneRot.y, renderer->Camera.Right)*glm::rotate(-PlaneRot.z, renderer->Camera.Forward);
        renderer->RenderMesh("f16", modelMat, DefaultMaterial);
    }

	modelMat = glm::translate(glm::vec3(0, 100, 0));
	renderer->RenderMesh("v_platform_full", modelMat, DefaultMaterial);
	modelMat = glm::translate(glm::vec3(-10, 100, 0));
	renderer->RenderMesh("v_platform_left", modelMat, DefaultMaterial);
	modelMat = glm::translate(glm::vec3(10, 100, 0));
	renderer->RenderMesh("v_platform_right", modelMat, DefaultMaterial);

    //Render Player
	tank.Render(renderer);
    /*modelMat = glm::translate(glm::vec3(renderer->Camera.Position.x, 2, renderer->Camera.Position.z))*glm::rotate(renderer->Camera.Rotation.x - PI / 2.0f, glm::vec3(0, 1, 0));
    renderer->RenderMesh("fulltank", modelMat, DefaultMaterial);*/

    /*modelMat = glm::translate(glm::vec3(0, 100, 20));
    renderer->RenderMesh("Cube", modelMat, CustomMaterial);
    modelMat = glm::translate(glm::vec3(0, 100, 40))*glm::scale(glm::vec3(4));
    renderer->RenderMesh("Cube", modelMat, DefaultMaterial);
    modelMat = glm::translate(glm::vec3(0, 100, 60))*glm::scale(glm::vec3(8));
    renderer->RenderMesh("Cube", modelMat, DefaultMaterial);
    modelMat = glm::translate(glm::vec3(0, 100, 80))*glm::scale(glm::vec3(16))*glm::rotate(totalTime / 5000.0f, glm::vec3(1.0f, 0.0f, 0.0f))*glm::rotate(totalTime / 3000.0f, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(totalTime / 2000.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    renderer->RenderMesh("Cube", modelMat, DefaultMaterial);*/

    GLenum err = glGetError();
    if (GLEW_OK != err)
    {
        printf("%s\n", glewGetErrorString(err));
    }

    //Render light source
    for each (auto light in renderer->m_point_light)
    {
        renderer->RenderRegularTetrahedron(light.Position, 3, Color3f(light.Color.x, light.Color.y, light.Color.z));
    }
    //renderer->RenderRegularTetrahedron(renderer->m_point_light[0].Position, 3, Color3f(renderer->m_point_light[0].Color.x, renderer->m_point_light[0].Color.y, renderer->m_point_light[0].Color.z));
    //renderer->RenderRegularTetrahedron(renderer->m_point_light[1].Position, 3, Color3f(renderer->m_point_light[1].Color.x, renderer->m_point_light[1].Color.y, renderer->m_point_light[1].Color.z));

    modelMat = glm::translate(glm::vec3(0.0f, -2.0f, 0.0f));
    renderer->RenderMesh("flatplane", modelMat, CustomMaterial);

	modelMat = glm::translate(glm::vec3(0, -2.0f, 0))*glm::scale(glm::vec3(1000,1,1000));
	renderer->RenderMesh("Cube", modelMat, DefaultMaterial);

    /*modelMat = glm::translate(glm::vec3(0.0f, 90.0f, 0.0f));
    renderer->RenderMesh("flatplane", modelMat, CustomMaterial);*/

    if (showTurret)
    {
        //renderer->RenderRegularTetrahedron(cursor, 1.0f, BLUE);
        renderer->RenderCircleHollow({ cursor.x,cursor.z }, 10, BLUE);
    }
    else
    {
        //renderer->RenderRegularTetrahedron(cursor, 0.5f, GREEN);
		renderer->RenderRegularTriangle({ cursor.x,cursor.z }, 0.5f,0, GREEN);
    }

    modelMat = glm::translate(renderer->Camera.Position)*glm::rotate(PI, glm::vec3(0, 1, 0))*glm::scale(glm::vec3(1000));
    renderer->RenderMesh("Skybox", modelMat, DefaultMaterial);

	renderer->Render();
    err = glGetError();
    if (GLEW_OK != err)
    {
        printf("%s\n", glewGetErrorString(err));
    }

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	drawGUI();
	glPopAttrib();
	glPopMatrix();
    glutSwapBuffers(); 
	glutPostRedisplay();

	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

    /*GLint nTotalMemoryInKB = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
        &nTotalMemoryInKB);

    GLint nCurAvailMemoryInKB = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
        &nCurAvailMemoryInKB);
    printf("Total:%dKB , Available:%dKB\n", nTotalMemoryInKB, nCurAvailMemoryInKB);*/

}
bool PlaceTurretAt(Vector3f pos)
{
    for each (auto tur in turrets)
    {
        float distance = glm::distance(tur->m_position ,pos);
        if (distance < 10.0f)
        {
            return false;
        }
    }
    Turret* turret = new Turret(pos, 0.01f);
    turrets.push_back(turret);
    return true;
}



void game()
{
	//UPDATES
	float deltaTime = glutGet(GLUT_ELAPSED_TIME) - totalTime;
	deltaTime /= 1000.0f;
	if (deltaTime > 1.0f)
	{
		deltaTime = 1.0f;
	}

	MainScene.Update(deltaTime);

	tank.Update(deltaTime);

	totalTime = (float)glutGet(GLUT_ELAPSED_TIME);

	//PATHFINDING
	Node* target = nodes.front();
	float distance = 1000;
	for each (Node* node in nodes)
	{
		float distance2 = DistanceSquared({ renderer->Camera.Position.x, renderer->Camera.Position.z }, { node->x,node->y });
		if (distance2 < distance)
		{
			target = node;
			distance = distance2;
		}
	}
	std::vector<Node*> new_path;
	for (int i = 0; i < pathfindingUpdates; i++)
	{
		Follower* fol = followers[followerUpdateIterator];

		followerUpdateIterator++;
		if (followerUpdateIterator >= followerAmount)followerUpdateIterator = 0;

		new_path = DoPathFinding(fol->m_target, target);
		if (new_path.size() != 0)
			fol->SetPath(new_path);


	}
	//Turrets damage followers
	for each (Follower* fol in followers)
	{
		for each (Turret* tur in turrets)
		{
			distance = Distance({ tur->m_position.x,tur->m_position.z }, { fol->x,fol->y });
			if (distance < 30)
			{
				fol->Damage(tur->m_damage*(float)deltaTime);
			}

		}
	}

	//INPUTS
	if ((mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) && mouselock)
	{
		renderer->Camera.Rotation.x += mouseDelta.x*0.001f;
		renderer->Camera.Rotation.y += mouseDelta.y*0.001f;
	}

	float dist = 1;;
	if (glm::intersectRayPlane(renderer->Camera.Position, renderer->Camera.Forward, glm::vec3(0, 4.5f, 0), glm::vec3(0, 1, 0), dist))
	{
		cursor = renderer->Camera.Position + renderer->Camera.Forward * dist;
	}
	else
	{
		cursor = renderer->Camera.Position;
		cursor.y = 4.5f;
	}

	//Escape -> Exit
	if (inputManager->IsKeyDown(27))
		glutExit();

	if (inputManager->IsKeyPressed('r'))
		reset();

	//TODO:
	if (inputManager->IsKeyPressed('m'))
	{
		left_entity->PlayAnimation("left");
		right_entity->PlayAnimation("right");
		platform_entity->PlayAnimation("platform");
		tank_entity->PlayAnimation("tank");
	}


	if (inputManager->IsKeyPressed('n'))
		platform_entity->PlayAnimation("anim1");

	if (inputManager->IsKeyPressed('b'))
	{
		right_entity->PlayAnimation("");
		left_entity->PlayAnimation("");
		platform_entity->PlayAnimation("");
		tank_entity->PlayAnimation("");
	}
	if (inputManager->IsKeyPressed('i'))
	{
		tank_rb_entity->EnableRigidbody();
	}
	if (inputManager->IsKeyPressed('o'))
	{
		tank_rb_entity->DisableRigidbody();
		tank_rb_entity->SetLocalPosition({ 0,0,0 });
	}

	glm::vec3 dir(0.0f, 0.0f, 0.0f);
	if (inputManager->IsSpecialDown(GLUT_KEY_UP))
	{
		dir.z = 1.0f;
	}
	else if (inputManager->IsSpecialDown(GLUT_KEY_DOWN))
	{
		dir.z = -1.0f;
	}
	if (inputManager->IsSpecialDown(GLUT_KEY_LEFT))
	{
		dir.x = 1.0f;
	}
	else if (inputManager->IsSpecialDown(GLUT_KEY_RIGHT))
	{
		dir.x = -1.0f;
	}
	if (dir.x != 0.0f || dir.z != 0.0f)
	{
		dir = glm::normalize(dir);
	}
	if (inputManager->IsKeyDown('q'))
	{
		tank_top_entity->Rotate(glm::rotate(glm::quat(1, 0, 0, 0), 2.0f*deltaTime, { 0,1,0 }));
	}
	else if (inputManager->IsKeyDown('e'))
	{
		tank_top_entity->Rotate(glm::rotate(glm::quat(1, 0, 0, 0), -2.0f*deltaTime, { 0,1,0 }));
	}
	dir *= 10.0f;
	tank_rb_entity->GetRidigbody()->SetVelocity({ dir.x, tank_rb_entity->GetRidigbody()->GetVelocity().y, dir.z });

	//Turret placing
    /*if (inputManager->IsKeyDown('q') || inputManager->IsMouseDown(GLUT_LEFT_BUTTON))
    {
        showTurret = true;
    }
    else
    {
        showTurret = false;
    }

    if (inputManager->IsKeyReleased('q') || inputManager->IsMouseReleased(GLUT_LEFT_BUTTON))
    {
        if (score >= 100.0f)
        {
            if (PlaceTurretAt(cursor))
            {
                score -= 100.0f;
            }
        }
    }*/

	//Place test objects into octree
	/*if (inputManager->IsKeyDown('e'))
	{
		float w = rand() % 4 + 1.0f;
		float h = rand() % 4 + 1.0f;
		float d = rand() % 4 + 1.0f;
		Vector3f pos = renderer->Camera.Position + 5.0f*renderer->Camera.Forward;
		test_objects.push_back({ pos.x - w / 2.0f, pos.y - h / 2.0f, pos.z - d / 2.0f,w,h,d });
		//test_objects.push_back({ cursor.x-w/2.0f,cursor.z-h/2.0f,w,h });
	}*/
	if (inputManager->IsKeyPressed('t'))
	{
		float w = rand() % 4 + 1.0f;
		float h = rand() % 4 + 1.0f;
		float d = rand() % 4 + 1.0f;
		Vector3f pos = renderer->Camera.Position + 5.0f*renderer->Camera.Forward;
		test_objects.push_back({ pos.x - w / 2.0f, pos.y - h / 2.0f, pos.z - d / 2.0f,w,h,d });
		//test_objects[0] = { cursor.x - w / 2.0f,cursor.z - h / 2.0f,w,h };
	}
	if (inputManager->IsKeyPressed('y'))
	{
		printf("%d\n", (int)test_objects.size());
	}

	test_octree.clear();
	for (int i = 0; i < test_objects.size(); i++)
	{
		test_octree.insert(&test_objects[i]);
	}

	follower_quadtree.clear();
	for (int i = 0; i < followers.size(); i++)
	{
		follower_quadtree.insert(followers[i]);
	}

    /*if (inputManager->IsMousePressed(GLUT_LEFT_BUTTON))
    {
        for each (Follower* fol in followers)
        {
            float distance = Distance({ renderer->Camera.Position.x, renderer->Camera.Position.z }, { fol->x,fol->y });
            if (distance < 30)
            {
                fol->Damage(20.0f);
            }
        }
    }*/

    //Movement
    glm::vec3 Forward = glm::normalize(glm::vec3(renderer->Camera.Forward.x, 0, renderer->Camera.Forward.z));
    //glm::vec3 Forward = glm::normalize(renderer->Camera.Forward);
    glm::vec3 Right = glm::normalize(glm::cross(glm::vec3(renderer->Camera.Forward.x, 0, renderer->Camera.Forward.z), glm::vec3(0, 1, 0)));

    float speed = 100.0f;
    if (inputManager->IsMouseDown(GLUT_RIGHT_BUTTON))
    {
        speed = 200.0f;
    }
	if (GetAsyncKeyState(0x57))
	{
		renderer->Camera.Position += Forward * speed * (float)deltaTime;
	}
	if (GetAsyncKeyState(0x53))
	{
		renderer->Camera.Position -= Forward * speed * (float)deltaTime;
	}
	if (GetAsyncKeyState(0x41))
	{
		renderer->Camera.Position -= Right * speed * (float)deltaTime;
	}
	if (GetAsyncKeyState(0x44))
	{
		renderer->Camera.Position += Right * speed * (float)deltaTime;
	}
	
	if (GetAsyncKeyState(0x57))//W
	{
		tank.Move(0);
	}
	else if (GetAsyncKeyState(0x53))//S
	{
		tank.Move(1);
	}
	else
	{
		tank.Move(2);
	}
	if (GetAsyncKeyState(0x41))//A
	{
		tank.Move(3);
	}
	else if (GetAsyncKeyState(0x44))//D
	{
		tank.Move(4);
	}
	else
	{
		tank.Move(5);
	}
	if (GetAsyncKeyState(0x51))//Q
	{
		tank.Move(6);
	}
	else if (GetAsyncKeyState(0x45))//E
	{
		tank.Move(7);
	}
	else
	{
		tank.Move(8);
	}

	if (GetAsyncKeyState(0x48))//H
	{

	}
	if (GetAsyncKeyState(0x4A))//J
	{

	}
	if (GetAsyncKeyState(0x4B))//K
	{

	}

	/*glm::mat4 rotation = glm::eulerAngleY(renderer->Camera.Rotation.x)*glm::eulerAngleX(renderer->Camera.Rotation.y);
	renderer->Camera.Position = glm::vec4(tank.GetPosition(),1.0f) + rotation * glm::translate(glm::vec3(0,0,10))* glm::vec4(0,0,0,1);*/

    /*if (inputManager->IsKeyDown('w'))
    {
        renderer->Camera.Position += Forward*speed;
    }
    if (inputManager->IsKeyDown('s'))
    {
        renderer->Camera.Position -= Forward*speed;
    }
    if (inputManager->IsKeyDown('a'))
    {
        renderer->Camera.Position -= Right*speed;
    }
    if (inputManager->IsKeyDown('d'))
    {
        renderer->Camera.Position += Right*speed;
    }*/

    /*if (inputManager->IsMouseDown(GLUT_MIDDLE_BUTTON))
    {
        for each (auto fol in followers)
        {
            if (DistanceToLine({ fol->x,fol->y }, Line2(renderer->Camera.Position.x, renderer->Camera.Position.z, renderer->Camera.Position.x + renderer->Camera.Forward.x, renderer->Camera.Position.z + renderer->Camera.Forward.z)) < 1)
            {
                fol->Damage(20.0f);
            }
        }
    }*/
    if (inputManager->IsMousePressed(3))//Scroll up
    {
        renderer->Camera.Position.y += 1.0f;
        zoom -= 5.0f;
    }
    if (inputManager->IsMousePressed(4))//Scroll down
    {
        renderer->Camera.Position.y -= 1.0f;
        zoom += 5.0f;
    }
    
    //Camera rotation for numpad
    if (inputManager->IsKeyDown('5'))
        renderer->Camera.Rotation.y += 0.02f;
    if (inputManager->IsKeyDown('2'))
        renderer->Camera.Rotation.y -= 0.02f;
    if (inputManager->IsKeyDown('3'))
        renderer->Camera.Rotation.x -= 0.02f;
    if (inputManager->IsKeyDown('1'))
        renderer->Camera.Rotation.x += 0.02f;

    if (renderer->Camera.Rotation.y > PI / 2.0f - 0.1f)
        renderer->Camera.Rotation.y = PI / 2.0f - 0.1f;
    if (renderer->Camera.Rotation.y < -PI / 2.0f + 0.1f)
        renderer->Camera.Rotation.y = -PI / 2.0f + 0.1f;

    //Follower updates
    for each (Follower* fol in followers)
    {
        int res = fol->Update((float)deltaTime);
        if (res == 1)//Dead
        {
            score += 1.0f;
            Node* randomnode = RandomEdgeNode();
            fol->x = randomnode->x;
            fol->y = randomnode->y;
            fol->m_target = randomnode;            
        }
       
        else if (res == 2)//No path
        {
            float distance = Distance({ renderer->Camera.Position.x, renderer->Camera.Position.z }, { fol->x,fol->y });
            if (distance < 10)
            {
                score -= 1.0f;
                Node* randomnode = RandomEdgeNode();
                fol->x = randomnode->x;
                fol->y = randomnode->y;
                fol->m_target = randomnode;
            }
            else
            {
                fol->Damage(-0.1f);
            }
        }
        else if (res == 3)//Dead from being stuck
        {
            Node* randomnode = RandomEdgeNode();
            fol->x = randomnode->x;
            fol->y = randomnode->y;
            fol->m_target = randomnode;
        }
    }
    
	mousePos.x = (float)inputManager->GetMousePosition()[0];
	mousePos.y = (float)inputManager->GetMousePosition()[1];
	mousePosWorld.x = 2*screenRatio * zoom*((float)(mousePos.x - (screenWidth / 2)) / screenWidth);
	mousePosWorld.y = -2*zoom*((float)mousePos.y / screenHeight - 0.5f);



    if (inputManager->IsKeyPressed('f'))
    {
        mouselock = !mouselock;
        mouseDelta = { 0,0 };
    }

    if (inputManager->IsSpecialReleased(GLUT_KEY_F11))
    {
        glutFullScreenToggle();
    }

    mouseDelta = { 0,0 }; //TODO remove
    inputManager->_Clear();
}




int main(int argc, char **argv) {
	inputManager = InputManager::Inst();
	mouseLeft = Point2(0, 0);
	mouseRight = Point2(0, 0);


    // init GLUT and create window
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(300, 100);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("GLUT Window");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "Status: Using GLU %s\n", gluGetString(GLU_VERSION));
	fprintf(stdout, "Status: Using GL %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "Status: Using GLUT %d (X.XX.XX)\n", glutGet(GLUT_VERSION));
	fprintf(stdout, "Renderer (%s): \n",	glGetString(GL_RENDERER));
	
    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(game);

    glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(KeyboardSpecial);
	glutSpecialUpFunc(KeyboardSpecialUp);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(MouseMove);
	glutMotionFunc(MouseMove);
    //glutMouseWheelFunc() //TODO

	//imgui init
	init();

    GameInit();

	//printf("%s\n",glGetString(GL_EXTENSIONS));
	
	wglSwapIntervalEXT(0);
    // enter GLUT event processing loop
    glutMainLoop();

	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

    return 1;
}

