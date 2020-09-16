#include <glm.hpp>


const unsigned int WINDOW_WIDTH = 960;
const unsigned int WINDOW_HEIGHT = 720;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

const char* faces[6] = {
	"cubemap/3/west.jpg",
	"cubemap/3/east.jpg",
	"cubemap/3/up.jpg",
	"cubemap/3/down.jpg",
	"cubemap/3/south.jpg",
	"cubemap/3/north.jpg"
};


class State {
public:
	//mouse:
	bool cursorEnabled;
	float lastX;
	float lastY;
	bool firstMouse;

	float groundScale;

	//lamp:
	float lampV;
	bool lampSwitch;
	glm::vec3 lampPos;
	glm::vec3 lampForward;


	//shadows:
	bool shadows;
	float near_plane;
	float far_plane;
	bool shadowsKeyPressed;

	//time:
	float deltaTime;
	float lastFrame;

	float* quadVertices2;

	float heightScale;

	//object positions:
	glm::vec3 groundPos;
	glm::vec3 reflectiveCubePos;
	glm::vec3 yellowBoxPos;
	glm::vec3 yellowBox2Pos;
	glm::vec3 yellowBox3Pos;
	glm::vec3 textureBoxPos;
    glm::vec3 textureBox2Pos;
	glm::vec3 brickWallPos;

	int filterType;


	State() {
		cursorEnabled = false;

		lastX = WINDOW_WIDTH / 2.0f;
		lastY = WINDOW_HEIGHT / 2.0f;
		firstMouse = true;

		shadowsKeyPressed = false;
		shadows = true;
		near_plane = 1.0f;
		far_plane = 35.0f;

		deltaTime = 0.0f;
		lastFrame = 0.0f;

		
		groundScale = 20.0f;
		groundPos = glm::vec3(0.0f, groundScale*(-1.0f)-3.0f, 0.0f);
		yellowBoxPos = glm::vec3(10.0f,4.7f, -10.0f);
		yellowBox2Pos = glm::vec3(5.0f, -1.0f, -5.0f);
		yellowBox3Pos = glm::vec3(5.0f, -1.0f, -10.0f);
		reflectiveCubePos = glm::vec3(-5.0f, -1.0f, -20.0f);
		textureBoxPos = glm::vec3(5.0f, -1.0f, 5.0f);
		textureBox2Pos = glm::vec3(10.0f, 0.0f, 10);
		brickWallPos = glm::vec3(0.0f, -1.0f, 0.0f);

		lampPos = glm::vec3(0.0f, -1.0f, 0.0f);
		lampForward = glm::vec3(0.0f, 0.0f, 1.0f);
		lampV = 4.5f;
		lampSwitch = true;

		heightScale = 0.1f;

		filterType = 1;
	}

	void MoveLamp() {
		lampPos += deltaTime * lampV * lampForward;
		if (((lampPos.z > 8.0f) && (lampSwitch)) || ((lampPos.z < -8.0f) && (!lampSwitch))) {
			lampForward *= (-1);
			lampSwitch = !lampSwitch;
		}
	}

};