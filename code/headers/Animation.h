#pragma once
#include "Entity.h"

class Entity;

struct Pose
{
	Pose() {};
	Pose(glm::vec3 position, glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f))
		:Position(position), Rotation(rotation), Scale(scale) {};
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};

struct KeyFrame
{
	KeyFrame(float _time, Pose _pose) :time(_time), pose(_pose) {};
	float time;
	Pose pose;
};

struct AnimationGroup
{
	std::string m_name = "";
	std::vector<std::pair<std::vector<int>, std::vector<KeyFrame>>> m_keyframes;
	float m_current_time = 0.0f;
	float m_end_time = 0.0f;
	bool m_playing = false;
	bool m_looping = false;
};

//TODO: Make make a vector of pair(vector<KeyFrame>, Entity*) for more complex animations that use children.
class Animation
{
public:

	Animation(std::string name);
	Animation();

	void SetTarget(Entity* target);
	Entity* GetTarget();

	void SetName(std::string name);
	std::string GetName();

	//void SetTransformation(Pose start, Pose end);
	void SetLooping(bool loops);
	bool GetLooping();

	void Play(bool start_from_beginning);
	void Stop();
	void Update(float deltaTime);

	void AddKeyFrame(KeyFrame keyframe, std::vector<int> child = {});
	void AddKeyFrame(float time, Pose pose, std::vector<int> child = {});
	void RemoveKeyFrame(uint keyframe, std::vector<int> child = {});


	AnimationGroup GetAnimationData();
	void SetAnimationData(AnimationGroup data);

	void SaveAnimation(std::string file);
	bool LoadAnimation(std::string file);

private:

	void _SortKeyframes(int index);
	void _CheckEndTime();

	//std::string m_name;
	AnimationGroup m_animation;
	Entity * m_parent;
	//int m_current_frame;

	//Entity * m_target;
	//float m_length;
	//float m_current_time;

	//float m_progress;
	//Pose m_start_pose;
	//Pose m_end_pose;

	//bool m_playing;
	//bool m_looping;
};





class AnimationController
{
public:
	AnimationController() {};
	void Update(float deltaTime);
	void AddAnimation(Animation animation);

	void PlayAnimation(std::string name);
	std::vector<Animation> GetAnimations();
	void SetAnimations(std::vector<Animation> animations);

private:
	std::vector<Animation> m_animations;
};