#pragma once

#include "ApplicationBase.h"

class RYU_API FRyuGame : public FApplicationBase
{
	typedef FApplicationBase Super;

public:

    FRyuGame();
    virtual ~FRyuGame();

protected:

    virtual bool InitWindow() override;
	virtual bool InitVulkan() override;

};
