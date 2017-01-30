#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Collider.h"

class HexagonCollider : public Collider
{
private:
	Hexagon hex;
	float height;
public:
	HexagonCollider(GameObject* o, float v, float height);
	Hexagon GetWorldHex();
	void Update(float dt) override;

};