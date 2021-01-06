#pragma once
#include "Common/d3dApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Object.h"

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

	virtual void Update(double elapsedTime) override;
	virtual void Draw() override {}

public:
	virtual void SendDataToNextScene(void* context) override {}
};


