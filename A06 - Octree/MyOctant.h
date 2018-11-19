/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __MYOCTANT_H_
#define __MYOCTANT_H_

#include "Simplex\Simplex.h"
#include "MyEntityManager.h"

namespace Simplex
{

//System Class
class MyOctant
{
	int m_nData = 0; //Number of elements in the list of elements
	std::vector<int> m_lData; //list of elements
	MeshManager* m_pMeshMngr = nullptr; //Pointer to Mesh manager
	MyEntityManager* m_pEntityMngr = nullptr;
	MyRigidBody* m_pRigidBody = nullptr;
	uint m_iID = 0;
	static uint m_nCount;
	MyOctant* m_pParent = nullptr;
	MyOctant* m_pChild[8];
	uint m_nLevel = 0;
	uint m_nMaxLevel;
	bool hasChildren = false;
	uint maxEntites = 5;

	MyOctant* m_pRoot = nullptr;//Root octant


	uint m_uChildren = 0;// Number of children on the octant (either 0 or 8)
	//std::vector<uint> m_EntityList; //List of Entities under this octant (Index in Entity Manager)


	vector3 octantMax = vector3(0.0f);
	vector3 octantMin = vector3(0.0f);
	vector3 octantCenter = vector3(0.0f);
	std::vector<MyOctant*> m_lChild; //list of nodes that contain objects (this will be applied to root only)

public:
	void Display(int);
	bool IsColliding(uint);
	void Subdivide(void);
	MyOctant(vector3 a_v3Center, float a_fSize);
	bool OctantContainsCheck(int);

	/*
	USAGE: Gets the total number of octants in the world
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	uint GetOctantCount(void);
	/*
	Usage: Constructor
	Arguments: ---
	Output: class object instance
	*/
	MyOctant(int levels, int maxEnt);
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	MyOctant(MyOctant const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	MyOctant& operator=(MyOctant const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~MyOctant(void);

	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	Output: ---
	*/
	void Swap(MyOctant& other);
	
	/*
	USAGE: Traverse the tree up to the leafs and sets the objects in them to the index
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void AssignIDtoEntity(void);

	/*
	USAGE: Deletes all children and the children of their children (almost sounds apocalyptic)
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void KillBranches(void);

	/*
	USAGE: Creates a tree using subdivisions, the max number of objects and levels
	ARGUMENTS:
	- uint a_nMaxLevel = 3 -> Sets the maximum level of the tree while constructing it
	OUTPUT: ---
	*/
	void ConstructTree(uint a_nMaxLevel = 3);

	/*
	Usage: Gets data member
	Arguments: ---
	Output: data
	*/
	int GetData(void);
	/*
	Usage: Sets data member
	Arguments: int a_nData = 1 -> data to set
	Output: ---
	*/
	void SetData(int a_nData = 1);
	/* Property */
	//__declspec(property(get = GetData, put = SetData)) int Data;

	/*
	Usage: adds data on vector
	Arguments: int a_nData -> data to add
	Output: ---
	*/
	void SetDataOnVector(int a_nData);
	/*
	Usage: gets data on vector at the specified entry
	Arguments: int a_nIndex -> entry index 
	Output: entry's content
	*/
	int& GetDataOnVector(int a_nIndex);


private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);

	
};//class

} //namespace Simplex

#endif //__MYOCTANT_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/