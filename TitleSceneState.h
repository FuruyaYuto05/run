#pragma once

class TitleScene;

// タイトル画面内の各フェーズを表す状態インターフェース。
class TitleSceneState {
public:
	virtual ~TitleSceneState() = default;
	virtual void Enter(TitleScene& scene) = 0;
	virtual void Update(TitleScene& scene) = 0;
	virtual const char* GetName() const = 0;
	virtual bool IsExit() const { return false; }
};

class TitleIntroState final : public TitleSceneState {
public:
	void Enter(TitleScene& scene) override;
	void Update(TitleScene& scene) override;
	const char* GetName() const override { return "Intro"; }
};

class TitleIdleState final : public TitleSceneState {
public:
	void Enter(TitleScene& scene) override;
	void Update(TitleScene& scene) override;
	const char* GetName() const override { return "Idle"; }
};

class TitleRestartFadeState final : public TitleSceneState {
public:
	void Enter(TitleScene& scene) override;
	void Update(TitleScene& scene) override;
	const char* GetName() const override { return "RestartFade"; }
};

class TitleExitState final : public TitleSceneState {
public:
	void Enter(TitleScene& scene) override;
	void Update(TitleScene& scene) override;
	const char* GetName() const override { return "Exit"; }
	bool IsExit() const override { return true; }
};
