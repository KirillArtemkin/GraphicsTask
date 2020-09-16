#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>

//basic camera movements
enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

//deafault camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 UniversalUp;
	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;

	//constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	//constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	//view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	//keyboard
	void ProcessKeyboard(CameraMovement direction, float deltaTime);

	//mouse
	void ProcessMouseMovement(float x, float y, GLboolean constrainPitch = true);

private:

	void updateCameraVectors();


};