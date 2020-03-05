#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 Simplex::MyCamera::GetPosition(void) { return m_v3Position; }
void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }
vector3 Simplex::MyCamera::GetTarget(void) { return m_v3Target; }
void Simplex::MyCamera::SetAbove(vector3 a_v3Above) { m_v3Above = a_v3Above; }
vector3 Simplex::MyCamera::GetAbove(void) { return m_v3Above; }
void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }
void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }
void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }
void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }
void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }
void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }
matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }
matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUpward(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and upward
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	m_v3Above = other.m_v3Above;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUpward(other.m_v3Position, other.m_v3Target, other.m_v3Above);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Above, other.m_v3Above);
	
	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Above = vector3(0.0f, 1.0f, 0.0f); //What is above the camera

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUpward(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;

	m_v3Above = a_v3Position + glm::normalize(a_v3Upward);
	
	//Calculate the Matrix
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	m_v3Target = m_v3Position + m_v3Forward; //calculate target vector
	m_m4View = glm::lookAt(m_v3Position, m_v3Target, glm::normalize(m_v3Above - m_v3Position)); //position, target, upward
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective) //perspective
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else //Orthographic
	{
		m_m4Projection = glm::ortho(m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
			m_v2Vertical.x, m_v2Vertical.y, //vertical
			m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}
//Moves the camera forwards or backwards
void MyCamera::MoveForward(float a_fDistance)
{
	//uses the forward vector to move FB
	m_v3Position += m_v3Forward * a_fDistance;
	m_v3Target += m_v3Forward * a_fDistance;
	m_v3Above += m_v3Forward * a_fDistance;
}
//Moves the camera up or down
void MyCamera::MoveVertical(float a_fDistance){

	//uses the up vector to move UD
	m_v3Position += m_v3Up * a_fDistance;
	m_v3Target += m_v3Up * a_fDistance;
	m_v3Above += m_v3Up * a_fDistance;

}
//Moves the camera left or right
void MyCamera::MoveSideways(float a_fDistance){

	//uses the right vector to move LR
	m_v3Position += m_v3Right * a_fDistance;
	m_v3Target += m_v3Right * a_fDistance;
	m_v3Above += m_v3Right * a_fDistance;

}
//Calulates y rotations
void MyCamera::Yaw(float a_fAngle, float a_fDamper) {

	//clamp the yaw rotation between -90 <= theta <= 90
	/*if (a_fAngle > 90) {
		a_fAngle = 90;
	}	
	else if (a_fAngle < -90) {
		a_fAngle = -90;
	}*/
	m_v3Rotations.y += (a_fAngle * a_fDamper); //apply damper and add to rotation vector
}
//Calculates x rotations
void MyCamera::Pitch(float a_fAngle, float a_fDamper) {

	//clamp the pitch rotation between -90 <= theta <= 90
	/*if (a_fAngle > 90) {
		a_fAngle = 90;
	}
	else if (a_fAngle < -90) {
		a_fAngle = -90;
	}*/
	m_v3Rotations.x += (a_fAngle * a_fDamper); //apply damper and add to rotation vector
}
//Calculates the camera's foward vector and initializes the right & up vectors aftwerwards
vector3 MyCamera::SetForwardVector(void) {

	vector3 start(0.0f, 0.0f, 1.0f); //normalized start vector (since facing into the screen at start)
	
	m_v3Forward = start;

	m_qOrientation = quaternion(m_v3Rotations);
	m_v3Forward = m_qOrientation * m_v3Forward; //set any rotation values

	SetRightVector(); //calculate right vector
	SetUpVector(); //calculate up vector
	return m_v3Forward;
}
//Calculate the right vector
vector3 MyCamera::SetRightVector(void) {

	vector3 axis(0.0f, 1.0f, 0.0f); //default up axis
	vector3 right = glm::cross(axis, m_v3Forward); //cross the axis with the forward directional vector to get the right vector (ie. vec3(0,1,0) X vec3(0,0,1) == vec3(1,0,0))
	m_v3Right = glm::normalize(right); //normalize since we only want the direction 
	return m_v3Right;
}
//calculate the up vector
vector3 MyCamera::SetUpVector(void) {

	vector3 axis(0.0f, -1.0f, 0.0f);
	vector3 up = axis;
	//vector3 up = glm::cross(m_v3Right, m_v3Forward); //cross the right vector with the forward directional vector to get the up vector (ie. vec3(0,0,1) X vec3(1,0,0) == vec3(0,1,0))
	m_v3Up = glm::normalize(up); //normalize since we only want the direction 
	return m_v3Up;
}


