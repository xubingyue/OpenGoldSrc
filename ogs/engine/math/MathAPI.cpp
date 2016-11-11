#include "math/Math.h"

#ifndef OGS_NULL_MATH_API_IMPL
float Math_VecToYaw(const float *rgflVector)
{
};

void Math_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
};

void Math_MakeVectors(const float *rgflVector)
{
};

void Math_AngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
};

int32 Math_RandomLong(int32 lLow, int32 lHigh)
{
};

float Math_RandomFloat(float flLow, float flHigh)
{
};
#else // OGS_NULL_MATH_API_IMPL
float Math_VecToYaw(const float *rgflVector)
{
	return 0.0f;
};

void Math_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
};

void Math_MakeVectors(const float *rgflVector)
{
};

void Math_AngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
};

int32 Math_RandomLong(int32 lLow, int32 lHigh)
{
	return 0;
};

float Math_RandomFloat(float flLow, float flHigh)
{
	return 0.0f;
};
#endif // OGS_NULL_MATH_API_IMPL