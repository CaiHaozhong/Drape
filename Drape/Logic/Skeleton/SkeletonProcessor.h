#pragma once
#include "SkeletonDef.h"
class SkeletonProcessor
{
public:
	virtual void process(Skeleton* skeleton) = 0;
};

