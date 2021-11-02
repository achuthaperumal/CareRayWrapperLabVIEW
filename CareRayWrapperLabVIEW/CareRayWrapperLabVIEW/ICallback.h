#ifndef __CALLBACKHEAD_530_H__
#define __CALLBACKHEAD_530_H__

#include "Structure.h"

using namespace CareRay;

class ICallback
{
public:
	ICallback(){};
	virtual ~ICallback(){};
	void virtual Process(int nEventID, CR_Event* pEventParam)
	{
		//Customzed
	}

protected:

private:
};

#endif
