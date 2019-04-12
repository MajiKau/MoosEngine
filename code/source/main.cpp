#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GL/wglew.h>

#include <glm.hpp>
#include <ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/intersect.hpp>

#include <imgui_impl_glut.h>

#include "code/headers/Geometry.h"
#include "code/headers/InputManager.h"

#include "code/headers/Renderer2D.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

const char* window_name = "GLUT Window";
bool window_focus = false;

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

glm::mat4 Projection;
glm::mat4 View;

//

bool warped = false;
bool mouselock = true;

bool rotateLight = true;
float lightDistance = 200.0f;
int pLightID = 0;

bool renderPlane = false;

bool showdebug = false;

//Scene MainScene;

Renderer2D* renderer;

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
/*void EntityToTreeNode(Entity* entity)
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
}*/

void drawGUI()
{
	ImGui_ImplGLUT_NewFrame(screenWidth, screenHeight);

	

    // Show a simple window
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("Debug"))
			showdebug ^= 1;
	}

		//Entity browser!
	/*{
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
				//if (name[0] != '\0')
				//	selected_entity->SpawnChild(name);
				//else
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
	}*/
	/*{
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
	}*/


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

void GameInit()
{
	renderer = new Renderer2D(zoom, screenRatio);

	/*Entity* box = MainScene.SpawnEntity();
	box->AddMesh("testcube");
	box->SetLocalPosition({ 5,0,0 });*/

}


void renderScene(void)
{
    Projection = glm::perspective(45.0f, (float)screenWidth / screenHeight, 0.1f, 1000.f);
    glm::vec4 front(0, 0, -1, 1);
    glm::vec4 right(1, 0, 0, 1);
    glm::vec4 up(0, 1, 0, 1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Rendering

	//MainScene.Render(renderer);    

    GLenum err = glGetError();
    if (GLEW_OK != err)
    {
        printf("%s\n", glewGetErrorString(err));
    }

	//renderer->Render();
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



float speed = 10.0f;

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

	//MainScene.Update(deltaTime);

	totalTime = (float)glutGet(GLUT_ELAPSED_TIME);



	

	//INPUTS
	if ((mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) && mouselock && window_focus)
	{
		//renderer->Camera.Rotation.x += mouseDelta.x*0.001f;
		//renderer->Camera.Rotation.y += mouseDelta.y*0.001f;
	}

	//Escape -> Exit
	if (inputManager->IsKeyDown(27))
		glutExit();



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

	if (inputManager->IsKeyPressed('t'))
	{
	}
	if (inputManager->IsKeyPressed('y'))
	{
	}

    //Movement

    if (inputManager->IsMousePressed(GLUT_RIGHT_BUTTON))
    {
        speed *= 5.0f;
    }
	if (inputManager->IsMouseReleased(GLUT_RIGHT_BUTTON))
	{
		speed /= 5.0f;
	}
	if (window_focus)
	{
		if (GetAsyncKeyState(0x57))//W
		{
			//renderer->Camera.Position += Forward * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x53))//S
		{
			//renderer->Camera.Position -= Forward * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x41))//A
		{
			//renderer->Camera.Position -= Right * speed * (float)deltaTime;
		}
		if (GetAsyncKeyState(0x44))//D
		{
			//renderer->Camera.Position += Right * speed * (float)deltaTime;
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
        //renderer->Camera.Position.y += 1.0f;
        zoom -= 5.0f;
    }
    if (inputManager->IsMousePressed(4))//Scroll down
    {
        //renderer->Camera.Position.y -= 1.0f;
        zoom += 5.0f;
    }
    

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
		//renderer->CompileShaders();
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

