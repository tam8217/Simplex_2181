#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This part initialize the camera position so I can see the scene better; 
		the guide cube and the tip of the pencil so all solutions on the exam 
		look the same to the grader
	*/
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 35.0f, ZERO_V3, AXIS_Y);
		
	m_pGuideCube = new MyMesh();
	m_pGuideCube->GenerateCube(10.0f, C_WHITE);

	m_pMesh = new MyMesh();
	m_pMesh->GenerateCone(1.0f, 2.0f, 8, vector3(0.0f, 0.0f, 0.0f));
#pragma endregion

	//Creating the body and eraser of the pencil
	bodyCyl = new MyMesh();
	bodyCyl->GenerateCylinder(1.0f, 4.0f, 8, vector3(1.0f, 1.0f, 0.0f));
	eraseCyl = new MyMesh();
	eraseCyl->GenerateCylinder(1.0f, 2.0f, 8, vector3(1.0f,.75f, .8f));

	//Please change to your name and email
	m_sProgramer = "Tristan Marshall - tam8217@g.rit.edu";

	//Getting the verticies from the generated cube
	allStopList = m_pGuideCube->GetVertexList();

	//Loading in the sound from the file
	if (!m_soundBuffer.loadFromFile("..//Pencil.wav"))
		std::cout << "Not loaded" << std::endl;
	else
	{
		//Setting the sound buffer
		m_sound.setBuffer(m_soundBuffer);

		//Playing the sound 
		m_sound.play();

		//Looping the sound
		m_sound.setLoop(true);
	}
}
void Application::Update(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This updates the internal clock of the Simplex library and process the Arcball
	*/
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();
#pragma endregion
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//Initializing static variables
	static int curPos = 0; //Tracking the current stop
	static int sideCount = 0; //Tracking which side of the cube is currently being "drawn" on
	static int numStops = 0; //Tracking the number of stops per side, used to normalize motion 
	static int stopCount = 0; //Tracking the number of times stops have been hit on a specific side
	static float fTimer = 0; //Tracking the current time for lerping

	//If on the first side, reduce the number of sides
	if (sideCount == 0)
		numStops = 5;
	else
		numStops = 6;
	
	//Get a timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock
	

	//Adding to the timer each frame
	fTimer += m_pSystem->GetDeltaTime(uClock);

	//Mapping the time to be a percentage for lerp
	float percentage = MapValue(fTimer, 0.0f, m_fTimeBetweenStops, 0.0f, 1.0f);

	
	//calculate the current position
	vector3 v3CurrentPos = glm::lerp(allStopList[curPos % allStopList.size()], allStopList[(curPos + 1) % allStopList.size()], percentage);

	//Creating variables used in calculating the orientation of the models
	vector3 bodyOffset;
	vector3 eraseOffset;
	matrix4 rotation;
	matrix4 tipModel;
	matrix4 bodyModel;
	matrix4 eraseModel;
	
	//Setting the offset and rotation of the pencil based on which side of the cube it's on
	switch (sideCount)
	{
	case 0:
		//Placing the body of the pencil behind the lead
		bodyOffset = vector3(0, 0, 4.0f);

		//Placing the eraser on top of the body
		eraseOffset = vector3(0, 0, 7.0f);
		
		//No rotation on this side
		rotation = IDENTITY_M4;

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	case 1:
		//Placing the body of the pencil behind the lead
		bodyOffset = vector3(4.0f, 0, 0);

		//Placing the eraser on top of the body
		eraseOffset = vector3(7.0f, 0, 0);

		//Setting the rotation to the correct side
		rotation = glm::rotate(IDENTITY_M4, glm::radians(90.0f), vector3(0.0f, 1.0f, 0.0f));

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	case 2:
		//Placing the body of the pencil behind the lead
		bodyOffset = vector3(0, 4.0f, 0);

		//Placing the eraser on top of the body
		eraseOffset = vector3(0, 7.0f, 0);

		//Setting the rotation to the correct side
		rotation = glm::rotate(IDENTITY_M4, glm::radians(-90.0f), vector3(1.0f, 0.0f, 0.0f));

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	case 3:
		bodyOffset = vector3(0, 0, -4.0f);

		//Placing the eraser on top of the body
		eraseOffset = vector3(0, 0, -7.0f);

		rotation = glm::rotate(IDENTITY_M4, glm::radians(180.0f), vector3(0, 1.0f, 0.0f));

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	case 4:
		bodyOffset = vector3(-4.0f,0,0);

		//Placing the eraser on top of the body
		eraseOffset = vector3(-7.0f,0,0);

		rotation = glm::rotate(IDENTITY_M4, glm::radians(-90.0f), vector3(0, 1.0f, 0.0f));

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	case 5:
		bodyOffset = vector3(0, -4.0f, 0);
		//Placing the eraser on top of the body
		eraseOffset = vector3(0, -7.0f, 0);

		rotation = glm::rotate(IDENTITY_M4, glm::radians(90.0f), vector3(1.0f, 0.0f, 0.0f));

		//Calculating the position of the models
		tipModel = glm::translate(IDENTITY_M4, v3CurrentPos)* rotation;
		bodyModel = glm::translate(IDENTITY_M4, v3CurrentPos + bodyOffset) * rotation;
		eraseModel = glm::translate(IDENTITY_M4, v3CurrentPos + eraseOffset) * rotation;
		break;
	default:
		break;
	}

	//Resetting the timer if the percentage goes too high
	if (percentage >= 1.0f)
	{

		//Resetting timer
		fTimer = m_pSystem->GetDeltaTime(uClock);
		//Moving to the next set of stops
		curPos++;
		//Adding to the stops for this side
		stopCount++;

		//If this cycle is done, move to the next side
		if ((stopCount % numStops) == 0)
		{
			//Move to next side
			sideCount++;

			//Looping back to the first side if necessary
			sideCount %= 6;

			//Resetting the stops
			stopCount = 0;
		}
	}

	//Drawinig the shapes
	//Modifying the view by arcball keeps it following the rotation with middle mouse
	bodyCyl->Render(m4Projection, m4View * ToMatrix4(m_qArcBall), bodyModel);
	m_pMesh->Render(m4Projection, m4View * ToMatrix4(m_qArcBall), tipModel);
	eraseCyl->Render(m4Projection, m4View * ToMatrix4(m_qArcBall), eraseModel);
	

#pragma region DOES NOT NEED CHANGES
	/*
		This part does not need any changes at all, it is just for rendering the guide cube, the 
		skybox as a frame of reference and the gui display
	*/
	//Render the guide cube
	m_pGuideCube->Render(m4Projection, m4View, ToMatrix4(m_qArcBall)); 

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
#pragma endregion
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	//I already deallocated the memory for the meshes if you allocate new memory please
	//deallocate it before ending the program
	SafeDelete(m_pMesh);
	SafeDelete(m_pGuideCube);
	SafeDelete(bodyCyl);
	SafeDelete(eraseCyl);
}