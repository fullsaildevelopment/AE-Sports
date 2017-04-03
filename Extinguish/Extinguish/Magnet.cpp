#include "Magnet.h"
#include "BallController.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Crosse.h"

//structors
Magnet::Magnet()
{
	SetDuration(8.0f);
}

void Magnet::Init(GameObject* obj)
{
}

//basic
void Magnet::Activate()
{
	//make magnet range bigger
	crosseC = GetPlayer()->GetGameObject()->GetTransform()->GetChild(0)->GetChild(0)->GetGameObject()->GetComponent<Crosse>();

	originalMultiplier = crosseC->GetMagnetMultiplier();
	crosseC->SetMagnetMultiplier(magnetRangeMultiplier);
	printf("Magnet activated\n");
}

void Magnet::Deactivate()
{
	PowerUp::Deactivate();

	//make magnet range back to normal
	crosseC->SetMagnetMultiplier(originalMultiplier);

	printf("Magnet deactivated\n");
}