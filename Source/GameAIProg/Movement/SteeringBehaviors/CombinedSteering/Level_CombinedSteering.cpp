#include "Level_CombinedSteering.h"
#include "imgui.h"

ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();

	WandererAgent = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass, FVector{ 400.f, 0.f, 90.f }, FRotator::ZeroRotator);
	if (IsValid(WandererAgent))
	{
		pWander_Threat = new Wander();
		WandererAgent->SetSteeringBehavior(pWander_Threat);
		WandererAgent->SetIsAutoOrienting(true);
	}

	SeekerAgent = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass, FVector{ -400.f, 0.f, 90.f }, FRotator::ZeroRotator);
	if (IsValid(SeekerAgent))
	{
		pSeek = new Seek();
		SeekerAgent->SetSteeringBehavior(pSeek);
		SeekerAgent->SetIsAutoOrienting(true);
	}

	EvaderAgent = GetWorld()->SpawnActor<ASteeringAgent>(
		SteeringAgentClass, FVector{ 0.f, 400.f, 90.f }, FRotator::ZeroRotator);
	if (IsValid(EvaderAgent))
	{
		pEvade = new Evade();
		pWander_Evader = new Wander();

		pEvade->SetEvadeRadius(EvadeRadius);

		pPrioritySteering = new PrioritySteering(
			std::vector<ISteeringBehavior*>{ pEvade, pWander_Evader });

		EvaderAgent->SetSteeringBehavior(pPrioritySteering);
		EvaderAgent->SetIsAutoOrienting(true);
		EvaderAgent->SetDebugRenderingEnabled(true);
	}
}

void ALevel_CombinedSteering::BeginDestroy()
{
	delete pPrioritySteering; pPrioritySteering = nullptr;
	delete pEvade;            pEvade = nullptr;
	delete pWander_Evader;    pWander_Evader = nullptr;
	delete pSeek;             pSeek = nullptr;
	delete pWander_Threat;    pWander_Threat = nullptr;

	Super::BeginDestroy();
}

void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region UI
	{
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target (for Seeker)");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Combined Steering");
		ImGui::Spacing();
		ImGui::Text("Yellow = Wanderer (threat)");
		ImGui::Text("Blue   = Seeker (follows mouse)");
		ImGui::Text("Red    = Evader (Wander until threatened, then Evade)");
		ImGui::Spacing();

		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
			if (IsValid(WandererAgent)) WandererAgent->SetDebugRenderingEnabled(CanDebugRender);
			if (IsValid(SeekerAgent))   SeekerAgent->SetDebugRenderingEnabled(CanDebugRender);
			if (IsValid(EvaderAgent))   EvaderAgent->SetDebugRenderingEnabled(CanDebugRender);
		}

		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Priority Steering Settings");
		ImGui::Spacing();

		ImGuiHelpers::ImGuiSliderFloatWithSetter("Evade Radius",
			EvadeRadius, 50.f, 1000.f,
			[this](float InVal)
			{
				EvadeRadius = InVal;
				if (pEvade) pEvade->SetEvadeRadius(EvadeRadius);
			}, "%.0f");

		ImGui::End();
	}
#pragma endregion

	if (pSeek)
		pSeek->SetTarget(MouseTarget);

	if (pEvade && IsValid(WandererAgent))
	{
		FTargetData Threat;
		Threat.Position = WandererAgent->GetPosition();
		Threat.Orientation = WandererAgent->GetRotation();
		Threat.LinearVelocity = WandererAgent->GetLinearVelocity();
		Threat.AngularVelocity = WandererAgent->GetAngularVelocity();
		pEvade->SetTarget(Threat);
	}
}