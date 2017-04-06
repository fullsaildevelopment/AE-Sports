#include "SuperJump.h"
#include "PlayerController.h"
#include "PowerUp.h"

//structors//
SuperJump::SuperJump() : PowerUp()
{
	SetDuration(8.0f);
}

//basic//
void SuperJump::Activate()
{
	PlayerController* player = GetPlayer();

	originalMultiplier = player->GetJumpMultiplier();
	player->SetJumpMultiplier(jumpMultiplier);
}

void SuperJump::Deactivate()
{
	PowerUp::Deactivate();

	PlayerController* player = GetPlayer();

	player->SetJumpMultiplier(originalMultiplier);
}

//getters//
std::string SuperJump::GetName()
{
	return name;
}