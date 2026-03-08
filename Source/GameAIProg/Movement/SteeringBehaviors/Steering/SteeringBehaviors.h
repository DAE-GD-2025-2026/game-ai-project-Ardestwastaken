#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>
#include "Kismet/KismetMathLibrary.h"

class ASteeringAgent;

class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) = 0;

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }

	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{
		return static_cast<T*>(this);
	}

protected:
	FTargetData Target;
};

class Seek : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Flee : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Arrive : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Face : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Pursuit : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Evade : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;

	void SetEvadeRadius(float Radius) { EvadeRadius = Radius; }
	float GetEvadeRadius() const { return EvadeRadius; }

private:
	float EvadeRadius{ 300.f };
};

class Wander : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
private:
	float m_OffsetDistance{ 600.f };
	float m_Radius{ 300.f };
	float m_MaxAngleChange{ FMath::DegreesToRadians(45.f) };
	float m_WanderAngle{ 0.f };
};