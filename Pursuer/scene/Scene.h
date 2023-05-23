#pragma once

class Scene
{
public:
	enum SCENE
	{
		Title,
		Tutorial,
		MainGame,
		GameOver,
		GameClear,
	};

	Scene() {};

	virtual ~Scene() {};

	// Initialize
	virtual void Initialize() = 0;

	// Update
	virtual void Update() = 0;

	// Pre Rendering
	virtual void PreDraw() = 0;

	// Post Rendering
	virtual void PostDraw() = 0; // If sprite drawing is included in PostDraw, the sprite will be drawn on top of the object.

	// Get endGame flag
	inline bool GetSceneStatus()
	{
		return endScene;
	}

	// Proceed to next scene
	inline SCENE NextScene()
	{
		return nextScene;
	}
protected:
	// Set the endScene flag to true
	inline void SceneShutdown()
	{
		endScene = true;
	}

	// endGame flag
	bool endScene;

	// Scene transition start flag
	bool nextFeed;

	// Next scene
	SCENE nextScene;
};