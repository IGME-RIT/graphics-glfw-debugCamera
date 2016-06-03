#pragma once
#include <GLFW\glfw3.h>
#include <GLM\gtc\type_ptr.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\quaternion.hpp>

class Camera
{
public:
	Camera(GLFWwindow* window = (GLFWwindow*)nullptr);
	~Camera();

	// Checks all of the current keyboard input and adjusts the view matrix accordingly
	void Update(float dt);
	// Perspective view
	void SetProjMatrix(float fov, float near, float far, float aspectRatio);
	// Orthographic view
	void SetProjMatrix(float width, float height, float near, float far, bool isOrthographic);

	glm::mat4 View_ProjMat();

	void position(glm::vec3 newPosition);
	const glm::vec3& position();
	void bearing(float newBearing);
	void elevation(float newElevation);
	const float bearing();
	const float elevation();

private:

	void UpdateViewMatrix();

private:
	// viewing data
	float _fov;
	float _near;
	float _far;
	float _aspectRatio;

	float _width;
	float _height;

	glm::mat4 _projMat;
	glm::mat4 _viewMat;
	bool _viewNeedsUpdate;

	glm::vec3 _position;
	float _elevation;
	float _bearing;

	// Input data
	bool _wKey;
	bool _aKey;
	bool _sKey;
	bool _dKey;
	bool _lArrowKey;
	bool _rArrowKey;
	bool _shiftKey;
	bool _ctrlKey;
	bool _uArrowKey;
	bool _dArrowKey;

	GLFWwindow* _window;
};
