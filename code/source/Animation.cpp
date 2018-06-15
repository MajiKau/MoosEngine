#include "code/headers/Animation.h"

Animation::Animation(Entity * target, glm::mat4 transformation, float time_in_seconds)
{
	m_target = target;
	m_transformation = transformation;
	m_length = time_in_seconds;

	m_progress = 0.0f;
	m_playing = true;
}

Animation::Animation()
{
	m_target = NULL;
	m_transformation = glm::imat4();
	m_length = 1.0f;
	m_progress = 0.0f;
	m_playing = false;
}

void Animation::SetTarget(Entity * target)
{
	m_target = target;
}

void Animation::SetTransformation(glm::mat4 transformation)
{
	m_transformation = transformation;
}

void Animation::Play(bool start_from_beginning)
{
	m_playing = true;
	if (start_from_beginning)
	{
		m_progress = 0.0f;
	}
}

void Animation::Stop()
{
	m_playing = false;
}

void Animation::Update(float deltaTime)
{
	if (m_playing)
	{
		m_progress += deltaTime / m_length;
	}
	if (m_progress >= 1.0f)
	{
		Stop();
		m_progress = 0.0f;
	}
}

void AnimationController::Update(float deltaTime)
{
	for each (auto animation in m_animations)
	{
		animation.Update(deltaTime);
	}
}
