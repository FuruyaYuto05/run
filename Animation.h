#pragma once

#include <vector>
#include <string>
#include <map>
#include "Math.h"

template <typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Math::Vector3>;
using KeyframeQuaternion = Keyframe<Math::Quaternion>;

Math::Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
Math::Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

struct NodeAnimation {
	std::vector<KeyframeVector3> translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVector3> scale;
};

struct Animation {
	float duration; // アニメーション全体の尺 秒単位
	std::map<std::string, NodeAnimation> nodeAnimations;
};

Animation LoadAnimationFile(const std::string& directoryPath, const std::  string& filename);