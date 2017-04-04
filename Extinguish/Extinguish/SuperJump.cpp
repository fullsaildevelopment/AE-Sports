#include "SuperJump.h"
#include "PlayerController.h"
#include "PowerUp.h"

SuperJump::SuperJump() : PowerUp()
{
	SetDuration(4.0f);
}

void SuperJump::Activate()
{
	PlayerController* player = GetPlayer();

	originalMultiplier = player->GetJumpMultiplier();
	player->SetJumpMultiplier(jumpMultiplier);

	//play a sound indicating you picked it up
}

void SuperJump::Deactivate()
{
	PowerUp::Deactivate();

	PlayerController* player = GetPlayer();

	player->SetJumpMultiplier(originalMultiplier);
}


std::string SuperJump::GetName()
{
	return name;
}