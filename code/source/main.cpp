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

#include "code/headers/Vehicle.h"
#include "code/headers/Scene.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

const char* window_name = "GLUT Window";
bool window_focus = false;

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

int selectedNode = 0;

float zoom = 100.0f;

BatchRenderer* renderer;
glm::vec3 previousCameraPosition;


glm::mat4 Projection;
glm::mat4 View;
glm::vec3 PlaneRot;

Material CustomMaterial;


float score = 500.0f;

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

	int mods = glutGetModifiers();
	io.KeyCtrl = GLUT_ACTIVE_CTRL & mods;
	io.KeyShift = GLUT_ACTIVE_SHIFT & mods;
	io.KeyAlt = GLUT_ACTIVE_ALT & mods;
}
void KeyboardSpecialUp(int key, int x, int y)
{
    inputManager->_ReleaseSpecial(key);

    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;

	int mods = glutGetModifiers();
	io.KeyCtrl = GLUT_ACTIVE_CTRL & mods;
	io.KeyShift = GLUT_ACTIVE_SHIFT & mods;
	io.KeyAlt = GLUT_ACTIVE_ALT & mods;
}
bool CheckWindowFocus(LPCSTR windowName)
{
	HWND windowHwnd = FindWindow(NULL, windowName);
	if (windowHwnd == NULL)
		return false;
	return windowHwnd == GetFocus();
}

void MouseMove(int x, int y)
{
    mouseDelta += {-x + screenWidth / 2.0f, -y + screenHeight / 2.0f};
    if (mouselock)
    {
        if (!warped && window_focus)
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
		io.MouseWheel = 1.0f;
        break;
    case 4:
		io.MouseWheel = -1.0f;
        break;
    default:
        printf("Unknown Mouse Button:%d\n", button);
        break;
    }
}
void MouseWheel(int wheel, int direction, int x, int y)//Not used!
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	io.MouseWheel = direction;
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

int selected_tree_node = 0;
void EntityToTreeNode(Entity* entity)
{
	std::vector<Entity*> children = entity->GetChildren();
	if (children.size() == 0)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| (selected_tree_node == entity->GetId() ? ImGuiTreeNodeFlags_Selected : 0) 
			| ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::TreeNodeEx(entity->GetName().c_str(), node_flags); 
		if (ImGui::IsItemClicked())
		{
			selected_tree_node = entity->GetId();
		}
	}
	else
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| (selected_tree_node == entity->GetId() ? ImGuiTreeNodeFlags_Selected : 0);
		bool node_open = ImGui::TreeNodeEx(entity->GetName().c_str(), node_flags);
		if (ImGui::IsItemClicked())
		{
			selected_tree_node = entity->GetId();
		}
		if (node_open)
		{
			for each (auto child in children)
			{
				EntityToTreeNode(child);
			}
			ImGui::TreePop();
		}
	}
}
void EntitiesToTreeNode(std::vector<Entity*> entities)
{
	for each (auto entity in entities)
	{
		EntityToTreeNode(entity);
	}
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

		//Entity browser!
	{
		ImGui::Begin("Browser");
		ImGui::BeginChild("EntityTree", { 0,200 }, true);
		EntitiesToTreeNode(MainScene.GetChildren());
		ImGui::EndChild();
		ImGui::End(); 
	}

	Entity* selected_entity = MainScene.FindEntityWithId(selected_tree_node);
	{
		ImGui::Begin("Entity");
		if (selected_entity != NULL)
		{
			glm::vec3 position = selected_entity->GetLocalPosition();
			glm::quat rotation = selected_entity->GetLocalRotation();
			glm::vec3 scale = selected_entity->GetLocalScale();
			ImGui::BeginChild("EntityInfo", { 0,220 }, true);
			ImGui::Text(("Name: " + selected_entity->GetName() + "[%d]").c_str(), selected_entity->GetId());
			ImGui::DragFloat3("Position", &position[0], 0.25f);
			ImGui::DragFloat4("Rotation", &rotation[0], 0.25f);
			ImGui::DragFloat3("Scale", &scale[0], 0.25f);
			static char name[20];
			ImGui::InputText("NewChildName", name, 20);
			if (ImGui::Button("SpawnChild"))
			{
				if (name[0] != '\0')
					selected_entity->SpawnChild(name);
				else
					selected_entity->SpawnChild();
			}
			if (ImGui::Button("SpawnParent"))
			{
				/*if (name[0] != '\0')
					selected_entity->SpawnChild(name);
				else*/
					selected_entity->SpawnParent();
			}
			static char mesh_name[20];
			ImGui::InputText("NewMeshName", mesh_name, 20);
			if (ImGui::Button("AddMesh"))
			{
				selected_entity->AddMesh(mesh_name);
			}
			ImGui::EndChild();
			selected_entity->SetLocalPosition(position);
			selected_entity->SetLocalRotation(rotation);
			selected_entity->SetLocalScale(scale);
		}
		ImGui::End();
	}
	{
		ImGui::Begin("Animation"); 
		KeyFrame frame(0, {});
		if (selected_entity != NULL)
		{
			auto animation_controller = selected_entity->GetAnimationController();
			auto animations = animation_controller->GetAnimations();
			int anim_num = 0;
			static int selected_animation = 0;
			static int selected_pair = 0;
			static int selected_keyframe = 0;
			//Animations inside the animation controller
			for each (auto animation in animations)
			{
				AnimationGroup data = animation.GetAnimationData();
				if (ImGui::Selectable(data.m_name.c_str(), selected_animation == anim_num, 0, { 0,0 }))
				{
					selected_animation = anim_num;
				}
				if (ImGui::IsItemClicked())
				{
					selected_pair = 0;
					selected_keyframe = 0;
				}
				anim_num++;
			}
			ImGui::Separator();
			if (selected_animation < animations.size())
			{
				AnimationGroup data = animations[selected_animation].GetAnimationData();

				ImGui::InputText("Name", &data.m_name[0], 16);
				ImGui::Checkbox("Loop", &data.m_looping);


				int pair_num = 0;
				for each (auto pair in data.m_keyframes)
				{
					std::string child = selected_entity->GetChild(pair.first)->GetName();


					if (ImGui::Selectable(("Anim:" + child).c_str(), selected_pair == pair_num, 0, { 0,0 }))
					{
						selected_pair = pair_num;
					}
					if (ImGui::IsItemClicked())
					{
						selected_keyframe = 0;
					}

					pair_num++;
				}
				ImGui::Separator();
				if (selected_pair < data.m_keyframes.size())
				{
					ImGui::BeginChild("Frames", { 0,45 }, true, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
					int frame_amount = data.m_keyframes[selected_pair].second.size();
					for (int i = 0; i < frame_amount; i++)
					{
						if (ImGui::Selectable(std::to_string(i).c_str(), selected_keyframe == i, 0, { 10,0 }))
							selected_keyframe = i;
						//if (i + 1 < frame_amount)
							ImGui::SameLine();
					}
					bool add_frame = ImGui::Button("+", { 15,13 });
					ImGui::SameLine();
					bool remove_frame = ImGui::Button("-", { 15,13 });
					ImGui::EndChild();

					ImGui::Separator();
					//assert(data.m_keyframes[selected_pair].second.size() != 0);
					if (selected_keyframe < data.m_keyframes[selected_pair].second.size())
					{
						//TODO: Breaks if last frame is removed. Add RemoveFrame function to Animation.
						if (remove_frame && frame_amount > 1)
						{
							data.m_keyframes[selected_pair].second.erase(data.m_keyframes[selected_pair].second.begin() + selected_keyframe);
							if (selected_keyframe == data.m_keyframes[selected_pair].second.size())
							{
								selected_keyframe--;
								if (selected_keyframe < 0)
								{
									selected_keyframe = 0;
								}
								else
								{
									frame = data.m_keyframes[selected_pair].second[selected_keyframe];
								}
							}
							else
							{
								frame = data.m_keyframes[selected_pair].second[selected_keyframe];
							}
						}
						else
						{
							frame = data.m_keyframes[selected_pair].second[selected_keyframe];
						}
					}
					else
					{
						selected_keyframe = 0;
						//frame = data.m_keyframes[selected_pair].second[selected_keyframe];
					}
					if (add_frame)
					{
						data.m_keyframes[selected_pair].second.push_back(frame);
					}



				}
				else
				{
					selected_pair = 0;
				}

				ImGui::DragFloat("Time", &frame.time, 0.05f);
				ImGui::DragFloat3("Position", &frame.pose.Position[0], 0.05f);
				ImGui::DragFloat4("Rotation", &frame.pose.Rotation[0], 0.05f);
				ImGui::DragFloat3("Scale", &frame.pose.Scale[0], 0.05f); 

				if (selected_keyframe < data.m_keyframes[selected_pair].second.size())
				{
					data.m_keyframes[selected_pair].second[selected_keyframe] = frame;
				}
				animations[selected_animation].SetAnimationData(data);
				animation_controller->SetAnimations(animations);

				if (ImGui::Button("Save Animation"))
				{
					animations[selected_animation].SaveAnimation("Content/Animations/"+data.m_name+".txt");
				}
				if (ImGui::Button("Load Animation"))
				{
					if(animations[selected_animation].LoadAnimation("Content/Animations/" + data.m_name + ".txt"))
						animation_controller->SetAnimations(animations);
				}
			}
		}
		ImGui::End();

		//Keyframe editing
		ImGui::Begin("KeyFrame");
		ImGui::End();
	}


	ImGui::Render();
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


Entity* fish_entity;
Entity* frame_entity;
Entity* platform_entity;
Entity* left_entity;
Entity* right_entity;

Entity* tank_entity;
Entity* tank_rb_entity;
Entity* tank_bot_entity;
Entity* tank_top_entity;

Entity* wave_entity;

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
   

	for (int x = -5; x < 5; x++)
	{
		for (int y = -5; y < 5; y++)
		{
			for (int z = -5; z < 5; z++)
			{
				//test_objects.push_back({ x * 100.0f - rand() % 10,y * 100.0f - rand() % 10,z * 100.0f - rand() % 10,3,6,4 });
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
	//platform.SaveAnimation("plat.txt");

	//Animation platformtest("");
	//platformtest.LoadAnimation("plat.txt");

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

	Entity* ground = MainScene.SpawnEntity("Ground");
	ground->AddMesh("testcube");
	ground->SetLocalPosition({ 50,10,0 });
	ground->SetLocalScale({ 20,1,20 });

	Entity* basiccube = MainScene.SpawnEntity("BasicCube");
	basiccube->AddMesh("BasicCube");
	basiccube->SetLocalPosition({ 0,10,0 });
	basiccube->SetLocalScale({ 10,10,10 });


	Entity* cube = MainScene.SpawnEntity("Cube");
	cube->AddMesh("Cube");
	cube->SetLocalPosition({ -20,20,-130 });
	cube->SetLocalScale({ 10,10,10 });

	fish_entity = MainScene.SpawnEntity("Platform_Root");
	fish_entity->SetLocalPosition({ 0,30,0 });

	frame_entity = fish_entity->SpawnChild("Platform_Frame");
	frame_entity->AddMesh("v_platform_frame");
	frame_entity->SetLocalPosition({ 0,0.5f,0 });

	platform_entity = fish_entity->SpawnChild("Platform_Full");
	platform_entity->AddMesh("v_platform_left");
	platform_entity->AddMesh("v_platform_right");
	platform_entity->AddAnimation(anim1);
	platform_entity->AddAnimation(platform);

	left_entity = fish_entity->SpawnChild("Platform_Left");
	left_entity->AddMesh("v_platform_left");
	left_entity->AddAnimation(left);

	right_entity = fish_entity->SpawnChild("Platform_Right");
	right_entity->AddMesh("v_platform_right");
	right_entity->AddAnimation(right);
	//e2->SetLocalPose(glm::translate(glm::vec3(0.0f, 20.0f, 0.0f))*glm::rotate(PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)));

	tank_rb_entity = platform_entity->SpawnChild("Tank");
	tank_entity = tank_rb_entity->SpawnChild("Tank_Mesh_Root");
	//tank_entity->AddMesh("fulltank");
	tank_entity->AddAnimation(tank);
	tank_entity->SetLocalPosition({ 0,1,0 });
	tank_bot_entity = tank_entity->SpawnChild("Tank_Bottom");
	tank_bot_entity->AddMesh("tank_bottom");
	tank_top_entity = tank_bot_entity->SpawnChild("Tank_Top");
	tank_top_entity->AddMesh("tank_top");
	tank_top_entity->SetLocalRotation(glm::rotate(glm::quat(1,0,0,0), -10.1f, { 0,1,0 }));

	Entity* door = MainScene.SpawnEntity("Door");
	door->AddMesh("DoorFrame");
	Entity* portal = door->SpawnChild("Portal");
	//portal->AddMesh("Door");
	Entity* tunnel = door->SpawnChild("Tunnel");
	tunnel->AddMesh("DoorFrame");
	tunnel->SetLocalScale({ 1,1,4 });
	tunnel->SetLocalPosition({ 0,0,-1.9f });
	door->SetLocalPosition({ 0,1,-100 });

	Entity* door2 = MainScene.SpawnEntity("Door2");
	door2->AddMesh("DoorFrame");
	Entity* portal2 = door2->SpawnChild("Portal2");
	portal2->SetLocalPosition({ 0,0,-9.6f });
	//portal2->AddMesh("Door");
	Entity* tunnel2 = door2->SpawnChild("Tunnel2");
	tunnel2->AddMesh("DoorFrame");
	tunnel2->SetLocalScale({ 1,1,20 });
	tunnel2->SetLocalPosition({ 0,0,-9.6f });
	Entity* door2c = door2->SpawnChild("Door2c");
	//door2c->AddMesh("DoorFrame");
	door2c->SetLocalPosition({ 0,0,-20.0f });
	door2->SetLocalPosition({ 0,-99,-100 });

	{
		Animation wave_animation("Wave");
		wave_animation.SetLooping(true);

		wave_animation.AddKeyFrame(0.0f, Pose({ 10.0f,10.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), -0.1f, glm::vec3(0, 1, 0)), { 1,1,1 }), {});//PARENT
		wave_animation.AddKeyFrame(1.0f, Pose({ 10.0f,10.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), 0.1f, glm::vec3(0, 1, 0)), { 0.5f,5.0f,3.0f }), {});//PARENT
		wave_animation.AddKeyFrame(2.0f, Pose({ 10.0f,10.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), -0.1f, glm::vec3(0, 1, 0)), { 1,1,1 }), {});//PARENT

		wave_animation.AddKeyFrame(0.0f, Pose({ 1.0f,4.0f,0.0f }, { 1,0,0,0 }, { 2,2,2 }), { 0 });//HEAD
		wave_animation.AddKeyFrame(1.0f, Pose({ -1.0f,4.0f,0.0f }, { 1,0,0,0 }, { 2,2,2 }), { 0 });//HEAD
		wave_animation.AddKeyFrame(2.0f, Pose({ 1.0f,4.0f,0.0f }, { 1,0,0,0 }, { 2,2,2 }), { 0 });//HEAD

		wave_animation.AddKeyFrame(0.0f, Pose({ 0.0f,0.0f,0.0f }, { 1,0,0,0 }, { 4,6,1 }), { 1 });//BODY

		wave_animation.AddKeyFrame(0.0f, Pose({ 2.0f,2.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), 0.0f, glm::vec3(0, 0, 1)), { 1,1,1 }), { 2 });//HAND_JOINT
		wave_animation.AddKeyFrame(1.0f, Pose({ 2.0f,2.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), PI / 3.0f, glm::vec3(0, 0, 1)), { 1,1,1 }), { 2 });//HAND_JOINT
		wave_animation.AddKeyFrame(2.0f, Pose({ 2.0f,2.0f,0.0f }, glm::rotate(glm::quat(1, 0, 0, 0), 0.0f, glm::vec3(0, 0, 1)), { 1,1,1 }), { 2 });//HAND_JOINT

		wave_animation.AddKeyFrame(0.0f, Pose({ 1.5f,0.0f,0.0f }, { 1,0,0,0 }, { 3,1,1 }), { 2,0 });//HAND

		wave_animation.SaveAnimation("SimpleWaveTest.txt");

		wave_animation.LoadAnimation("Content/Animations/Wave.txt");

		Animation LoadTestAnimation;
		LoadTestAnimation.LoadAnimation("SimpleWaveTest.txt");
		LoadTestAnimation.SaveAnimation("LoadTest.txt");


		wave_entity = MainScene.SpawnEntity("Person");
		wave_entity->AddAnimation(wave_animation);
		Entity* head = wave_entity->SpawnChild("Head");
		head->AddMesh("Cube");
		Entity* body = wave_entity->SpawnChild("Body");
		body->AddMesh("Cube");
		Entity* hand_joint = wave_entity->SpawnChild("Elbow");
		Entity* hand = hand_joint->SpawnChild("Arm");
		hand->AddMesh("Cube");


		wave_entity->SetLocalPosition({ 10,100,10 });
		wave_entity->PlayAnimation("Wave");

		Entity* person_root = wave_entity->SpawnParent("Person_Root");
		person_root->SetLocalPosition({ 20,-20,-120 });
		person_root->SetLocalRotation(glm::rotate(glm::quat(1,0,0,0), 2*PI/3,glm::vec3(0,1,0)));

		//wave_animation.LoadAnimation("Content/Animations/Wave.txt");
		//wave_entity->GetAnimationController()->SetAnimations({ wave_animation });
	}
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Rendering

	MainScene.Render(renderer);

	for each (auto light in renderer->m_point_light)
	{
		renderer->RenderRegularTetrahedron(light.Position, 3, Color3f(light.Color.x, light.Color.y, light.Color.z));
	}
    

    Material DefaultMaterial;
    DefaultMaterial.ambient = { 1.0f,1.0f,1.0f };//Currently does nothing
    DefaultMaterial.diffuse = { 0.5f,0.5f,0.5f };//Currently does nothing
    DefaultMaterial.specular = { 1.0f,1.0f,1.0f };//Currently does nothing
    DefaultMaterial.shininess = 32.0f;

    Material material = DefaultMaterial;

    GLenum err = glGetError();
    if (GLEW_OK != err)
    {
        printf("%s\n", glewGetErrorString(err));
    }

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



void game()
{
	//UPDATES
	float deltaTime = glutGet(GLUT_ELAPSED_TIME) - totalTime;
	deltaTime /= 1000.0f;
	if (deltaTime > 1.0f)
	{
		deltaTime = 1.0f;
	}
	
	window_focus = CheckWindowFocus(window_name);

	MainScene.Update(deltaTime);

	tank.Update(deltaTime);

	totalTime = (float)glutGet(GLUT_ELAPSED_TIME);

	if (rotateLight)
	{
		renderer->m_point_light[0].Position.x = lightDistance * sinf(totalTime / 500.0f);
		renderer->m_point_light[0].Position.z = lightDistance * cosf(totalTime / 500.0f);

		renderer->m_point_light[1].Position.x = lightDistance * sinf(PI + totalTime / 500.0f);
		renderer->m_point_light[1].Position.z = lightDistance * cosf(PI + totalTime / 500.0f);
	}

	

	//INPUTS
	if ((mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) && mouselock && window_focus)
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

	//if (inputManager->IsKeyPressed('r'))

	//TODO:
	if (inputManager->IsKeyPressed('m'))
	{
		left_entity->PlayAnimation("left");
		right_entity->PlayAnimation("right");
		platform_entity->PlayAnimation("platform");
		tank_entity->PlayAnimation("tank");
	}


	if (inputManager->IsKeyPressed('n'))
	{
		platform_entity->PlayAnimation("anim1");
		wave_entity->SetLocalPosition(cursor);
	}

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

	if (inputManager->IsKeyPressed('t'))
	{
	}
	if (inputManager->IsKeyPressed('y'))
	{
	}

    //Movement
    glm::vec3 Forward = glm::normalize(glm::vec3(renderer->Camera.Forward.x, 0, renderer->Camera.Forward.z));
    //glm::vec3 Forward = glm::normalize(renderer->Camera.Forward);
    glm::vec3 Right = glm::normalize(glm::cross(glm::vec3(renderer->Camera.Forward.x, 0, renderer->Camera.Forward.z), glm::vec3(0, 1, 0)));

    float speed = 100.0f;
    if (inputManager->IsMouseDown(GLUT_RIGHT_BUTTON))
    {
        speed = 10.0f;
    }
	if (window_focus)
	{
		if (GetAsyncKeyState(0x57))//W
		{
			renderer->Camera.Position += Forward * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x53))//S
		{
			renderer->Camera.Position -= Forward * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x41))//A
		{
			renderer->Camera.Position -= Right * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x44))//D
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
	}

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

	if (previousCameraPosition.x > -2.0f &&  previousCameraPosition.x < 2.0f)
	{
		if (renderer->Camera.Position.y > 0)
		{

			if (renderer->Camera.Position.z < -99.0f && previousCameraPosition.z >= -99.0f)//Forward
			{
				renderer->Camera.Position.y -= 100.0f;
				glm::vec3 res;
				bool hit = glm::intersectLineTriangle(previousCameraPosition, renderer->Camera.Position - previousCameraPosition, { -0.45, 1.0, -100 }, { 0.45, 1.0, -100 }, { 0.45, 3.1, -100 }, res);
				if (hit) printf("Hit!\n");
			}
			else if (renderer->Camera.Position.z > -100.0f && previousCameraPosition.z <= -100.0f)//Backward
			{
				renderer->Camera.Position.y -= 100.0f;
				renderer->Camera.Position.z -= 7.7f;
				glm::vec3 res;
				bool hit = glm::intersectLineTriangle(previousCameraPosition, renderer->Camera.Position - previousCameraPosition, { -0.45, 1.0, -100 }, { 0.45, 1.0, -100 }, { 0.45, 3.1, -100 }, res);
				if (hit) printf("Hit!\n");
			}
		}
		else
		{
			if (renderer->Camera.Position.z > -99.0f && previousCameraPosition.z <= -99.0f)//Backward
			{
				renderer->Camera.Position.y += 100.0f;
				glm::vec3 res;
				bool hit = glm::intersectLineTriangle(previousCameraPosition, renderer->Camera.Position - previousCameraPosition, { -0.45, 1.0, -100 }, { 0.45, 1.0, -100 }, { 0.45, 3.1, -100 }, res);
				if (hit) printf("Hit!\n");
			}
			else if (renderer->Camera.Position.z < -109.0f && previousCameraPosition.z >= -109.0f)//Forward
			{
				renderer->Camera.Position.z += 7.7f;
				renderer->Camera.Position.y += 100.0f;
				glm::vec3 res;
				bool hit = glm::intersectLineTriangle(previousCameraPosition, renderer->Camera.Position - previousCameraPosition, { -0.45, 1.0, -100 }, { 0.45, 1.0, -100 }, { 0.45, 3.1, -100 }, res);
				if (hit) printf("Hit!\n");
			}
		}
	}

	previousCameraPosition = renderer->Camera.Position;

	//printf("X:%.2f,Y:%.2f,Z:%.2f\n", renderer->Camera.Position.x, renderer->Camera.Position.y, renderer->Camera.Position.z);
    
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

	if (inputManager->IsSpecialReleased(GLUT_KEY_F5))
	{
		renderer->CompileShaders();
	}

    mouseDelta = { 0,0 }; //TODO remove
    inputManager->_Clear();
}




int main(int argc, char **argv)
{
	inputManager = InputManager::Inst();
	mouseLeft = Point2(0, 0);
	mouseRight = Point2(0, 0);


    // init GLUT and create window
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(300, 100);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow(window_name);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LEQUAL); 
	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_STENCIL_TEST); // Enable stencil testing

	glEnable(GL_CULL_FACE); // enable face culling
	//glCullFace(GL_FRONT); // culls front faces
	glCullFace(GL_BACK); // culls back faces

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
	//glutMouseWheelFunc(MouseWheel); //TODO

	//imgui init
	init();

    GameInit();

	//printf("%s\n",glGetString(GL_EXTENSIONS));
	
	wglSwapIntervalEXT(0);//Vsync
    // enter GLUT event processing loop
    glutMainLoop();

	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

    return 1;
}

