#include "ParticleSystem.hpp"

class Camera
{
public:
	static Camera *getInstance();
	static void kill();

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 target;
	glm::vec3 dir;
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 view;
	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float fov;
	bool firstMouse;
private:
	Camera();
	~Camera();
	static Camera *_camelton;
};
