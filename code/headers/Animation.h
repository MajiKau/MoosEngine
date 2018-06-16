#pragma once
#include "Entity.h"

class Entity;

struct Pose
{
	Pose() {};
	Pose(glm::vec3 position, glm::quat rotation) :Position(position), Rotation(rotation) {};
	glm::vec3 Position;
	glm::quat Rotation;
};

struct KeyFrame
{
	KeyFrame(float _time, Pose _pose) :time(_time), pose(_pose) {};
	float time;
	Pose pose;
};

class Animation
{
public:
	Animation(std::string name);
	//Animation(Entity* target, Pose start, Pose end, float time_in_seconds);
	Animation();
	void SetTarget(Entity* target);
	Entity* GetTarget();

	std::string GetName();

	//void SetTransformation(Pose start, Pose end);
	void SetLooping(bool loops);

	void Play(bool start_from_beginning);
	void Stop();
	void Update(float deltaTime);

	void AddKeyFrame(float time, Pose pose);
	void AddKeyFrame(KeyFrame keyframe);

private:

	void _SortKeyframes();

	std::string m_name;
	std::vector<KeyFrame> m_keyframes;
	int m_current_frame;

	Entity * m_target;
	//float m_length;
	float m_current_time;

	float m_progress;
	Pose m_start_pose;
	Pose m_end_pose;

	bool m_playing;
	bool m_looping;
};



class AnimationController
{
public:
	AnimationController() {};
	void Update(float deltaTime);
	void AddAnimation(Animation animation);

	void PlayAnimation(std::string name);
private:
	std::vector<Animation> m_animations;
};