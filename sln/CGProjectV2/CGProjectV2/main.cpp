//Артемкин К Р
//группа 301 ВМК
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Camera.h"
#include "Shapes.h"
#include "Shader.h"
#include "State.h"

//utilities:
State state = State();
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
unsigned int LoadCubemap(char const ** address);
unsigned int loadTexture(char const * path);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void RenderLamp(Shader shader, unsigned int VAO);
void renderCubemap(Shader shader, unsigned int VAO, unsigned int buffer);
void RenderSkyBox(Shader shader, unsigned int VAO, unsigned int texture);
void RenderScreen(Shader shader, unsigned int VAO, unsigned int texture);
void RenderGround(Shader shader, unsigned int VAO, unsigned int depthMap);
void RenderBoxes(Shader shader, unsigned int VAO, unsigned int depthMap);
void RenderReflectiveBox(Shader shader, unsigned int VAO, unsigned int texture);
void RenderTextureBox(Shader shader, unsigned int VAO, unsigned int texture, unsigned int depthMap);
void RenderBrickWall(Shader shader, unsigned int VAO, unsigned int diffTexture, unsigned int normTextrue);
void RenderBrickWall2(Shader shader, unsigned int VAO, unsigned int diffTexture, unsigned int normTexture, unsigned int dispTexure);

int main() {

	//glfw's initialization and configuration:
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window:
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//glad initialisation
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//compile shaders:
	Shader shadowPhong("shaders/Shadow.vs", "shaders/ShadowPCF.fs");
	Shader shadowTexturePhong("shaders/ShadowTex.vs", "shaders/ShadowTexPCF.fs");
	Shader depthMapShader("shaders/depthShader.vs", "shaders/depthShader.fs", "shaders/depthShader.gs");
	Shader defaultSrceen("shaders/screen.vs", "shaders/screen.fs");
	Shader bnwSrceen("shaders/screen.vs", "shaders/screenBandW.fs");
	Shader edgeScreen("shaders/screen.vs", "shaders/screenKernel.fs");
	Shader lampShader("shaders/simple.vs", "shaders/lamp.fs");
	Shader reflectCM("shaders/reflect.vs", "shaders/reflect.fs");
	Shader cubeShader("shaders/cube.vs", "shaders/cube.fs");
	Shader Phong("shaders/Phong.vs", "shaders/Phong.fs");
	Shader Debug("shaders/cube.vs", "shaders/debug.fs");
	Shader NormalMaping("shaders/Normal.vs", "shaders/Normal.fs");
	Shader debug3("shaders/simple.vs", "shaders/simple.fs");
	Shader paralaxMap("shaders/paralaxMap.vs", "shaders/paralaxMap.fs");

	//VAO:
	unsigned int VBO[5], VAO[5];//1-cubeCF; 2-cubeTexCF; 3-quadTex; 4-skybox; 5 - normal map
	glGenVertexArrays(5, VAO);
	glGenBuffers(5, VBO);

	//cubeCF:
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCF), cubeCF, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//cubeTexCF:
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCF), cubeTexCF, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//quad:
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//skybox:
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//normal map:
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadNormVertices), &(quadNormVertices), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	//textures:
	unsigned int cubemapTexture = LoadCubemap(faces);
	unsigned int marbleTexture = loadTexture("textures/marble.jpg");
	unsigned int diffuseBrick = loadTexture("textures/brickwall.jpg");
	unsigned int normalBrick = loadTexture("textures/brickwall_normal.jpg");
	unsigned int diffuseBrick2 = loadTexture("textures/bricks2.jpg");
	unsigned int normalBrick2 = loadTexture("textures/bricks2_normal.jpg");
	unsigned int dispBrick2 = loadTexture("textures/bricks2_disp.jpg");

	//framebuffer:
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	//renderbuffer object
	unsigned rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//for shadows(depth fbo):
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while (!glfwWindowShouldClose(window)) {

		//time:
		float currentFrame = glfwGetTime();
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;
		state.MoveLamp();

		//input:
		processInput(window);

		//1st pass:
		renderCubemap(depthMapShader, VAO[0], depthMapFBO);
		//2nd pass:
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.0f, 0.15f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		RenderBrickWall(NormalMaping, VAO[4], diffuseBrick, normalBrick);
		RenderBrickWall2(paralaxMap, VAO[4], diffuseBrick2, normalBrick2, dispBrick2);
		RenderLamp(lampShader, VAO[0]);
		RenderGround(shadowPhong, VAO[0], depthCubemap);
		RenderBoxes(shadowPhong, VAO[0], depthCubemap);
		RenderTextureBox(shadowTexturePhong, VAO[1], marbleTexture, depthCubemap);
		RenderReflectiveBox(reflectCM, VAO[0], cubemapTexture);
		RenderSkyBox(cubeShader, VAO[3], cubemapTexture);

		//3rd pass:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		switch (state.filterType) {
		case 1:
			RenderScreen(defaultSrceen, VAO[2], colorBuffer);
			break;
		case 2:
			RenderScreen(bnwSrceen, VAO[2], colorBuffer);
			break;
		case 3:
			RenderScreen(edgeScreen, VAO[2], colorBuffer);
			break;
		}
	
		//finally:
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}








//render:
void RenderBrickWall2(Shader shader, unsigned int VAO, unsigned int diffTexture, unsigned int normTexture, unsigned int dispTexture) {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;


	shader.use();
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0, -2.9, 5.0));
	model = glm::scale(model, glm::vec3(2));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setFloat("heightScale", state.heightScale);
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dispTexture);
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("depthMap", 2);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void RenderBrickWall(Shader shader, unsigned int VAO, unsigned int diffTexture, unsigned int normTexture) {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;


	shader.use();
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0,-2.9,-5.0));
	model = glm::scale(model, glm::vec3(2));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normTexture);
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void RenderTextureBox(Shader shader, unsigned int VAO, unsigned int texture, unsigned int depthMap) {
	glUseProgram(shader.id);
	shader.setInt("shadows", state.shadows);
	shader.setFloat("far_plane", state.far_plane);
	shader.setInt("reverse_normals", 0);
	glUniform1i(glGetUniformLocation(shader.id, "marbleTexture"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);


	shader.setInt("diffuseTexture", 0);
	shader.setInt("depthMap", 1);
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	glBindVertexArray(VAO);
	float angle = 20.0f;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	model = glm::translate(model, state.textureBoxPos);


	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4();
	model = glm::translate(model, state.textureBox2Pos);
	model = glm::scale(model, glm::vec3(3.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void RenderReflectiveBox(Shader shader, unsigned int VAO, unsigned int texture) {
	float angle = 20.0f;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	shader.use();
	shader.setInt("shadows", state.shadows);
	shader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("cameraPos", camera.Position);

	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	model = glm::mat4();
	model = glm::translate(model, state.reflectiveCubePos);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	shader.setInt("skybox", 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderBoxes(Shader shader, unsigned int VAO, unsigned int depthMap) {
	float angle = 20.0f;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
	shader.setInt("depthMap", 0);
	shader.setFloat("far_plane", state.far_plane);
	shader.setInt("shadows", state.shadows);
	shader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);
	shader.setInt("reverse_normals", 0);

	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	model = glm::mat4();
	model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, -0.3f, -0.5f));
	model = glm::translate(model, state.yellowBoxPos);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//2nd box:
	model = glm::mat4();
	model = glm::translate(model, state.yellowBox2Pos);
	shader.setMat4("model", model);
	shader.setVec3("objectColor", 0.1f, 0.9f, 0.9f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//3rd box:
	model = glm::mat4();
	model = glm::translate(model, state.yellowBox3Pos);
	shader.setMat4("model", model);
	shader.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderLamp(Shader shader, unsigned int VAO) {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	shader.use();
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	shader.use();
	model = glm::mat4();
	model = glm::translate(model, state.lampPos);
	model = glm::scale(model, glm::vec3(0.25f));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderGround(Shader shader, unsigned int VAO, unsigned int depthMap) {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
	shader.setInt("depthMap", 0);
	shader.setFloat("far_plane", state.far_plane);
	shader.setInt("shadows", state.shadows);
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	shader.setVec3("objectColor", 0.95f, 0.5f, 0.5f);
	shader.setVec3("lightPos", state.lampPos);
	shader.setVec3("viewPos", camera.Position);

	model = glm::mat4();
	model = glm::translate(model, state.groundPos);
	model = glm::scale(model, glm::vec3(state.groundScale));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderScreen(Shader shader, unsigned int VAO, unsigned int texture) {
	shader.use();
	glBindVertexArray(VAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInt("screenTexture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderSkyBox(Shader shader, unsigned int VAO, unsigned int texture) {
	glDepthFunc(GL_LEQUAL);
	shader.use();
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	shader.setInt("skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void renderCubemap(Shader shader, unsigned int VAO, unsigned int buffer) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, state.near_plane, state.far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(state.lampPos, state.lampPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	shader.use();
	for (unsigned int i = 0; i < 6; ++i)
		shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	shader.setFloat("far_plane", state.far_plane);
	shader.setVec3("lightPos", state.lampPos);

	float angle = 20.0f;
	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, state.groundPos);
	model = glm::scale(model, glm::vec3(40.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, -0.3f, -0.5f));
	model = glm::translate(model, state.yellowBoxPos);
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, state.yellowBox2Pos);
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, state.yellowBox3Pos);
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	model = glm::translate(model, state.textureBoxPos);
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, state.textureBox2Pos);
	model = glm::scale(model, glm::vec3(3.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}






//textures:
unsigned int LoadCubemap(char const ** address) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	int width, height, nrChannels;
	unsigned char* data = NULL;
	for (int i = 0; i < 6; i++) {
		data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	return texture;
}
unsigned int loadTexture(char const * path) {
	//load and create a texture
	unsigned int texture;

	glGenTextures(1, &texture);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}










//state utilities:
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (state.firstMouse)
	{
		state.lastX = xpos;
		state.lastY = ypos;
		state.firstMouse = false;
	}

	float xoffset = xpos - state.lastX;
	float yoffset = state.lastY - ypos; 

	state.lastX = xpos;
	state.lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	//exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//camera movements
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, state.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, state.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, state.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, state.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		state.filterType = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		state.filterType = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		state.filterType = 3;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !state.shadowsKeyPressed)
	{
		state.shadows = !state.shadows;
		state.shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		state.shadowsKeyPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !state.cursorEnabled)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		state.cursorEnabled = true;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		state.cursorEnabled = false;
	}

}