#pragma once
#include "Scene.h"

class CTitleScene : public CScene
{
public:
	CTitleScene();
	~CTitleScene();

	virtual void Update(double elapsedTime) override;
	virtual void Draw() override;

public:
	virtual void SendDataToNextScene(void* context) override {}

};

