#include "SuperJump.h"
#include "PlayerController.h"

SuperJump::SuperJump() : PowerUp()
{
	SetDuration(2.0f);
}

void SuperJump::Activate()
{
	PlayerController* player = GetPlayer();

	originalMultiplier = player->GetJumpMultiplier();
	player->SetJumpMultiplier(jumpMultiplier);
}

void SuperJump::Deactivate()
{
	PlayerController* player = GetPlayer();

	player->SetJumpMultiplier(originalMultiplier);
}