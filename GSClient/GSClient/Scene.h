#pragma once

class CScene
{
public:
	CScene();
	virtual ~CScene();

	// just proto, fill with d3d obj
	virtual void Update(double elapsedTime) {}
	virtual void Draw() {}

public:
	virtual void SendDataToNextScene(void* context) {}
};


class CNullScene : public CScene
{
public :
	CNullScene();
	~CNullScene();

	virtual void Update(double elapsedTime) override {}
	virtual void Draw() override {}

public:
	virtual void SendDataToNextScene(void* context) override {}
};