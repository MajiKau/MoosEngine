#include "code/headers/Animation.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_interpolation.hpp>
#include <gtx/compatibility.hpp>

Animation::Animation(Entity * target, Pose start, Pose end, float time_in_seconds)
{
	m_target = target;
	m_start_pose = start;
	m_end_pose = end;
	m_length = time_in_seconds;

	m_progress = 0.0f;
	m_playing = true;
}

Animation::Animation()
{
	m_target = NULL;
	m_start_pose = Pose();
	m_end_pose = Pose();
	m_length = 1.0f;
	m_progress = 0.0f;
	m_playing = false;
}

void Animation::SetTarget(Entity * target)
{
	m_target = target;
}

Entity * Animation::GetTarget()
{
	return m_target;
}

void Animation::SetTransformation(Pose start, Pose end)
{
	m_start_pose = start;
	m_end_pose = end;
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
	/*glm::quat q1 = glm::quat(m_start_pose);
	glm::quat q2 = glm::quat(m_end_pose);
	glm::quat q = glm::slerp(q1, q2, m_progress);*/
	glm::vec3 new_position = glm::lerp(m_start_pose.Position, m_end_pose.Position, m_progress);
	glm::quat new_rotation = glm::slerp(m_start_pose.Rotation, m_end_pose.Rotation, m_progress);
	m_target->SetLocalPosition(new_position);
	m_target->SetLocalRotation(new_rotation);
	/*glm::mat4 new_pose = glm::interpolate(glm::mat4(1.0f), m_end_pose, 0.0f);
	new_pose = glm::interpolate(glm::mat4(1.0f), m_end_pose, 2.0f);*/
	//m_target->SetLocalPose(new_pose);
	//glm::vec3 m_position = m_target->GetLocalPosition();

	if (m_playing)
	{
		m_progress += deltaTime / m_length;
	}
	if (m_progress >= 1.0f)
	{
		Stop();
		m_progress = 1.0f;
	}
}

void AnimationController::Update(float deltaTime)
{
	for (size_t i = 0; i < m_animations.size(); i++)
	{
		m_animations[i].Update(deltaTime);
	}
}

void AnimationController::AddAnimation(Animation animation)
{
	m_animations.emplace_back(animation);
}
