#include "code/headers/Animation.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_interpolation.hpp>
#include <gtx/compatibility.hpp>

Animation::Animation(std::string name)
{
	m_name = name;
	m_current_frame = 0;
	m_current_time = 0;
	m_target = NULL;
	m_progress = 0.0f;
	m_start_pose = Pose();
	m_end_pose = Pose();
	m_playing = false;
	m_looping = false;
}

//Animation::Animation(Entity * target, Pose start, Pose end, float time_in_seconds)
//{
//	m_target = target;
//	m_start_pose = start;
//	m_end_pose = end;
//	m_length = time_in_seconds;
//
//	m_progress = 0.0f;
//	m_playing = true;
//}

Animation::Animation()
{
	m_name = ""; 
	m_current_frame = 0;
	m_current_time = 0;
	m_target = NULL;
	m_progress = 0.0f;
	m_start_pose = Pose();
	m_end_pose = Pose();
	m_playing = false; 
	m_looping = false;
}

void Animation::SetTarget(Entity * target)
{
	m_target = target;
}

Entity * Animation::GetTarget()
{
	return m_target;
}

std::string Animation::GetName()
{
	return m_name;
}

//void Animation::SetTransformation(Pose start, Pose end)
//{
//	m_start_pose = start;
//	m_end_pose = end;
//}

void Animation::SetLooping(bool loops)
{
	m_looping = loops;
}

void Animation::Play(bool start_from_beginning)
{
	if (m_keyframes.size() == 0)
	{
		printf("Trying to play animation with no keyframes!\n");
		return;
	}
	m_playing = true;
	if (start_from_beginning)
	{
		m_current_time = 0.0f;
		m_current_frame = 0;
	}
}

void Animation::Stop()
{
	m_playing = false;
}

void Animation::Update(float deltaTime)
{
	if (!m_playing) return;

	m_current_time += deltaTime;
	int last_frame = m_keyframes.size() - 1;
	if (m_current_time > m_keyframes[last_frame].time)
	{
		if (m_looping)
		{
			m_current_time -= m_keyframes[last_frame].time;
		}
		else
		{
			m_current_time = m_keyframes[last_frame].time;
			m_playing = false;
		}
		m_current_frame = 0;
	}
	while (m_current_time > m_keyframes[m_current_frame + 1].time)
	{
		m_current_frame++;
		if (m_current_frame >= last_frame)
		{

		}
	}
	m_start_pose = m_keyframes[m_current_frame].pose;
	m_end_pose = m_keyframes[m_current_frame+1].pose;
	m_progress = (m_current_time - m_keyframes[m_current_frame].time) / (m_keyframes[m_current_frame + 1].time - m_keyframes[m_current_frame].time);

	glm::vec3 new_position = glm::lerp(m_start_pose.Position, m_end_pose.Position, m_progress);
	glm::quat new_rotation = glm::slerp(m_start_pose.Rotation, m_end_pose.Rotation, m_progress);
	m_target->SetLocalPosition(new_position);
	m_target->SetLocalRotation(new_rotation);

	/*if (m_playing)
	{
		m_progress += deltaTime / m_length;
	}
	if (m_progress >= 1.0f)
	{
		Stop();
		m_progress = 1.0f;
	}*/
}

void Animation::AddKeyFrame(float time, Pose pose)
{
	m_keyframes.emplace_back(KeyFrame(time, pose));
	_SortKeyframes();
	printf("Frames:\n");
	int i = 0;
	for each(auto frame in m_keyframes)
	{
		printf("Time[%d]:%.1f\n", i, frame.time);
		i++;
	}
}

void Animation::AddKeyFrame(KeyFrame keyframe)
{
	m_keyframes.emplace_back(keyframe);
	_SortKeyframes();
}

bool IsKeyFrameEarlier(KeyFrame& const k1, KeyFrame& const k2)
{
	if (k1.time < k2.time)
		return true;
	return false;
}

void Animation::_SortKeyframes()
{
	std::sort(m_keyframes.begin(), m_keyframes.end(),IsKeyFrameEarlier);
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

void AnimationController::PlayAnimation(std::string name)
{
	bool found = false;
	for (int i = 0; i < m_animations.size(); i++)
	{
		m_animations[i].Stop();
		if (name == m_animations[i].GetName())
		{
			if (!found)
			{
				m_animations[i].Play(true);
				found = true;
			}
			else
			{
				printf("Found another animation with the same name: %s\n", name);	
			}
		}
	}
	if (!found)
	{
		printf("Animation not found: %s\n", name);
	}
}
