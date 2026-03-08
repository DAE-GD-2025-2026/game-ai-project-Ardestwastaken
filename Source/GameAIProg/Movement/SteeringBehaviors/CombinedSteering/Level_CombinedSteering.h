// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombinedSteeringBehaviors.h"
#include "GameAIProg/Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Level_CombinedSteering.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_CombinedSteering : public ALevel_Base
{
	GENERATED_BODY()

public:
	ALevel_CombinedSteering();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	UPROPERTY()
	ASteeringAgent* WandererAgent{ nullptr };
	Wander* pWander_Threat{ nullptr };

	UPROPERTY()
	ASteeringAgent* SeekerAgent{ nullptr };
	See
	UPROPERTY()
	ASteeringAgent* EvaderAgent{ nullptr };
	Evade* pEvade{ nullptr };
	Wander* pWander_Evader{ nullptr };
	PrioritySteering* pPrioritySteering{ nullptr };

	bool  CanDebugRender{ false };
	float EvadeRadius{ 300.f };
};