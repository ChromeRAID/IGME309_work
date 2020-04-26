
#include "MyOctant.h"

using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

void MyOctant::Init(void) {


	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_uID = m_uOctantCount;
	m_pRoot = nullptr;
	m_pParent = nullptr;

	for (size_t i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}
}

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) {
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> boundingObjs;
	uint objs = m_pEntityMngr->GetEntityCount();

	for (size_t i = 0; i < objs; i++) {
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* rb = entity->GetRigidBody();
		boundingObjs.push_back(rb->GetMinGlobal());
		boundingObjs.push_back(rb->GetMaxGlobal());
	}
	MyRigidBody rb(boundingObjs);
	vector3 halfWidth = rb.GetHalfWidth();
	float max = halfWidth.x;

	for (size_t i = 1; i < 3; i++) {
		if (max < halfWidth[i]) {
			max = halfWidth[i];
		}
	}
	vector3 center = rb.GetCenterLocal();
	boundingObjs.clear();

	m_fSize = max * 2.0f;
	m_v3Center = center;
	m_v3Min = m_v3Center - vector3(max);
	m_v3Max = m_v3Center + vector3(max);

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);

}
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize) {
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Min = m_v3Center - vector3(m_fSize) / 2.0f;
	m_v3Max = m_v3Center + vector3(m_fSize) / 2.0f;

	m_uOctantCount++;
}
MyOctant::MyOctant(MyOctant const& other) {

	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Max = other.m_v3Max;
	m_v3Min = other.m_v3Min;
	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;
	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (size_t i = 0; i < 8; i++)
	{
		m_pChild[i], other.m_pChild[i];
	}
}
//swaps this octants information with another
void MyOctant::Swap(MyOctant& other) {
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	for (size_t i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
MyOctant* MyOctant::GetParent(void) {
	return m_pParent;
}
//clear all lists in octants
void MyOctant::Release(void) {
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0;
	m_EntityList.clear();
	m_lChild.clear();
}

//assignment operator
MyOctant& MyOctant::operator=(MyOctant const& other) {
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
///destructor
MyOctant::~MyOctant() {
	Release();
}

float MyOctant::GetSize(void) {
	return m_fSize;
}
vector3 MyOctant::GetCenterGlobal(void) {
	return m_v3Center;
}
vector3 MyOctant::GetMaxGlobal(void) {
	return m_v3Max;
}
vector3 MyOctant::GetMinGlobal(void) {
	return m_v3Min;
}
//displays the octants
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) {
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	for (size_t i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
	
}
//displays the octants
void MyOctant::Display(vector3 a_v3Color) {
	for (size_t i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
//creates 8 potential octants
void MyOctant::Subdivide(void) {
	if (m_uLevel >= m_uMaxLevel || m_uChildren != 0) {
		return;
	}
	m_uChildren = 8;
	float size = m_fSize / 4.0f;
	float sizeD = size * 2.0f;
	vector3 center;
	//o0
	center = m_v3Center;
	center.x -= size;
	center.y -= size;
	center.z -= size;
	m_pChild[0] = new MyOctant(center, sizeD);
	//o1
	center.x += sizeD;
	m_pChild[1] = new MyOctant(center, sizeD);
	//o2
	center.z += sizeD;
	m_pChild[2] = new MyOctant(center, sizeD);
	//o3
	center.x -= sizeD;
	m_pChild[3] = new MyOctant(center, sizeD);
	//o4
	center.y += sizeD;
	m_pChild[4] = new MyOctant(center, sizeD);
	//o5
	center.z -= sizeD;
	m_pChild[5] = new MyOctant(center, sizeD);
	//o6
	center.x += sizeD;
	m_pChild[6] = new MyOctant(center, sizeD);
	//o7
	center.z += sizeD;
	m_pChild[7] = new MyOctant(center, sizeD);
	//assign data to children and subdivide them too
	for (size_t i = 0; i < 8; i++) {
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount)) {
			m_pChild[i]->Subdivide();
		}
	}

}
//get child by index
MyOctant* MyOctant::GetChild(uint a_nChild) {
	if (a_nChild > 7)
	{
		return nullptr;
	}
	return m_pChild[a_nChild];
}
//check if the index of the scene object is within the octant
bool MyOctant::IsColliding(uint a_uRBIndex) {
	uint objCount = m_pEntityMngr->GetEntityCount();

	if (a_uRBIndex >= objCount) {
		return false;
	}
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* rb = entity->GetRigidBody();
	vector3 min = rb->GetMinGlobal();
	vector3 max = rb->GetMaxGlobal();

	if (m_v3Max.x <= max.x || m_v3Min.x >= min.x ||
		m_v3Max.y <= max.y || m_v3Min.y >= min.y ||
		m_v3Max.z <= max.z || m_v3Min.z >= min.z) {
		return false;
	}
	
	return true;
}
//check if the octant is a leaf
bool MyOctant::IsLeaf(void) {
	if (m_uChildren == 0) {
		return true;
	}
	return false;
}
//check to see how many objects are within the  octant
bool MyOctant::ContainsMoreThan(uint a_nEntities) {
	uint count = 0;
	uint objCount = m_pEntityMngr->GetEntityCount();

	for (size_t i = 0; i < objCount; i++) {
		if (IsColliding(i)) { //check collisions
			count++;
		}
		if (count > a_nEntities) { //more than 5 so subdivide
			return true;
		}
	}
	return false;
}
//destroy children 
void MyOctant::KillBranches(void) {
	for (size_t i = 0; i < m_uChildren; i++) {
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}
//displays octants with no children
void MyOctant::DisplayLeafs(vector3 a_v3Color) {
	uint count = m_lChild.size();
	for (size_t i = 0; i < count; i++) {
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
//clears entity list for all children and self
void MyOctant::ClearEntityList(void) {
	for (size_t i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}
//initializes tree and starts subdivisions
void MyOctant::ConstructTree(uint a_nMaxLevel) {

	if (m_uLevel != 0) {
		return;
	}
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	//clear out lists
	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();
	//subdivide if contains a lot of cubes
	if (ContainsMoreThan(m_uIdealEntityCount)) {
		Subdivide();
	}
	//give ID's and construct parental list
	AssignIDtoEntity();
	ConstructList();
}
//recursively set the ID of containing cubes to children
void MyOctant::AssignIDtoEntity(void) {
	for (size_t i = 0; i < m_uChildren; i++) {//loop through children
		m_pChild[i]->AssignIDtoEntity();
	}
	if (m_uChildren == 0) {
		uint entities = m_pEntityMngr->GetEntityCount();
		for (size_t i = 0; i < entities; i++) {
			if (IsColliding(i)) { //if the box is colliding add to entity list
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}
//recursively set the children of the tree
void MyOctant::ConstructList(void) {
	for (size_t i = 0; i < m_uChildren; i++) { //loop through children
		m_pChild[i]->ConstructList();
	}
	if (m_EntityList.size() > 0) {
		m_pRoot->m_lChild.push_back(this); //set this to a child of the root
	}
}
//return the octant count
uint MyOctant::GetOctantCount(void) { 
	return m_uOctantCount; 
}

