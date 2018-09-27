#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);

	//Creating the cubes
	for (size_t i = 0; i < 11; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{
			//Checking if there should be a hole 
			//Far sides
			if (i == 0 || i == 10)
			{
				if (j == 1 || j == 2 || j == 3)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();

					//Generating the cube
					tempMesh->GenerateCube(1.0f, C_BLACK);

					//Generating the position
					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					//Rendering it initially
					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					//Storing the values
					spaceCubes.push_back(tempMesh);
					positions.push_back(position);
				}
			}
			//Next column in
			if (i == 1 || i == 9)
			{
				if (j == 3 || j == 4)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();
					tempMesh->GenerateCube(1.0f, C_BLACK);

					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					spaceCubes.push_back(tempMesh);
					positions.push_back(position);
				}
			}
			//3rd column
			if (i == 2 || i == 8)
			{
				if(j == 1 || j == 2 || j == 3 || j == 4 || j == 5 || j == 7)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();
					tempMesh->GenerateCube(1.0f, C_BLACK);

					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					spaceCubes.push_back(tempMesh);
					positions.push_back(position);

				}
			}
			//4th column
			if (i == 3 || i == 7)
			{
				if(j == 0 || j == 2 || j == 3 || j == 5 || j == 6)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();
					tempMesh->GenerateCube(1.0f, C_BLACK);

					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					spaceCubes.push_back(tempMesh);
					positions.push_back(position);
				}
			}
			//5th column
			if (i == 4 || i == 6)
			{
				
				if(j == 0 || j == 2 || j == 3 || j == 4 || j == 5)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();
					tempMesh->GenerateCube(1.0f, C_BLACK);

					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					spaceCubes.push_back(tempMesh);
					positions.push_back(position);
				}
			}
			//Middle
			if (i == 5)
			{
				if (j == 2 || j == 3 || j == 4 || j == 5)
				{
					//Creating a new mesh
					MyMesh* tempMesh = new MyMesh();
					tempMesh->GenerateCube(1.0f, C_BLACK);

					matrix4 position = glm::translate(vector3(-5.0f + i, -4.0f + j, 0.0f));

					tempMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), position);

					spaceCubes.push_back(tempMesh);
					positions.push_back(position);
				}
			}
		}
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

	//Creating a static variable for use in this 
	static float fDisplacement = 0.0f;
	
	//Incrementing the variable for movement
	fDisplacement += .1f;

	//Creating a matrix which will provide movement
	matrix4 m4Space = glm::translate(vector3(fDisplacement, 1.0f, 0.0f));

	//Looping through every part of the invader
	for (size_t i = 0; i < spaceCubes.size(); i++)
	{
		//Rendering the cube, and moving it
		spaceCubes[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Space * positions[i]);
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);

	//Deleting every cube
	for (size_t i = 0; i < spaceCubes.size(); i++)
	{
		SafeDelete(spaceCubes[i]);
	}
	//release GUI
	ShutdownGUI();
}