#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);

	meshCount = 12; //set cube count

	meshList = new MyMesh[meshCount]; //create data container

	for (int i = 0; i < meshCount; i++) { //loop and generate cubes
		MyMesh mesh;
		meshList[i] = mesh;
		meshList[i].GenerateCube(1.0f, C_BLACK);
	}

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	counter += moveSpeed; //increment counter, we can clamp this to prevent to give the primiatives bounds

	m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));

	for (int i = 0; i < meshCount; i++) { //loop and render each cube
		GLfloat x = 1.0f + (2.0f * cos((GLfloat)i * 2.0f * PI / meshCount));
		GLfloat y = 1.0f + (2.0f * sin((GLfloat)i * 2.0f * PI / meshCount));
		meshList[i].Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(x + counter , y, 0.0f))); //add the positions to the translation matrix
	}
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{

	if (meshList != nullptr) {
		delete[] meshList;
		meshList = nullptr;
	}
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}