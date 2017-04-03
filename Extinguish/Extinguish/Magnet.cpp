#include "Magnet.h"
#include "BallController.h"
#include "GameObject.h"

//structors
Magnet::Magnet()
{
	SetDuration(8.0f);
}

void Magnet::Init(GameObject* obj)
{
	//cache must be done in init
	ballC = GetGameObject()->FindGameObject("GameBall")->GetComponent<BallController>();
}

//basic
void Magnet::Activate()
{
	//make magnet range bigger
	originalMultiplier = ballC->GetMagnetMultiplier();
	ballC->SetMagnetMultiplier(magnetRangeMultiplier);
	printf("Magnet activated\n");
}

void Magnet::Deactivate()
{
	PowerUp::Deactivate();

	//make magnet range back to normal
	ballC->SetMagnetMultiplier(originalMultiplier);

	printf("Magnet deactivated\n");
}