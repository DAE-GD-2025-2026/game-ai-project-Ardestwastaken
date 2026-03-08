#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	: WeightedBehaviors(WeightedBehaviors)
{
};

SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedOutput = {};
	float TotalWeight = 0.f;

	for (WeightedBehavior& wb : WeightedBehaviors)
	{
		if (wb.pBehavior && wb.Weight > 0.f)
		{
			SteeringOutput BehaviorOutput = wb.pBehavior->CalculateSteering(DeltaT, Agent);
			BlendedOutput.LinearVelocity += BehaviorOutput.LinearVelocity * wb.Weight;
			BlendedOutput.AngularVelocity += BehaviorOutput.AngularVelocity * wb.Weight;
			TotalWeight += wb.Weight;
		}
	}

	if (TotalWeight > 0.f)
	{
		BlendedOutput.LinearVelocity /= TotalWeight;
		BlendedOutput.AngularVelocity /= TotalWeight;
	}

	BlendedOutput.IsValid = true;
	return BlendedOutput;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(), WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		});

	if (it != WeightedBehaviors.end())
		return &it->Weight;

	return nullptr;
}

SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	return Steering;
}