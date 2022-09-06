#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <random>
#include <functional>
#include <algorithm>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include "Model.h"
#include "Player.h"

// SIMULATION ONE

// function for resizing the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



const unsigned int width = 900;
const unsigned int height = 900;

//timing
double currentFrame;
double deltaTime = 0.0f;
double lastFrame = 0.0f;

GLfloat lightVertices[] =
{ //     COORDINATES     //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};




int main()
{
	std::vector<Model> models;
										 // scale 			//translation	      //colors 
	Model temp("models/ssphere.ply", { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.00f, 0.0f }, { 1.0f, 0.2f, 0.5f});
	Model temp2("models/ssphere.ply", { .75f, .75f, .75f }, { 0.0f, 1.5f, 0.0f }, { 0.4f, 0.2f, 1.0f});
	Model temp3("models/ssphere.ply", { .5f, .5f, .5f }, { 0.0f, 2.5f, 0.0f }, { 0.7f, 1.0f, 0.2f });
	//Model temp2("models/tetrahedron.ply", { 5.0f, 5.0f, 5.0f }, { 1.0f, 0.2f, 0.5f });
	models.emplace_back(temp);
	models.emplace_back(temp2);
	models.emplace_back(temp3);
	//models.emplace_back(temp2);


			// first model in static_models container is going to be the plane
	std::vector<Model> static_models;   // scale 			//translation	      //colors 
	Model plane("models/plane.ply", { 500.0f, 500.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	static_models.emplace_back(plane);

			//skybox (going to utilize the regular shader for this)
	Model skybox("models/cube.ply", { 500.0f, 500.0f, 500.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });

	//random float generator
	std::mt19937 generator;
	std::uniform_real_distribution<float> uniform_distribution_scaling(5.0f, 20.0f);

	//generate static models
	for (int i = 0; i < 100; i++)
	{
		//get random floats
		float randX = uniform_distribution_scaling(generator);
		float randY = uniform_distribution_scaling(generator);
		float randZ = uniform_distribution_scaling(generator);

		static_models.emplace_back(Model("models/ssphere.ply", { randX, randY, randZ }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
		std::cout << "RANDOMS: " << randX << "\t" << randY << "\t" << randZ << "\t" << std::endl;
	}

	//create player
	Player player(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//Model plane("models/plane.ply", { 0.0f,0.0f,0.0f }, { 0.4f,0.7f,0.4f });
	//models.emplace_back(plane);

	

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window name 
	const char* window_name = "3D Fun!";

	// Create a GLFWwindow object of width x height pixels, naming it with value of window_name
	GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);

	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	std::vector<VAO> VAOS;

	for (int i = 0; i < models.size(); i++)
	{
		VAOS.emplace_back();
		VAOS.back().Bind();
		VBO tempVBO(&models.at(i).vertices.front(), models.at(i).vertices.size() * sizeof(GLfloat));
		EBO tempEBO(&models.at(i).indices.front(), models.at(i).indices.size() * sizeof(GLuint));

		VAOS.back().LinkAttrib(tempVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
		VAOS.back().LinkAttrib(tempVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		VAOS.back().LinkAttrib(tempVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		VAOS.back().LinkAttrib(tempVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		VAOS.back().Unbind();
		tempVBO.Unbind();
		tempEBO.Unbind();
	}

	std::vector<VAO> VAOS_static;
	std::vector<Shader> static_model_shaders;

	for (int i = 0; i < static_models.size(); i++)
	{
		Shader tempShader("plane.vert", "plane.frag");
		// create a shader program for each model (in case we need to rotate them or transform them in some way they will be independent)
		static_model_shaders.emplace_back(tempShader);

		VAOS_static.emplace_back();
		VAOS_static.back().Bind();
		VBO tempVBO(&static_models.at(i).vertices.front(), static_models.at(i).vertices.size() * sizeof(GLfloat));
		EBO tempEBO(&static_models.at(i).indices.front(), static_models.at(i).indices.size() * sizeof(GLuint));

		VAOS_static.back().LinkAttrib(tempVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
		VAOS_static.back().LinkAttrib(tempVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		VAOS_static.back().LinkAttrib(tempVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		VAOS_static.back().LinkAttrib(tempVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		VAOS_static.back().Unbind();
		tempVBO.Unbind();
		tempEBO.Unbind();
	}











			//skybox
	//make shader object
	Shader skyboxShader("plane.vert", "plane.frag");
	// create and bind
	VAO skyboxVAO;
	skyboxVAO.Bind();
	// VBO AND EBO
	VBO skyboxVBO(&skybox.vertices.front(), skybox.vertices.size() * sizeof(GLfloat));
	EBO skyboxEBO(&skybox.indices.front(), skybox.indices.size() * sizeof(GLuint));
	//link attributes
	skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	skyboxVAO.LinkAttrib(skyboxVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	skyboxVAO.LinkAttrib(skyboxVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	skyboxVAO.LinkAttrib(skyboxVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	//Unbind all
	skyboxVAO.Unbind();
	skyboxVBO.Unbind();
	skyboxEBO.Unbind();












	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");
	// Generates Vertex Array Object and binds it
	VAO lightVAO;
	lightVAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	// Generates Element Buffer Object and links it to indices
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	// Links VBO attributes such as coordinates and colors to VAO
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();

	// static model vectors for position and matrix
	std::vector<glm::vec3> staticModelPositions;
	std::vector<glm::mat4> staticModels;

	// initialize all static_model object positions
	for (int i = 0; i < static_models.size(); i++)
	{
		staticModelPositions.emplace_back();
		staticModelPositions.back() = glm::vec3(0.0f, 0.0f, 0.0f);
		staticModels.emplace_back();
		staticModels.at(i) = glm::mat4(1.0f);
		staticModels.back() = glm::translate(staticModels.back(), staticModelPositions.back());
	}

	//rotate the first model in the model container (which is the plane)
	staticModels.at(0) = glm::rotate(staticModels.at(0), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//move tetrahedrons to random locations (start at the second index because the first is the plane)
		//the distribution we will be using
	std::uniform_real_distribution<float> uniform_distribution_translating(20.0f, 80.0f);
	// creating gen object for use generating a random bool
	auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine()); 
	for (int i = 1; i < static_models.size(); i++)
	{
		//generate random floats between -45 and 45
		bool b1 = gen();
		bool b2 = gen();

		float translateRandomX = uniform_distribution_translating(generator);
		float translateRandomZ = uniform_distribution_translating(generator);

		if (b1)
		{
			translateRandomX = -translateRandomX;
		}
		if (b2)
		{
			translateRandomZ = -translateRandomZ;
		}

		staticModelPositions.at(i) = glm::vec3(translateRandomX, 0.0f, translateRandomZ);
		staticModels.at(i) = glm::translate(staticModels.at(i), staticModelPositions.at(i));
	}









	//skybox object position
	glm::vec3 skyboxPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 skyboxModel = glm::mat4(1.0f);
	skyboxModel = glm::translate(skyboxModel, skyboxPos);









	//pyramid object position
	glm::vec3 pyramidPos = glm::vec3(0.0f, 1.0f, 2.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	//light object position and color
	glm::vec3 lightColor = glm::vec3(1.0f, 0.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 60.0f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);


	//light object
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform3f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);


	//object
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Texture
	Texture brickTex("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	//set texture for object
	brickTex.texUnit(shaderProgram, "tex0", 0);


	std::vector<Texture> textures;

	//static objects
	for (int i = 0; i < static_models.size(); i++)
	{
		//activate first before setting uniforms
		static_model_shaders.at(i).Activate();
																			// static model load model, then load lightColor, then load lightPos for each static_model
		glUniformMatrix4fv(glGetUniformLocation(static_model_shaders.at(i).ID, "model"), 1, GL_FALSE, glm::value_ptr(staticModels.at(i)));
		glUniform4f(glGetUniformLocation(static_model_shaders.at(i).ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, 1.0f);
		glUniform3f(glGetUniformLocation(static_model_shaders.at(i).ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		//create texture objects
		textures.emplace_back(Texture("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
		textures.back().texUnit(static_model_shaders.at(i), "tex0", 0);
	}












	//skybox texture
	Texture stars("stars.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	//set texture for object

	//skybox setting initial uniform values
	skyboxShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(skyboxModel));
	glUniform4f(glGetUniformLocation(skyboxShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, 1.0f);
	glUniform3f(glGetUniformLocation(skyboxShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	stars.texUnit(skyboxShader, "tex0", 0);













	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));


	bool colorfulLight = false;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		// per-frame time logic
		// -------------------

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Specify the color of the background
		glClearColor(0.17f, 0.15f, 0.15f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs, passes on deltaTime for timing
		camera.Inputs(window, deltaTime);

		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		player.changeInPosition.x = 0.0f;
		player.changeInPosition.z = 0.0f;

			float objectSpeed = 9.0f;
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				player.changeInPosition.x += deltaTime * objectSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				player.changeInPosition.x -= deltaTime * objectSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				player.changeInPosition.z -= deltaTime * objectSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				player.changeInPosition.z += deltaTime * objectSpeed;
			}

			player.Update(deltaTime);
			std::cout << player.position.y << std::endl;

			// jumping 
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			{
				player.velocity.y = 15.0f;
				player.acceleration.y = -40.0f;
			}

			if (player.position.y < 0.0f)
			{
				player.position.y = 0.01f;
				player.changeInPosition.y = 0.0f;
				player.velocity.y = 0.0f;
				player.acceleration.y = 0.0f;
			}
			pyramidModel = glm::translate(pyramidModel, glm::vec3(player.changeInPosition.x, player.changeInPosition.y, player.changeInPosition.z));


			if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
			{
					lightColor.x = 1.0f;
					lightColor.y = 1.0f;
					lightColor.z = 1.0f;
					colorfulLight = false;
			}
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{
				colorfulLight = true;
			}

			if (colorfulLight)
			{
				lightColor.x = std::max((float)sin(glfwGetTime()), 0.0f);
				lightColor.y = std::max((float)cos(glfwGetTime()), 0.0f);
				lightColor.z = std::max((float)sin(glfwGetTime() + 3.14159f), 0.0f);
			}

		// Tells OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Exports the camera Position to the Fragment Shader for specular lighting
		
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		//glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightDirection"), -0.2f, -1.0f, -0.3f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		// Export the camMatrix to the Vertex Shader of the pyramid
		camera.Matrix(shaderProgram, "camMatrix");
		// Binds texture so that is appears in rendering
		brickTex.Bind();

		// draw all models
		for (int i = 0; i < models.size(); i++)
		{
			VAOS.at(i).Bind();
			glDrawElements(GL_TRIANGLES, models.at(i).indices.size(), GL_UNSIGNED_INT, 0);
			VAOS.at(i).Unbind();
		}
		

		//draw all static models (each with a different shader and matrix)
		for (int i = 0; i < static_models.size(); i++)
		{
																				//activate shader for current model
			// Tells OpenGL which Shader Program we want to use
			static_model_shaders.at(i).Activate();
			// Exports the camera Position to the Fragment Shader for specular lighting

			glUniform3f(glGetUniformLocation(static_model_shaders.at(i).ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
			glUniformMatrix4fv(glGetUniformLocation(static_model_shaders.at(i).ID, "model"), 1, GL_FALSE, glm::value_ptr(staticModels.at(i)));
			glUniform4f(glGetUniformLocation(static_model_shaders.at(i).ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, 1.0f);
			// Export the camMatrix to the Vertex Shader of the pyramid
			camera.Matrix(static_model_shaders.at(i), "camMatrix");
			// Binds texture so that is appears in rendering
			textures.at(i).Bind();

			VAOS_static.at(i).Bind();
			glDrawElements(GL_TRIANGLES, static_models.at(i).indices.size(), GL_UNSIGNED_INT, 0);
			VAOS_static.at(i).Unbind();
		}





		//draw skybox
		skyboxShader.Activate();
		glUniform3f(glGetUniformLocation(skyboxShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(skyboxModel));
		glUniform4f(glGetUniformLocation(skyboxShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, 1.0f);

		camera.Matrix(skyboxShader, "camMatrix");

		stars.Bind();

		skyboxVAO.Bind();
		glDrawElements(GL_TRIANGLES, skybox.indices.size(), GL_UNSIGNED_INT, 0);
		skyboxVAO.Unbind();




		
		// Tells OpenGL which Shader Program we want to use
		lightShader.Activate();
		// Export the camMatrix to the Vertex Shader of the light cube
		camera.Matrix(lightShader, "camMatrix");
		//update lightColor uniform
		glUniform3f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		// Bind the VAO so OpenGL knows to use it
		lightVAO.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//std::cout << "sizeof(lightIndices) / sizeof(int)\t\t-\t\t" << sizeof(lightIndices) / sizeof(int) << std::endl;
		lightVAO.Unbind();
		

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();

	}

	// Delete all the objects we've created
	for (int i = 0; i < models.size(); i++)
	{
		VAOS.at(i).Delete();
	}

	brickTex.Delete();
	shaderProgram.Delete();
	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();
	lightShader.Delete();

	skyboxShader.Delete();
	stars.Delete();
	skyboxVAO.Delete();
	skyboxVBO.Delete();
	skyboxEBO.Delete();

	for (int i = 0; i < static_model_shaders.size(); i++)
	{
		static_model_shaders.at(i).Delete();
	}

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}