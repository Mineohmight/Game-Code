#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include <vector>
#include<glm/glm.hpp>

class Player 
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 changeInPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	Player(glm::vec3 P0, glm::vec3 V0, glm::vec3 AC);

	void Update(float dt);
};

#endif