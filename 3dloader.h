#pragma once
#include "3ddefindes.h"
#include <vector>
#include <math.h>
#include <fstream>



// ����� �������� ���� ��� ��������
class CLoad3DS
{
public:

	// ������ ���������� � chunk-�
	struct tChunk
	{
		unsigned short int ID;          // ID chunk-�
		unsigned int length;            // ������ chunk-�
		unsigned int bytesRead;         // ����� �������� ���� ��� ����� chunk-�
	};

	// ��������� ��� �������� 3DS (��� ��� .3ds ������ 4 unsigned short)
	struct tIndices {
		unsigned short a, b, c, bVisible;   // ��� ������ ������� ��� ����� 1,2,3 �������
											// ������, ���� ���� ���������
	};


	CLoad3DS();     // ��� ����� ����������������� ��� ������

					// ��� �-� � ����� ��������� ��� �������� 3DS
	bool Import3DS(t3DModel *pModel, std::string strFileName);

private:
	// ������ ������ � ��������� � � ���������� ������ char-��
	int GetString(char *);

	// ������ ��������� chunk
	void ReadChunk(tChunk *);

	// ������ ��������� ������� chunk
	void ProcessNextChunk(t3DModel *pModel, tChunk *);

	// ������ chunk-� �������
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);

	// ������ chunk-� ���������
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);

	// ������ RGB-�������� ����� �������
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);

	// ������ ������� �������
	void ReadVertices(t3DObject *pObject, tChunk *);

	// ������ ���������� ��������� �������
	void ReadVertexIndices(t3DObject *pObject, tChunk *);

	// ������ ���������� ���������� �������
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);

	// ������ ��� ���������, ������������ �������, � ������������� materialID
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);

	// ������������ ������� ������ �������
	void ComputeNormals(t3DModel *pModel);

	// This frees memory and closes the file
	// ����������� ������ � ��������� ����
	void CleanUp();

	fstream file;
};