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

class Animation
{
public:
	Animation(Entity* target, Pose start, Pose end, float time_in_seconds);
	Animation();
	void SetTarget(Entity* target);
	Entity* GetTarget();

	void SetTransformation(Pose start, Pose end);

	void Play(bool start_from_beginning = false);
	void Stop();
	void Update(float deltaTime);

private:
	Entity * m_target;
	Pose m_start_pose;
	Pose m_end_pose;
	bool m_playing;
	float m_length;
	float m_progress;
};

class AnimationController
{
public:
	AnimationController() {};
	void Update(float deltaTime);
	void AddAnimation(Animation animation);
private:
	std::vector<Animation> m_animations;
};