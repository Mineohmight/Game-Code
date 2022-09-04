#include "Player.h"

Player::Player(glm::vec3 P0, glm::vec3 V0, glm::vec3 AC)
{
	position = P0;
	velocity = V0;
	acceleration = AC;
}

void Player::Update(float dt)
{
	// acceleration times delta time

	velocity.y += acceleration.y * dt;
	changeInPosition.y = velocity.y * dt + 0.5f * acceleration.y * dt * dt;
	position.y += changeInPosition.y;

}