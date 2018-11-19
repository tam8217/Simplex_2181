#include "MyOctant.h"
using namespace Simplex;
uint MyOctant::m_nCount = 0;
//  MyOctant
void MyOctant::Init(void)
{
	m_nData = 0;

	//Getting references to the mesh and entiti managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	//Initializing all the children
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	//If there are children, go in and add all the chilcren
	if (m_uChildren != 0)
	{
		uint num = 0;
		//Adding through each child
		for (size_t i = 0; i < 8; i++)
		{
			num += m_pChild[i]->GetOctantCount();
		}

		//Returning the number 
		return num;
	}
	
	return 1;
}

MyOctant::MyOctant(int levels, int maxEnt)
{
	//Initialize the Octree 
	Init();

	//Setting the maximum levels deep to do
	m_nMaxLevel = levels;
	//Setting a maximum number of entities
	maxEntites = maxEnt;

	//Setting the root to be the initial octant
	m_pRoot = this;
	
	//Getting all the entites in the scene
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();

	//m_EntityList = indList;
	//Getting a count of the entites
	uint iEntityCount = l_Entity_List.size();

	//Creating a vector to be used to store the maxes and mins of every ribid body
	std::vector<vector3> v3MaxMin_list;

	//Storing the min and max of every rigidbody to be used to create the octree
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = l_Entity_List[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
	}

	//Creating the big rigidbody 
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);

	//Making a cube with the provided mins and maxes
	m_pRigidBody->MakeCubic();

	//Getting the center and storing it
	octantCenter = m_pRigidBody->GetCenterLocal();

	//Getting the size of this box
	vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();

	//Getting the min and max of the octant
	octantMax = m_pRigidBody->GetMaxLocal();
	octantMin = m_pRigidBody->GetMinLocal();

	//Making the ID of this octant equal to the count of octants
	m_iID = m_nCount;

	//Actually building the tree with the data
	ConstructTree(m_nMaxLevel);
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	//Initializing the variables for this leaf
	Init();

	//Creating a vector to hold the min and max of the leaf
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);


	//Getting the center and storing it
	octantCenter = a_v3Center;

	//Getting the min and max of the octant
	octantMax = octantCenter + (vector3(a_fSize));
	octantMin = octantCenter - (vector3(a_fSize));

	//Increasing the count of octants that have been made
	m_nCount++;
	m_iID = m_nCount;
}

bool Simplex::MyOctant::OctantContainsCheck(int numEntites)
{
	//Keeping track of how many objects complete the check for being within the octant
	int tempCounter = 0;

	//Getting the number of entities in the list
	uint iEntityCount = m_pEntityMngr->GetEntityList().size();

	//Looping through each entity, checking if it's within an octant
	for (size_t i = 0; i < iEntityCount; i++)
	{
		//If it's colliding, increment the counter
		if (IsColliding(i))
		{
			tempCounter++;
		}

		//return true from the loop if there's enough netities 
		if (tempCounter > numEntites)
		{
			return true;
		}
	}
	return false;
}


void MyOctant::Subdivide()
{
	//If deep enough into the tre, stop dividing
	if (m_nLevel >= m_nMaxLevel)
	{
		return;
	}

	//If at a leaf, don't try and subdivide
	if (m_uChildren == 8)
	{
		return;
	}

	//Getting the center of this box
	vector3 v3Center = m_pRigidBody->GetCenterLocal();

	//Getting the size of this box
	vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();

	//Getting the center of the child
	float fSize = (v3HalfWidth.x) / 2.0f;
	float fCenters = fSize;

	//Creating 8 boxes all around the center
	m_pChild[0] = new MyOctant(v3Center + vector3( fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new MyOctant(v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new MyOctant(v3Center + vector3(-fCenters,-fCenters, fCenters), fSize);
	m_pChild[3] = new MyOctant(v3Center + vector3( fCenters,-fCenters, fCenters), fSize);
	
	m_pChild[4] = new MyOctant(v3Center + vector3( fCenters, fCenters,-fCenters), fSize);
	m_pChild[5] = new MyOctant(v3Center + vector3(-fCenters, fCenters,-fCenters), fSize);
	m_pChild[6] = new MyOctant(v3Center + vector3(-fCenters,-fCenters,-fCenters), fSize);
	m_pChild[7] = new MyOctant(v3Center + vector3( fCenters,-fCenters,-fCenters), fSize);
	
	//Setting the number of children in this octant
	m_uChildren = 8;

	//Subdividing further
	for (uint i = 0; i < 8; i++)
	{
		//Incrementing the level
		m_pChild[i]->m_nLevel = m_nLevel + 1;

		//Keeping track of the level
		m_pChild[i]->m_pParent = this;

		//Sending the max level down
		m_pChild[i]->m_nMaxLevel = m_nMaxLevel;
		//Keepinig track of the root node throughout
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pRoot->m_nCount++;
		
		//If there are enough entites in the child, increase the octant
		if (m_pChild[i]->OctantContainsCheck(maxEntites))
		{
			m_pChild[i]->Subdivide();
		}

	}
}

void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//Sending the method to all the children
	for (size_t i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}

	//If at a leaf, has no children
	if (m_uChildren == 0)
	{
		//Looping through each entity to check where it collides
		for (size_t i = 0; i < m_pEntityMngr->GetEntityList().size(); i++)
		{
			//If the entity is inside of the box, add it to the entity list, and add the dimension
			if (IsColliding(i))
			{
				m_pEntityMngr->AddDimension(i, m_iID);
			}
		}
	}
	
}
void Simplex::MyOctant::KillBranches(void)
{
	//Looping through each child and deleting its branches
	//Only deleting if there are children present
	if (m_uChildren != 0)
	{
		for (size_t i = 0; i < 8; i++)
		{
			//Sending the method into the children
			m_pChild[i]->KillBranches();

			//Deleting the pointer at the child
			SafeDelete(m_pChild[i]);
		}

		//Resetting number of children
		m_uChildren = 0;
	}
	
}
void MyOctant::Release(void)
{
	//Deleting all the branches from the root
	if (m_nLevel == 0)
	{
		KillBranches();
	}
	
	//Cleaning up data
	m_lData.clear();
	m_uChildren = 0;
	m_lChild.clear();
}
void Simplex::MyOctant::Display(int levelToDisplay)
{
	//Displaying all parts of the octant
	if (levelToDisplay == -1)
	{
		//Making the cubes display
		m_pRigidBody->AddToRenderList();

		//Displpaying children
		for (uint i = 0; i < 8; i++)
		{
			//Displaying all the children
			if (m_pChild[i])
				m_pChild[i]->Display(levelToDisplay);
		}
	}
	//Just displaying the outside box
	else if (levelToDisplay == 0)
	{
		m_pRigidBody->AddToRenderList();
	}

	//Searching for a specific Level to display
	else
	{
		//If at the correct level, display it
		if (m_iID == levelToDisplay)
		{
			m_pRigidBody->AddToRenderList();
			return;
		}

		//If not, go deeper
		else
		{
			for (uint i = 0; i < 8; i++)
			{
				//Displaying all the children
				if (m_pChild[i])
					m_pChild[i]->Display(levelToDisplay);
			}
		}
	}
}
bool Simplex::MyOctant::IsColliding(uint index)
{
	//Getting the rigidbody of the index 
	MyRigidBody* tempRigidBody = m_pEntityMngr->GetEntityList()[index]->GetRigidBody();

	//Doing AABB colision with individual boxes and the octant square
	if (octantMax.x < tempRigidBody->GetMinGlobal().x) 
		return false;
	if (octantMin.x > tempRigidBody->GetMaxGlobal().x)
		return false;

	if (octantMax.y < tempRigidBody->GetMinGlobal().y) 
		return false;
	if (octantMin.y > tempRigidBody->GetMaxGlobal().y)
		return false;

	if (octantMax.z < tempRigidBody->GetMinGlobal().z) 
		return false;
	if (octantMin.z > tempRigidBody->GetMaxGlobal().z) 
		return false;

	return true;
	
}
MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

MyOctant::~MyOctant(){Release();};
//Accessors
void MyOctant::SetData(int a_nData){ m_nData = a_nData; }
void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Setting the max level to what's passed in
	m_nMaxLevel = a_nMaxLevel;

	//Setting the count to now be 1 becuase there's an octant present
	m_nCount = 1;

	//Subdividing from the base tree
	Subdivide();

	//Assigning IDs to all the entities to help with collision
	AssignIDtoEntity();
}
int MyOctant::GetData(void){ return m_nData; }
void MyOctant::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

