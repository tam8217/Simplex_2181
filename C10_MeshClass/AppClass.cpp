#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);
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

	/*
	static float fDisplacement = 0.0f;
	fDisplacement += .1f;
	matrix4 m4Space = glm::translate(vector3(fDisplacement, 1.0f, 0.0f));
	matrix4 m4Translation1 = m4Space * glm::translate(vector3(1.0f, 1.0f, 1.0f));
	//matrix4 m4Scale = glm::scale(vector3(3.0f, 3.0f, 3.0f));
	//matrix4 m4Compound = m4Translation1 * ToMatrix4(m_qArcBall);
	*/
	static DWORD startTime = GetTickCount();

	DWORD curTime = GetTickCount();

	DWORD deltTime = curTime - startTime;

	float timeSeconds = static_cast<float>(deltTime / 1000.0f);
	float totalTime = 5.0f;
	float fPercent = MapValue(timeSeconds, 0.0f, totalTime, 0.0f, 1.0f);
	static vector3 v3InitialPoint(0, 0, 0);
	static vector3 v3EndPoint(5, 0, 0);

	
	vector3 v3Position = glm::lerp(v3InitialPoint, v3EndPoint, fPercent);
	matrix4 m4Rotation = glm::rotate(IDENTITY_M4, glm::radians(90.0f), AXIS_Z);
	//matrix4 m4Position = glm::translate(identtyM4, v3Position);
	
	//std::cout << startTime << " " << curTime  << " " << deltTime << " " << timeSeconds <<std::endl;
	//matrix4 m4Translation2 = m4Space * glm::translate(vector3(3.0f, 3.0f, 3.0f));

	m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Rotation);

	


	
	if (fPercent >= 1.0f)
	{
		//fPercent = 0.0f;
		std::swap(v3InitialPoint, v3EndPoint);
		startTime = GetTickCount();
	}
	else
	{
		//fPercent += .01f;
		//fPercent += timeSeconds / totalTime;
	}
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Translation2);
		
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}