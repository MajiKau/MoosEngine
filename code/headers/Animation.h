#pragma once
#include "Entity.h"

class Animation
{
public:
	Animation(Entity* target, glm::mat4 transformation, float time_in_seconds);
	Animation();
	void SetTarget(Entity* target);
	void SetTransformation(glm::mat4 transformation);

	void Play(bool start_from_beginning = false);
	void Stop();
	void Update(float deltaTime);

private:
	Entity * m_target;
	glm::mat4 m_transformation;
	bool m_playing;
	float m_length;
	float m_progress;
};

class AnimationController
{
public:
	AnimationController() {};
	void Update(float deltaTime);
	
private:
	std::vector<Animation> m_animations;
};