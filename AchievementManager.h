#pragma once

#include <array>
#include <cstddef>

// アプリを起動している間の実績達成状態をまとめて保持する。
// 後でセーブ機能を入れる際も、このクラスだけを保存対象にすればよい。
enum class AchievementId {
	FirstPlay,
	Run10Meters,
	CollectFiveSouls,
	Count,
};

class AchievementManager {
public:
	static AchievementManager& GetInstance() {
		static AchievementManager instance;
		return instance;
	}

	void Unlock(AchievementId id) {
		unlocked_[static_cast<size_t>(id)] = true;
	}

	bool IsUnlocked(AchievementId id) const {
		return unlocked_[static_cast<size_t>(id)];
	}

	int GetUnlockedCount() const {
		int count = 0;
		for (const bool isUnlocked : unlocked_) {
			if (isUnlocked) { ++count; }
		}
		return count;
	}

	static constexpr int GetAchievementCount() {
		return static_cast<int>(AchievementId::Count);
	}

private:
	std::array<bool, static_cast<size_t>(AchievementId::Count)> unlocked_{};
};
