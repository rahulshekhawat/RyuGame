#pragma once

#include "ApplicationBase.h"

class RYU_API FRyuGame : public FApplicationBase
{

public:

    FRyuGame();
    virtual ~FRyuGame();

protected:

    virtual bool InitWindow() override;
	virtual bool InitVulkan() override;

};
