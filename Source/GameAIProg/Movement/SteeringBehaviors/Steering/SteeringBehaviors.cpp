#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = Target.Position - Agent.GetPosition();
	return steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = Agent.GetPosition() - Target.Position;
	return steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	float TargetRadius{ 100 };
	float SlowRadius{ 500 };

	FVector2f agentPos{ Agent.GetPosition() };
	FVector2f targetPos{ Target.Position };
	float ActualRadius{ FVector2f::Distance(agentPos, targetPos) };

	float SpeedPercentage = (ActualRadius - TargetRadius) / (SlowRadius - TargetRadius);

	SteeringOutput steering{};
	steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Agent.SetMaxLinearSpeed(SpeedPercentage * Agent.GetOriginalMaxLinearVelocity());
	return steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.IsValid = true;

	FVector const AgentPos = Agent.GetActorLocation();
	FVector2D const Direction = FVector2D(Target.Position.X - AgentPos.X, Target.Position.Y - AgentPos.Y).GetSafeNormal();

	FVector const Forward = Agent.GetActorForwardVector();
	FVector2D const CurrentDirection = FVector2D(Forward.X, Forward.Y).GetSafeNormal();

	float const Dot = FVector2D::DotProduct(CurrentDirection, Direction);
	float const Cross = FVector2D::CrossProduct(CurrentDirection, Direction);

	steering.AngularVelocity = (Dot > 0.999f) ? 0.0f : ((Cross > 0) ? 1.0f : -1.0f);
	return steering;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};

	FVector2f agentPos{ Agent.GetPosition() };
	FVector2f targetPos{ Target.Position };
	float distance = FVector2f::Distance(targetPos, agentPos);
	float speed = Agent.GetOriginalMaxLinearVelocity();
	float timeToTarget = (speed > 0) ? (distance / speed) : 0.0f;

	FVector2D predictedPosition = Target.Position + (Target.LinearVelocity * timeToTarget);
	steering.LinearVelocity = predictedPosition - Agent.GetPosition();
	return steering;
}

// Evade with radius:
// Returns IsValid = false when the threat is OUTSIDE the EvadeRadius,
// allowing PrioritySteering to fall through to the next behavior (e.g. Wander).
SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};

	FVector2f agentPos{ Agent.GetPosition() };
	FVector2f targetPos{ Target.Position };
	float distance = FVector2f::Distance(targetPos, agentPos);

	// Outside evade radius ? mark invalid so PrioritySteering falls through
	if (EvadeRadius > 0.f && distance > EvadeRadius)
	{
		steering.IsValid = false;
		return steering;
	}

	float speed = Agent.GetOriginalMaxLinearVelocity();
	float timeToTarget = (speed > 0) ? (distance / speed) : 0.0f;

	FVector2D predictedPosition = Target.Position + (Target.LinearVelocity * timeToTarget);
	steering.LinearVelocity = Agent.GetPosition() - predictedPosition;
	steering.IsValid = true;
	return steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	float AngleChange = FMath::RandRange(-m_MaxAngleChange, m_MaxAngleChange);
	m_WanderAngle += AngleChange;

	FVector Forward = Agent.GetActorForwardVector();
	FVector2D Forward2D = FVector2D(Forward.X, Forward.Y).GetSafeNormal();

	FVector2D AgentPos = Agent.GetPosition();
	FVector2D CircleCenter = AgentPos + Forward2D * m_OffsetDistance;

	FVector2D PointOnCircle = FVector2D(
		CircleCenter.X + m_Radius * FMath::Cos(m_WanderAngle),
		CircleCenter.Y + m_Radius * FMath::Sin(m_WanderAngle)
	);

	Target.Position = PointOnCircle;
	SteeringOutput seekOutput{};
	seekOutput.LinearVelocity = Target.Position - Agent.GetPosition();
	return seekOutput;
}