/*
Debug Camera
(c) 2015
original authors: Benjamin Robbins
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Implements a basic controllable camera system handy for any 3-D game.

	Camera
	- This class maintains data related to drawing as well as input. Every update it queries the keyboard input and uses that
	data to effect a translation or rotation in the camera's view matrix. The view and projection matrices provided by the camera
	are applied to the model matrix before it is sent to the shader for the rndering pipeline.
*/
#include "Camera.h"

#include <GLFW\glfw3.h>

#define ROT_RATE 60.0f
#define PAN_RATE 1.0f
#define MAX_ELE 60.0f

Camera::Camera(GLFWwindow* window)
{
	_fov = 0.0f;
	_near = 0.0f;
	_far = 0.0f;
	_aspectRatio = 0.0f;

	_width = 0.0f;
	_height = 0.0f;

	_projMat = glm::mat4();
	_viewMat = glm::mat4();

	_position = glm::vec3();
	_elevation = 0.0f;
	_bearing = 0.0f;

	_viewNeedsUpdate = true;

	_wKey = false;
	_aKey = false;
	_sKey = false;
	_dKey = false;
	_lArrowKey = false;
	_rArrowKey = false;
	_shiftKey = false;
	_ctrlKey = false;
	_uArrowKey = false;
	_dArrowKey = false;

	_window = window;
}
Camera::~Camera()
{

}

void Camera::Update(float dt)
{
	// Update key states
	_wKey = glfwGetKey(_window, GLFW_KEY_W);
	_aKey = glfwGetKey(_window, GLFW_KEY_A);
	_sKey = glfwGetKey(_window, GLFW_KEY_S);
	_dKey = glfwGetKey(_window, GLFW_KEY_D);
	_lArrowKey = glfwGetKey(_window, GLFW_KEY_LEFT);
	_rArrowKey = glfwGetKey(_window, GLFW_KEY_RIGHT);
	_shiftKey = glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT);
	_ctrlKey = glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL);
	_uArrowKey = glfwGetKey(_window, GLFW_KEY_UP);
	_dArrowKey = glfwGetKey(_window, GLFW_KEY_DOWN);

	// Check whether any keys are actually pressed before proceeding
	if (_wKey || _aKey || _sKey || _dKey || _lArrowKey || _rArrowKey || _shiftKey || _ctrlKey || _uArrowKey || _dArrowKey)
	{
		// Update current movement
		glm::vec3 move = glm::vec3(_aKey * 1.0f + _dKey * -1.0f, _ctrlKey * -1.0f + _shiftKey * 1.0f, _sKey * -1.0f + _wKey * 1.0f);
		if (abs(move.x + move.y + move.z) >= 1.0f) glm::normalize(move);

		// Update rotation
		// Horizontal
		_bearing = _bearing + (_lArrowKey * 1.0f + _rArrowKey * -1.0f) * ROT_RATE * dt;
		// Vertical
		_elevation = _elevation + (_uArrowKey * -1.0f + _dArrowKey * 1.0f) * ROT_RATE * dt;

		// To modify our position based on which direction we are facing, we have to rotate the move vector with the bearing quaternion
		_position += glm::vec3(glm::mat4_cast(glm::angleAxis(_bearing, glm::vec3(0.0f, 1.0f, 0.0f))) * glm::vec4(move, 0.0f) * PAN_RATE * dt);

		_viewNeedsUpdate = true;
	}

	UpdateViewMatrix();
}

void Camera::SetProjMatrix(float fov, float near, float far, float aspectRatio)
{
	_fov = fov;
	_aspectRatio = aspectRatio;
	float top = tanf(_fov / 2.0f);
	_projMat = glm::perspectiveFov(_fov, _aspectRatio, 1.0f / _aspectRatio, near, far);
}

void Camera::SetProjMatrix(float width, float height, float near, float far, bool isOrthographic)
{
	_aspectRatio = width / height;
	_projMat = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, near, far);
}

glm::mat4 Camera::View_ProjMat()
{
	return _projMat * _viewMat;
}

void Camera::UpdateViewMatrix()
{
	if (_viewNeedsUpdate)
	{
		// Constrain elevation
		_elevation = _elevation < MAX_ELE ? _elevation : MAX_ELE;
		_elevation = _elevation > -MAX_ELE ? _elevation : -MAX_ELE;
		// Apply rotations
		glm::mat4 rotationMat = glm::mat4_cast(glm::angleAxis(_bearing, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(_elevation, glm::vec3(1.0f, 0.0f, 0.0f)));
		glm::vec3 forward = glm::vec3(rotationMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		_viewMat = glm::lookAt(_position, _position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	_viewNeedsUpdate = false;
}

void Camera::position(glm::vec3 newPosition)
{
	_position = newPosition;
	_viewNeedsUpdate = true;
}
const glm::vec3& Camera::position()
{
	return _position;
}
void Camera::bearing(float newBearing)
{
	_bearing = newBearing;
	_viewNeedsUpdate = true;
}
void Camera::elevation(float newElevation)
{
	_elevation = newElevation;
	_viewNeedsUpdate = true;
}
const float Camera::bearing()
{
	return _bearing;
}
const float Camera::elevation()
{
	return _elevation;
}
