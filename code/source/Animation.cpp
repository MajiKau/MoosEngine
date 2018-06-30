#include "code/headers/Animation.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_interpolation.hpp>
#include <gtx/compatibility.hpp>

#include <fstream>

Animation::Animation(std::string name)
{
	m_animation.m_name = name; 
	m_parent = NULL;
}
Animation::Animation()
{
	m_parent = NULL;
}

//Animation::Animation(std::string name)
//{
//	m_name = name;
//	m_current_frame = 0;
//	m_current_time = 0;
//	m_target = NULL;
//	m_progress = 0.0f;
//	m_start_pose = Pose();
//	m_end_pose = Pose();
//	m_playing = false;
//	m_looping = false;
//}

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

//Animation::Animation()
//{
//	m_name = ""; 
//	m_current_frame = 0;
//	m_current_time = 0;
//	m_target = NULL;
//	m_progress = 0.0f;
//	m_start_pose = Pose();
//	m_end_pose = Pose();
//	m_playing = false; 
//	m_looping = false;
//}

void Animation::SetTarget(Entity * target)
{
	m_parent = target;
}

Entity * Animation::GetTarget()
{
	return m_parent;
}

std::string Animation::GetName()
{
	return m_animation.m_name;
}

//void Animation::SetTransformation(Pose start, Pose end)
//{
//	m_start_pose = start;
//	m_end_pose = end;
//}

void Animation::SetLooping(bool loops)
{
	m_animation.m_looping = loops;
}

void Animation::Play(bool start_from_beginning)
{
	if (m_animation.m_keyframes.size() == 0)
	{
		printf("Trying to play animation with no keyframes!\n");
		return;
	}
	m_animation.m_playing = true;
	if (start_from_beginning)
	{
		m_animation.m_current_time = 0.0f;
	}
}

void Animation::Stop()
{
	m_animation.m_playing = false;
}

void Animation::Update(float deltaTime)
{
	if (!m_animation.m_playing || m_parent==NULL) return;

	m_animation.m_current_time += deltaTime;

	for (int i = 0; i < m_animation.m_keyframes.size(); i++)
	{
		int last_frame = (int)m_animation.m_keyframes[i].second.size() - 1;
		int current_frame = 0;
		Entity* target = m_parent;

		for each (auto index in m_animation.m_keyframes[i].first)
		{
			if(target != NULL)
			target = target->GetChild(index);
		}	

		if (target == NULL)
		{
			printf("Missing child for animation!\n");
			continue;
		}

		if (last_frame == 0)
		{
			Pose pose = m_animation.m_keyframes[i].second[last_frame].pose;
			target->SetLocalPosition(pose.Position);
			target->SetLocalRotation(pose.Rotation);
			target->SetLocalScale(pose.Scale);
			continue;
		}


		if (m_animation.m_current_time > m_animation.m_end_time)
		{
			if (m_animation.m_looping)
			{
				m_animation.m_current_time -= m_animation.m_end_time;
			}
			else
			{
				m_animation.m_current_time = m_animation.m_end_time;
				m_animation.m_playing = false;
			}
			current_frame = 0;
		}
		while (m_animation.m_current_time > m_animation.m_keyframes[i].second[current_frame + 1].time)
		{
			current_frame++; 
			if (current_frame >= last_frame)
			{
				Pose pose = m_animation.m_keyframes[i].second[last_frame].pose;
				target->SetLocalPosition(pose.Position);
				target->SetLocalRotation(pose.Rotation);
				target->SetLocalScale(pose.Scale);
				goto END_LOOP;
			}
		}
		

		Pose start_pose = m_animation.m_keyframes[i].second[current_frame].pose;
		Pose end_pose = m_animation.m_keyframes[i].second[current_frame + 1].pose;
		float progress = (m_animation.m_current_time - m_animation.m_keyframes[i].second[current_frame].time) 
			/ (m_animation.m_keyframes[i].second[current_frame + 1].time - m_animation.m_keyframes[i].second[current_frame].time);

		glm::vec3 new_position = glm::lerp(start_pose.Position, end_pose.Position, progress);
		glm::quat new_rotation = glm::slerp(start_pose.Rotation, end_pose.Rotation, progress);
		glm::vec3 new_scale = glm::lerp(start_pose.Scale, end_pose.Scale, progress);
		target->SetLocalPosition(new_position);
		target->SetLocalRotation(new_rotation);
		target->SetLocalScale(new_scale);
		END_LOOP:;
	}
}

void Animation::AddKeyFrame(KeyFrame keyframe, std::vector<int> child)
{
	int index = -1;
	for (int i = 0; i < m_animation.m_keyframes.size(); i++)
	{
		if (child == m_animation.m_keyframes[i].first)
		{
			index = i;
		}
	}
	if (index == -1)
	{
		m_animation.m_keyframes.push_back({ child,{ keyframe } });
		index = (int)m_animation.m_keyframes.size() - 1;
	}
	else
	{
		m_animation.m_keyframes[index].second.push_back(keyframe);
	}
	_SortKeyframes(index);

	if (keyframe.time > m_animation.m_end_time)
	{
		m_animation.m_end_time = keyframe.time;
	}

	printf("Animation: %s(%d)\n",m_animation.m_name.c_str(),index);
	printf("Frames: %d\n", (int)m_animation.m_keyframes[index].second.size());
	int i = 0;
	for each(auto frame in m_animation.m_keyframes[index].second)
	{
		printf("[%d]: Time:%.1f Pose:%.1f,%.1f,%.1f\n", i, frame.time,frame.pose.Position.x, frame.pose.Position.y, frame.pose.Position.z);
		i++;
	}
}

void Animation::AddKeyFrame(float time, Pose pose, std::vector<int> child)
{
	AddKeyFrame(KeyFrame(time, pose), child);
}

//TODO: Make more readable
void Animation::SaveAnimation(std::string file)
{
	std::ofstream ofile(file);
	ofile << "Name: " << m_animation.m_name << "\n";
	ofile << "Looping: " << m_animation.m_looping << "\n";
	ofile << "Animation Components: " << m_animation.m_keyframes.size() << "\n";
	for each(auto animation in m_animation.m_keyframes)
	{
		ofile << "Hierarchy:" << animation.first.size() << "\n";
		ofile << "Child";
		for each(auto child in animation.first)
		{
			ofile << ": " << child << " ";
		}
		ofile << "\nFrames: " << animation.second.size() << "\n";
		for each(auto frame in animation.second)
		{
			ofile << "Time: " << frame.time << " Pos:" << frame.pose.Position.x << " " << frame.pose.Position.y << " " << frame.pose.Position.z 
				<< " Rot: "	<< frame.pose.Rotation.w << " " << frame.pose.Rotation.x << " " << frame.pose.Rotation.y << " " << frame.pose.Rotation.z
				<< " Scale: " << frame.pose.Scale.x << " " << frame.pose.Scale.y << " " << frame.pose.Scale.z
				<< "\n";
		}
	}
	ofile.close();

}

//TODO: Make it work
void Animation::LoadAnimation(std::string file)
{
	/*std::ifstream ifile(file);
	ifile >> m_name;
	int number_of_frames;
	ifile >> number_of_frames;
	ifile >> m_looping;
	m_keyframes.clear();
	for (int i=0;i<number_of_frames;i++)
	{
		float time;
		glm::vec3 position;
		glm::quat rotation;
		ifile >> time >> position.x >> position.y >> position.z
			>> rotation.w >> rotation.x >> rotation.y >> rotation.z;
		m_keyframes.push_back(KeyFrame(time, Pose(position, rotation)));
	}
	ifile.close();*/
}

bool IsKeyFrameEarlier(KeyFrame& const k1, KeyFrame& const k2)
{
	if (k1.time < k2.time)
		return true;
	return false;
}

void Animation::_SortKeyframes(int index)
{
	std::sort(m_animation.m_keyframes[index].second.begin(), m_animation.m_keyframes[index].second.end(),IsKeyFrameEarlier);
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
	m_animations.push_back(animation);
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
				printf("Found another animation with the same name: %s\n", name.c_str());	
			}
		}
	}
	if (!found)
	{
		printf("Animation not found: %s\n", name.c_str());
	}
}
