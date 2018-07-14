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

void Animation::SetName(std::string name)
{
	m_animation.m_name = name;
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

bool Animation::GetLooping()
{
	return m_animation.m_looping;
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

	/*printf("Animation: %s(%d)\n",m_animation.m_name.c_str(),index);
	printf("Frames: %d\n", (int)m_animation.m_keyframes[index].second.size());
	int i = 0;
	for each(auto frame in m_animation.m_keyframes[index].second)
	{
		printf("[%d]: Time:%.1f Pose:%.1f,%.1f,%.1f\n", i, frame.time,frame.pose.Position.x, frame.pose.Position.y, frame.pose.Position.z);
		i++;
	}*/
}

void Animation::AddKeyFrame(float time, Pose pose, std::vector<int> child)
{
	AddKeyFrame(KeyFrame(time, pose), child);
}

void Animation::RemoveKeyFrame(uint keyframe, std::vector<int> child)
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
		printf("Failed to remove KeyFrame (Child doesn't exist).\n");
		return;
	}
	if (keyframe > m_animation.m_keyframes[index].second.size()-1)
	{
		printf("Failed to remove KeyFrame (KeyFrame doesn't exist).\n");
		return;
	}

	float time = m_animation.m_keyframes[index].second[keyframe].time;

	m_animation.m_keyframes[index].second.erase(m_animation.m_keyframes[index].second.begin() + keyframe);

	if (m_animation.m_end_time == time)
	{
		_CheckEndTime();
	}
}

AnimationGroup Animation::GetAnimationData()
{
	return m_animation;
}

void Animation::SetAnimationData(AnimationGroup data)
{
	m_animation = data;
}

//TODO: Make more readable
void Animation::SaveAnimation(std::string file)
{
	std::ofstream ofile(file);
	ofile << "{\n";
	ofile << "\t[Name]: " << m_animation.m_name << "\n";
	ofile << "\t[Looping]: " << m_animation.m_looping << "\n";
	ofile << "\t[Animation Components]: " << m_animation.m_keyframes.size() << "\n";
	ofile << "\t{\n";
	for each(auto animation in m_animation.m_keyframes)
	{
		ofile << "\t\t{\n";
		ofile << "\t\t\t[Hierarchy]: " << animation.first.size() << "\n";
		ofile << "\t\t\t[Child]: { ";
		for each(auto child in animation.first)
		{
			ofile << child << " ";
		}
		ofile << "}\n";

		ofile << "\t\t\t[Frames]: " << animation.second.size() << "\n";
		ofile << "\t\t\t{\n";
		for each(auto frame in animation.second)
		{
			ofile << "\t\t\t\t{\n";
			ofile << "\t\t\t\t\t[Time]: " << frame.time << "\n";
			ofile << "\t\t\t\t\t[Position]: { " << frame.pose.Position.x << " " << frame.pose.Position.y << " " << frame.pose.Position.z << " }\n";
			ofile << "\t\t\t\t\t[Rotation]: { " << frame.pose.Rotation.w << " " << frame.pose.Rotation.x << " " << frame.pose.Rotation.y << " " << frame.pose.Rotation.z << " }\n";
			ofile << "\t\t\t\t\t[Scale]: { " << frame.pose.Scale.x << " " << frame.pose.Scale.y << " " << frame.pose.Scale.z << " }\n";
			ofile << "\t\t\t\t}\n";
		}
		ofile << "\t\t\t}\n";
		ofile << "\t\t}\n";
	}
	ofile << "\t}\n";
	ofile << "}\n";
	ofile.close();

}

bool Animation::LoadAnimation(std::string file)
{
	std::ifstream ifile(file);
	if (!ifile)
	{
		return false;
	}

	ifile.ignore(2000, ':');
	ifile >> m_animation.m_name;
	ifile.ignore(2000, ':');
	ifile >> m_animation.m_looping;
	int number_of_components;
	ifile.ignore(2000, ':');
	ifile >> number_of_components;
	m_animation.m_keyframes.clear();
	for (int i = 0; i < number_of_components; i++)
	{
		int depth_of_hierarchy;
		ifile.ignore(2000, ':');
		ifile >> depth_of_hierarchy;
		ifile.ignore(2000, '{');
		std::vector<int> hierarchy;
		for (int i = 0; i < depth_of_hierarchy; i++)
		{
			int child_id;
			ifile >> child_id;
			hierarchy.push_back(child_id);
		}

		int number_of_frames;
		ifile.ignore(2000, ':');
		ifile >> number_of_frames; 
		std::vector<KeyFrame> keyframes;
		for (int i = 0; i < number_of_frames; i++)
		{
			float time;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			ifile.ignore(2000, ':');
			ifile >> time;
			ifile.ignore(2000, '{');
			ifile >> position.x >> position.y >> position.z;
			ifile.ignore(2000, '{');
			ifile >> rotation.w >> rotation.x >> rotation.y >> rotation.z;
			ifile.ignore(2000, '{');
			ifile >> scale.x >> scale.y >> scale.z;
			keyframes.push_back(KeyFrame(time, Pose(position, rotation, scale)));
		}
		m_animation.m_keyframes.push_back({ hierarchy,  keyframes });
	}

	ifile.close();
	return true;
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

void Animation::_CheckEndTime()
{
	float end_time = 0;
	for (int i = 0; i < m_animation.m_keyframes.size(); i++)
	{
		for (int k = 0; k < m_animation.m_keyframes[i].second.size(); k++)
		{
			if (m_animation.m_keyframes[i].second[k].time > end_time)
			{
				end_time = m_animation.m_keyframes[i].second[k].time;
			}
		}
	}
	m_animation.m_end_time = end_time;
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

std::vector<Animation> AnimationController::GetAnimations()
{
	return m_animations;
}

void AnimationController::SetAnimations(std::vector<Animation> animations)
{
	m_animations = animations;
}
