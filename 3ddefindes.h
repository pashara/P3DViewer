#pragma once

// ������� ������� ����� ������:
#include <vector>
#include <math.h>
// ������� ������������ ���:
using namespace std;

// ��������� 3D-�����
struct CVector3 {
	float x, y, z;
};

// ��������� 2D-�����
struct CVector2 {
	float x, y;
};

// ��� ��������� ��������. ��� ������������ ��� �������������� �������� ���������
// ������ � �������. ��� ���������� �������� ��� � ���, ����� ������ ������ � �������
// ������ �������� �����������. �� �� ����� �������� ���������� ���������.
struct tFace
{
	int vertIndex[3];           // indicies for the verts that make up this triangle
	int coordIndex[3];          // indicies for the tex coords to texture this face
};


// ��� ��������� ������ ���������� � ���������. ��� ����� ���� ���������� ����� �����.
// ��������� �������� �� ������������, �� � ������� ��, ����� ����� ������� �� ���
// �������.
struct tMaterialInfo
{
	char  strName[255];         // ��� ��������
	char  strFile[255];         // ��� ����� ��������
	unsigned char  color[3];             // ���� ������� (R, G, B)
	int   texureId;             // ID ��������
	float uTile;                // u-tiling �������� (������ �� ������������)
	float vTile;                // v-tiling �������� (������ �� ������������)
	float uOffset;              // u-offset �������� (������ �� ������������)
	float vOffset;              // v-offset �������� (������ �� ������������)
};




// �������� ��� ���������� � ������/�����.
// � �������� ������� ����� �������� �� ��� � ����� � 
// ��������� ����� LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
	int  numOfVerts;            // ����� ������ � ������
	int  numOfFaces;            // ����� ��������� � ������
	int  numTexVertex;          // ����� ���������� ���������
	int  materialID;            // ID �������� ��� �������������, ������ ������� �������
	bool bHasTexture;           // TRUE ���� ���� ���������� ����� ��� ����� �������
	char strName[255];          // ��� �������
	CVector3  *pVerts;          // ������ ������ �������
	CVector3  *pNormals;            // ������� �������
	CVector2  *pTexVerts;           // ���������� ����������
	tFace *pFaces;              // �������� �������
};



// �������� ���������� � ������. ���� ������� �� �������� � �����. �� ����� ������������
// ����� ������� �� STL (Standart Template Library) ����� ��������� ��������� ��� ����������
// ����������.
struct t3DModel
{
	int numOfObjects;           // ����� �������� � ������
	int numOfMaterials;         // ����� ���������� ������
	vector<tMaterialInfo> pMaterials;   // ����� �������� ���������� (�������� � �����)
	vector<t3DObject> pObject;      // ������ �������� � ������
};
