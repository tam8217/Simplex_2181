#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Creating the center of the circle
	vector3 vec1 = vector3(0.0f, 0.0f, -a_fHeight);
	vector3 tip = vector3(0.0f, 0.0f, a_fHeight);

	//Calculating the degrees, then converting to radians, needed for each subdivision
	float degs = 360 / a_nSubdivisions;
	float rads = (degs * PI) / 180;

	//Creating the Cone
	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//Creating two points to form the triangle
		vector3 vec2 = vector3(a_fRadius * cos(rads *i), a_fRadius *sin(rads *i), -a_fHeight);
		vector3 vec3 = vector3(a_fRadius * cos(rads *(i + 1)), a_fRadius *sin(rads *(i + 1)), -a_fHeight); 


		//Creating the triangle, keeping one point at the origin
		AddTri(vec1, vec3, vec2);

		//Making another triangle which connects to the tip of the cone
		AddTri(vec2, vec3, tip);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Getting the center for both circles
	float midHeight = .5 * a_fHeight;
	vector3 base1 = vector3(0.0f, 0.0f, midHeight);
	vector3 base2 = vector3(0.0f, 0.0f,-midHeight);

	

	//Calculating the degrees, then converting to radians, needed for each subdivision
	float degs = 360 / a_nSubdivisions;
	float rads = (degs * PI) / 180;

	//Creating the triangles
	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//Creating two points to form the triangle
		vector3 vec2 = vector3(a_fRadius * cos(rads *i), a_fRadius *sin(rads *i), midHeight);
		vector3 vec3 = vector3(a_fRadius * cos(rads *(i + 1)), a_fRadius *sin(rads *(i + 1)), midHeight); 

		vector3 vec4 = vector3(a_fRadius * cos(rads *i), a_fRadius *sin(rads *i), -midHeight);
		vector3 vec5 = vector3(a_fRadius * cos(rads *(i + 1)), a_fRadius *sin(rads *(i + 1)), -midHeight); 


		//Creating the triangle for the top circle
		AddTri(base1, vec2, vec3);

		//Creating the triangle for the bottom circle
		AddTri(base2, vec5, vec4);


		//Drawing the sides of the cylinder, connecting the triangles/circles
		AddQuad(vec3, vec2, vec5, vec4);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Finding the height to have both bases at 
	float midHeight = .5 * a_fHeight;

	//Getting the center for both circles
	vector3 base1 = vector3(0.0f, 0.0f, midHeight);
	vector3 base2 = vector3(0.0f, 0.0f, -midHeight);

	//Calculating the degrees, then converting to radians, needed for each subdivision
	float degs = 360 / a_nSubdivisions;
	float rads = (degs * PI) / 180;

	//Creating the tube
	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//Calculating the inner points on the top and bottom
		vector3 topInner1 = vector3(a_fInnerRadius * cos(rads *i), a_fInnerRadius *sin(rads *i), midHeight);
		vector3 topInner2 = vector3(a_fInnerRadius * cos(rads *(i + 1)), a_fInnerRadius *sin(rads *(i + 1)), midHeight); 

		vector3 bottomInner1 = vector3(a_fInnerRadius * cos(rads *i), a_fInnerRadius *sin(rads *i), -midHeight);
		vector3 bottomInner2 = vector3(a_fInnerRadius * cos(rads *(i + 1)), a_fInnerRadius *sin(rads *(i + 1)), -midHeight); 

		//Calculating the outer points on the top and bottom
		vector3 topOuter1 = vector3(a_fOuterRadius * cos(rads *i), a_fOuterRadius *sin(rads *i), midHeight);
		vector3 topOuter2 = vector3(a_fOuterRadius * cos(rads *(i + 1)), a_fOuterRadius *sin(rads *(i + 1)), midHeight); 

		vector3 bottomOuter1 = vector3(a_fOuterRadius * cos(rads *i), a_fOuterRadius *sin(rads *i), -midHeight);
		vector3 bottomOuter2 = vector3(a_fOuterRadius * cos(rads *(i + 1)), a_fOuterRadius *sin(rads *(i + 1)), -midHeight); 

		//Drawing quads to create the rings on both sides
		AddQuad(topInner2, topInner1, topOuter2, topOuter1);
		AddQuad(bottomInner1, bottomInner2, bottomOuter1, bottomOuter2);

		//Drawing quads to connect the rings
		AddQuad(topInner1, topInner2, bottomInner1, bottomInner2);
		AddQuad(topOuter2, topOuter1, bottomOuter2, bottomOuter1);
	}
	
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	//Calculating the degrees, then converting to radians, needed for each subdivision
	//float degs = 360 / a_nSubdivisionsA;
	//float rads = (degs * PI) / 180;

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//Vectors to store the height and radius for each part of the sphere
	std::vector<float> heightsFirst = std::vector<float>();
	std::vector<float> radiiFirst = std::vector<float>();
	std::vector<float> heightsSecond = std::vector<float>();
	std::vector<float> radiiSecond = std::vector<float>();

	int totalCircPoints = 2 + (a_nSubdivisions * 2); //This accounts for the two poles, and the mirroring of the points in the circle
	
	//Calculating the divisions in the sphere based off the new number of points
	float degs = 360 / totalCircPoints;
	float rads = (degs * PI) / 180;	

	//Calculating a non-drawn circle to extrapolate height and radius values for the sphere
	for (size_t i = 0; i < totalCircPoints; i++)
	{
		//Circle drawing as done before
		vector3 vec2 = vector3(a_fRadius * cos(rads *i), a_fRadius *sin(rads *i), 0);
		vector3 vec3 = vector3(a_fRadius * cos(rads *(i + 1)), a_fRadius *sin(rads *(i + 1)), 0);

		//Saving values for use later
		radiiFirst.push_back(vec2.x);
		radiiSecond.push_back(vec3.x);

		heightsFirst.push_back(vec2.y);
		heightsSecond.push_back(vec3.y);
	}

	//Drawing the sphere
	for (size_t i = 0; i < totalCircPoints; i++)
	{
		//Drawing each layer individually
		for (size_t j = 0; j < totalCircPoints; j++)
		{
			//Calculating the bottom set of points in the layer
			vector3 bottom1 = vector3(radiiFirst[i] * cos(rads *j), radiiFirst[i] *sin(rads *j), heightsFirst[i]);
			vector3 bottom2 = vector3(radiiFirst[i] * cos(rads *(j + 1)), radiiFirst[i] * sin(rads *(j + 1)), heightsFirst[i]);
			
			//Calculating the top set of points in the layer, becomes the bottom set in the next iteration
			vector3 top1 = vector3(radiiSecond[i] * cos(rads *j), radiiSecond[i] * sin(rads *j), heightsSecond[i]);
			vector3 top2 = vector3(radiiSecond[i] * cos(rads *(j + 1)), radiiSecond[i] *sin(rads *(j + 1)), heightsSecond[i]);

			//Drawing the points
			AddQuad(bottom1, bottom2, top1, top2);
		}
	}
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}