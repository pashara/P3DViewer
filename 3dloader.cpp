#include "3dloader.h"
#include <iostream>
using namespace std;



//>------ ������� Chunk, � ������ ������� 3ds-�����
#define PRIMARY       0x4D4D

//>------ ������� Chunk-�
#define OBJECTINFO    0x3D3D            // ��� ������������� ������ ���� ����� ����������� �� �������
#define VERSION       0x0002            // ������������� ������ .3ds �����
#define EDITKEYFRAME  0xB000            // ����� ��� ���� ���������� � ������

//>------ ���-������� OBJECTINFO
#define MATERIAL      0xAFFF        // ���������� � ���������
#define OBJECT        0x4000        // ��������, �������, � �.�...

//>------ ���-������� ��� MATERIAL
#define MATNAME       0xA000            // �������� ���������
#define MATDIFFUSE    0xA020            // ������ ���� �������/���������
#define MATMAP        0xA200            // ����� ��� ������ ���������
#define MATMAPFILE    0xA300            // ������ ��� ����� ��������

#define OBJECT_MESH   0x4100            // ��� ��� �����, ��� �������� ��������� ����� ������

//>------ ���-������� ��� OBJECT_MESH
#define OBJECT_VERTICES     0x4110      // ������� �������
#define OBJECT_FACES        0x4120      // �������� �������
#define OBJECT_MATERIAL     0x4130      // ������ ���������, ���� ������ ����� ��������, ����� ����/��������
#define OBJECT_UV       0x4140      // UV ���������� ����������





// ������������ �������� ������� (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))











int gBuffer[50000] = { 0 };   // ������������ ��� ������ ������������� ������

							  // � ���� ����� ��������� ���� ���, ����������� ��� �������� ������ .3ds.
							  // ��� ��� ��������: �� ���������� chunk, ����� ���������� ��� ID.
							  // � ����������� �� ��� ID, ���������� ����������, ���������� � chunk-�.
							  // ���� �� �� ������ ������ ��� ����������, ������� ������ ��.
							  // �� ������, ��� ����� ���� ����� ����������, ��� ��� ������ chunk ������
							  // ���� ������ � ������.


							  ///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\*
							  /////
							  /////   ����������� �������������� ������ tChunk
							  /////
							  ///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\*

CLoad3DS::CLoad3DS()
{
	//m_FilePointer = NULL;
}

///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\*
/////
/////   ���������� �������� ��� ��������, ������ � ����� ������� .3ds
/////
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\*

bool CLoad3DS::Import3DS(t3DModel *pModel, std::string strFileName)
{
	char strMessage[255] = { 0 };
	tChunk currentChunk = { 0 };

	// ��������� .3ds ����
	//m_FilePointer = fopen(strFileName.c_str(), "rb");
	file.open(strFileName.c_str(), ios::in | ios::binary);

	// ��������, ��� ��������� �� ���� ����� (�� ������� ����)
	if (!file)
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// ������ ����, ����� ��������� ����� �����, ����� ��������, ��� ��� 3DS.
	// ���� ��� ������ ����, �� ������ ID chunk-� ����� PRIMARY

	// ������ ������ chunk �����, ����� ��������, ��� ��� 3DS
	ReadChunk(&currentChunk);

	// ��������, ��� ��� 3DS
	if (currentChunk.ID != PRIMARY)
	{
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// ������ �������� ������ ������. ProcessNextChunk() - ����������� �������

	// �������� �������� �������� ������� ����������� �������
	ProcessNextChunk(pModel, &currentChunk);

	// ����� ��������� ����� ����� ��� ����� ���������� ������� ������
	ComputeNormals(pModel);

	// � ����� ��������� ��
	CleanUp();

	return true;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ��� ������� ������ � ��������� ����
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\*

void CLoad3DS::CleanUp()
{
	/*if (m_FilePointer) {
	fclose(m_FilePointer);  // ��������� ����
	m_FilePointer = NULL;
	}*/
	if (file.is_open())
		file.close();
}


///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ������� ������ �����, ����� ���������� ��� ������
/////
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject = { 0 };      // ������������ ��� ���������� ������� � ������
	tMaterialInfo newTexture = { 0 };     // ������������ ��� ���������� ���������

	tChunk currentChunk = { 0 };      // ������� chunk ��� ��������
	tChunk tempChunk = { 0 };         // ��������� chunk ��� �������� ������

									  // ���� ��������� ID chunk-a ������ ��� ��� ������ ������. �����,
									  // ���� ����� �������� ������ �� chunk-�, ������ ���. ���� �� ���� chunk ���
									  // �� �����, ������ ������ chunk � "��������" ������.

									  // ���������� ������ ���������, ���� �� ������ �� ����� ������ �����.
									  // ����� ������ ���� ������, ����������� ����������� ����� � ����������
									  // �� � ����� �������.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// ������ ��������� chunk
		ReadChunk(&currentChunk);

		// �������� chunk ID
		switch (currentChunk.ID)
		{
		case VERSION:           // ������ �����

								// ������ ������ ����� � ��������� ����������� ����� � ���������� bytesRead
								//fread(gBuffer, 1, currentChunk.length -	currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;

			// ���� ������ ����� ������ 3, ������� ��������������, ��� �����
			// ���������� ��������.
			if ((currentChunk.length - currentChunk.bytesRead == 4) && (gBuffer[0] > 0x03)) {
				//MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly",
				//	"Warning", MB_OK);
			}
			break;

		case OBJECTINFO:        // �������� ������ ����
		{
			// ���� chunk �������� ������ ����. ����� ��� ��������� ��� chunk-�� MATERIAL
			// � OBJECT. ������ �� �������� ������ ���������� ���������� � ��������.

			// ������ ��������� chunk
			ReadChunk(&tempChunk);

			// �������� ������ ����
			//fread(gBuffer, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), tempChunk.length - tempChunk.bytesRead);
			tempChunk.bytesRead += tempChunk.length - tempChunk.bytesRead;

			// ����������� bytesRead �� ����� ����������� ����
			currentChunk.bytesRead += tempChunk.bytesRead;

			// ��������� � ���������� chunk-�, ��� ����� MATERIAL, ����� OBJECT
			ProcessNextChunk(pModel, &currentChunk);
			break;
		}
		case MATERIAL:          // �������� ���������� � ���������

								// ���� chunk - ����� ��� ���������� � ���������

								// ����������� ����� ����������
			pModel->numOfMaterials++;


			// ��������� ������ ��������� �������� � ��� ������ �������.
			pModel->pMaterials.push_back(newTexture);

			// �������� �������, �������������� ��������
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case OBJECT:            // ������ ��� ��������� �������

								// ���� chunk - ����� ��� chunk-��, �������� ���������� �������.
								// ����� �� ������ ��� �������.

								// ����������� ������� ��������
			pModel->numOfObjects++;

			// ��������� ����� ������� tObject � ������ ��������
			pModel->pObject.push_back(newObject);

			//pModel->pObject[pModel->numOfObjects - 1] = { 0 };
			// �������������� ������ � ��� ��� ������
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));

			// �������� � ��������� ��� �������, ����� ����������� ������� ����������� ����
			currentChunk.bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);

			// ��������� � ������ ���������� ���������� �������
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), &currentChunk);
			break;

		case EDITKEYFRAME:

			// ��� ��� � ����� ������� ������� ����, ��������� ��� ��������, � �� �������
			// ���������� � ���������� ��������. ���� chunk - ����� ��� ���� ����������
			// �� ��������. � ������� ������ ���� ������ ����� �������� ������.

			//ProcessNextKeyFrameChunk(pModel, currentChunk);

			// ������ � "��������" ��������� �������� ������ � ����������� �������
			//fread(gBuffer, 1, currentChunk.length -	currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		default:

			// ��������� chunk-�, ������� ��� �� �����, ����� ���������� �����. ���
			// �� ��� ����� ��������� � "��������" ���������� ����������� ��� ������������
			// chunk-� � ��������� ������� ����������� ����.
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;
		}

		// �������� ����������� ����� ���������� chunk-� � ��������
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\*
/////
/////   ������� ��������� ��� ���������� �� �������
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	// ������� chunk, � ������� ��������
	tChunk currentChunk = { 0 };

	// ���������� ������ ��� chunk-�, ���� �� ����� �� ����� ���� ������
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// ������ ��������� ������
		ReadChunk(&currentChunk);

		// ���������, ��� ��� �� ������
		switch (currentChunk.ID)
		{
		case OBJECT_MESH:       // ��� ��� �����, ��� �� ������ ����� ������

								// ����� ����� ������, ��������� ��� ���������� ���������
			ProcessNextObjectChunk(pModel, pObject, &currentChunk);
			break;

		case OBJECT_VERTICES:       // ������� ������ �������
			ReadVertices(pObject, &currentChunk);
			break;

		case OBJECT_FACES:      // �������� �������
			ReadVertexIndices(pObject, &currentChunk);
			break;

		case OBJECT_MATERIAL:       // ��� ��������� �������

									// ��� ������ ������ ��� ���������, ���������� � ������� ��������. ��� ����� ����
									// ���� ��� ���������� �����. ��� ������ ����� �������� ��������, � �������
									// ��������� �������� (���� �������� �� ������� ��������� �������, ��� ������
									// �������� �������� ������ �� ����� �������). ������ � ��� ����� ������ ����
									// �������� �� ���� ������, ��� ��� ������� ������ ID ���������.

									// ������ �� ������ ��� ���������, ������������ � �������
			ReadObjectMaterial(pModel, pObject, &currentChunk);
			break;

		case OBJECT_UV:     // ������ ���������� ���������� �������

							// ��� ������ �������� ��� UV-���������� �������. ��������� ��.
			ReadUVCoordinates(pObject, &currentChunk);
			break;

		default:

			// Read past the ignored or unknown chunks
			// ������ ������������/����������� ������ � "��������" ������
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;
		}

		// ���������� ����������� ������ � ��������
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\*
/////
/////   ��� ������� ������ ��� ���������� � ��������� (��������)
/////
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	// ������� chunk ��� ������
	tChunk currentChunk = { 0 };

	// ���������� ������ ��� chunk-�, ���� �� ����� �� ����� ���������
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// ������ ��������� ������
		ReadChunk(&currentChunk);

		// ���������, ��� ������ �� ���������
		switch (currentChunk.ID)
		{
		case MATNAME:       // ��� ������ ������ ��� ���������

							// ������ ��� ���������
							//fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(pModel->pMaterials[pModel->numOfMaterials - 1].strName), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		case MATDIFFUSE:        // ������ RGB-���� �������
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), &currentChunk);
			break;

		case MATMAP:            // ��� ����� ���������� � ��������

								// ������ ���������� ���������� � ���������
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case MATMAPFILE:        // ������ ��� ����� ���������

								// ������ ��� ����� ���������
								//fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile,1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(pModel->pMaterials[pModel->numOfMaterials - 1].strFile), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		default:

			// ������ ��������� ������ � "�����"
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;

			break;
		}

		// ���������� ����������� ������ � ��������
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ID chunk-� � ��� ������ � ������
/////
///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadChunk(tChunk *pChunk)
{
	// ������� ������ ID ������ (2 �����).
	// ID chunk-� - ���, ��������, OBJECT/MATERIAL. ��� ������� ���,
	// ����� ������ ����� ���� ��������� � ���� ������.
	//fread(&pChunk->ID, 1, 2, m_FilePointer);
	file.read((char*)&(pChunk->ID), 2);
	pChunk->bytesRead = 2;
	// ����� ������ ������ ������ (4 �����). ������ �� �����,
	// ������� ������ ��� ����� ����� ���������.
	//fread(&pChunk->length, 1, 4, m_FilePointer);
	file.read((char*)&(pChunk->length), 4);
	pChunk->bytesRead += 4;
}

///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\*
/////
/////   ������ ������ � ������ char-��
/////
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\*

int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;

	// ������ 1 ���� ������, ������ ����� ������
	//fread(pBuffer, 1, 1, m_FilePointer);
	file.read((char*)(pBuffer), 1);
	// ����, ���� �� �������� NULL
	while (*(pBuffer + index++) != 0) {

		// ������ ������� �� �����, ���� �� ������� NULL
		//fread(pBuffer + index, 1, 1, m_FilePointer);
		file.read((char*)(pBuffer + index), 1);
	}

	// ����� ������ ������, �.�. ������� ������ �� ��������� (������� NULL)
	return strlen(pBuffer) + 1;
}


///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\*
/////
/////   ������ ������ RGB-�����
/////
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{
	tChunk tempChunk = { 0 };

	// ������ ���������� � �����
	ReadChunk(&tempChunk);

	// ������ RGB-���� (3 ����� - �� 0 �� 255)
	//fread(pMaterial->color, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);
	file.read((char*)(pMaterial->color), tempChunk.length - tempChunk.bytesRead);
	tempChunk.bytesRead += tempChunk.length - tempChunk.bytesRead;

	// ����������� �������
	pChunk->bytesRead += tempChunk.bytesRead;
}


///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ������� ��� ������� ������
/////
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;       // ������������ ��� ������ ������� �������� ��������

									// ����� ��������� ������� ������ ��� �������, ����� ������� ��������� ��
									// �����, ����� ��� �� �����. ���������, ��� ����� ��������� ������ 3 ��
									// 4 �������� ��� ������� ��������. ��� ��������� �������� - ���� ���������
									// ��� 3DS Max, ������� ������ ��� ��� �� ������.

									// ������ ����� ��������� ����� �������
									//fread(&pObject->numOfFaces, 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numOfFaces), 2);
	pPreviousChunk->bytesRead += 2;

	// �������� ���������� ������ ��� ��������� � �������������� ���������
	pObject->pFaces = new tFace[pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	// �������� ����� ��� �������� ����� �������
	for (int i = 0; i < pObject->numOfFaces; i++)
	{
		// ����� ������ A-B-C ������� ��� ��������, �� ���������� 4-� ��������.
		for (int j = 0; j < 4; j++)
		{
			// ������ ������ ������ ������� ��� �������� ��������
			//fread(&index, 1, sizeof(index), m_FilePointer);
			file.read((char*)&(index), sizeof(index));
			pPreviousChunk->bytesRead += sizeof(index);

			if (j < 3)
			{
				// ��������� ������ � ��������� ���������
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}


///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ UV-���������� �������
/////
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// ����� ��������� ������� UV-��������� ��� �������, ������� �����
	// ��������� �� ������ ����������, ����� ��� �� �����.

	// ������ ����� UV-���������
	//fread(&pObject->numTexVertex, 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numTexVertex), 2);
	pPreviousChunk->bytesRead += 2;

	// �������� ������ ��� �������� UV-���������
	pObject->pTexVerts = new CVector2[pObject->numTexVertex];

	// ������ ���������� ���������� (������ �� 2� float)
	//fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(pObject->pTexVerts), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;
}


///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ������� �������
/////
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// ��� � � ����������� chunk-��, ������ ��� ������ ���� �������,
	// ����� ����� �� ����������.

	// ������ ����� ������
	//fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numOfVerts), 2);
	pPreviousChunk->bytesRead += 2;

	// �������� ������ ��� ������ � �������������� ���������
	pObject->pVerts = new CVector3[pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);

	// ������ � ������ ������ (������ �� 3 float)
	//fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(pObject->pVerts), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;

	// ������ ��� ������� ���������. ��� ��� � ������� 3DS Max ������ ����������
	// ���, ����� �������� Y-�������� � Z-�������� ����� ������.

	// �������� ����� ��� ������� � ������ y<->z
	for (int i = 0; i < pObject->numOfVerts; i++)
	{
		// ��������� ������ ����-� Y
		float fTempY = pObject->pVerts[i].y;

		// ������������� �������� Y � Z
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// ������������� �������� Z � Y
		// � ������ ��� �������������, �.�. � 3ds max Z-��� �����������
		pObject->pVerts[i].z = -fTempY;
	}
}


///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\*
/////
/////   ������� ������ ��� ���������, ����������� �� ������, � ������������� materialID
/////
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = { 0 };        // ������ ��� ���������

										  // *��� ����� ��������?* - �������� - ��� ���� + ���������� ����� �������.
										  // ����� �� ������������ ������ ���������� ���� �������, "�����������" � �.�.
										  // ������ ��� ����� ������ ���� ��� ��� ���������� �����.

										  // ������ ��� ���������, ������������ � �������� �������.
										  // strMaterial ������ ������ ��������� ������ � ������ ���������, ���� "Material #2" � �.�...
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// ������, ���� ��� ���������, ����� ������ ����� ��� ��������� � ��������� ��
	// ����� �� ���������� � �����. ����� ������ �������� � ������ ��� ����������� 
	// ������, ����������� materialID ������� � ������� ����� ���������. 

	// �������� ����� ��� ���������
	for (int i = 0; i < pModel->numOfMaterials; i++)
	{
		// ���� ������ ��� ����������� �������� ��������� � ������ �������
		if (strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			// ���������, ���� �� ���������� �����. ���� strFile ��������
			// ������ � ������� >=1, �������� ����.
			if (strlen(pModel->pMaterials[i].strFile) > 0)
			{
				// ������������� ID ��������� � ������� ������ 'i' � ����������� ��������
				pObject->materialID = i;

				// ������������� ���� ��������������� � true
				pObject->bHasTexture = true;
			}
			break;
		}
		else
		{
			// ��������� ����, ����� �������, ���� �� ��� �������� �� ���� �������
			if (pObject->bHasTexture != true)
			{
				// ������������� ID ��������� � -1, ����� �������, ��� ��������� ��� ������� ���
				pObject->materialID = -1;
			}
		}
	}

	// ��������� ������ � "�����"
	//fread(gBuffer, 1,pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(gBuffer), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;
}

// *Note*
//
// ���� ���� ��������� �������������� �������, ����������� ������� ������. ��� ���
// �����, ����� ������ ��������� ������������� �����. � ������� ������ �� ��� ������ 
// ��� �������, ��� ��� ��� ������� ����� ������ ���������� ����� 3dmath.h/.cpp

//////////////////////////////  Math Functions  ////////////////////////////////*

// ������������ �������� ������� (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// ������������ ������� ����� 2 ������� � ���������� ���������
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;           // ������ �������������� ������

	vVector.x = vPoint1.x - vPoint2.x;
	vVector.y = vPoint1.y - vPoint2.y;
	vVector.z = vPoint1.z - vPoint2.z;

	return vVector;             // ����� �������������� ������
}

// This adds 2 vectors together and returns the result
// ���������� 2 ������� � ���������� ���������
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;               // ������ �������������� ������

	vResult.x = vVector2.x + vVector1.x;
	vResult.y = vVector2.y + vVector1.y;
	vResult.z = vVector2.z + vVector1.z;

	return vResult;                 // ����� ���������
}

// ����� ������ �� ���������� ����� � ���������� ���������
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;

	vResult.x = vVector1.x / Scaler;
	vResult.y = vVector1.y / Scaler;
	vResult.z = vVector1.z / Scaler;

	return vResult;
}

// ���������� ��������� ������������ (dot product) ���� ��������
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;

	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;
}

// ���������� ������� �������
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;

	Magnitude = Mag(vNormal);

	vNormal.x /= (float)Magnitude;
	vNormal.y /= (float)Magnitude;
	vNormal.z /= (float)Magnitude;

	return vNormal;
}

///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\*
/////
/////   ������� ������������ ������� ��� ������� � ��� ������
/////
///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\*

void CLoad3DS::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// ���� �������� ���, ���������� ���� ���
	if (pModel->numOfObjects <= 0)
		return;

	// ��� ����� ������� ������? ��� ��� ���������� �� ��������� ��������? ���� ��
	// ����� ������� ������������, ��� "������� ��������". ���� �� �������� OpenGL
	// ������� �������� ��� ���������, ��� ������ ����� ��������� ������� � ������.
	// ���� �� �� ������� ������� ��� ������ �������, ���������� ������ �����
	// ��������� ����������, �.�. ����� ������������.

	// �������� ����� ��� ������� ��� ���������� �� ������
	for (int index = 0; index < pModel->numOfObjects; index++)
	{
		// ������� ������� ������
		t3DObject *pObject = &(pModel->pObject[index]);

		// �������� ������ ��� ������ ����������
		CVector3 *pNormals = new CVector3[pObject->numOfFaces];
		CVector3 *pTempNormals = new CVector3[pObject->numOfFaces];
		pObject->pNormals = new CVector3[pObject->numOfVerts];

		// �������� ����� ��� �������� �������
		for (int i = 0; i < pObject->numOfFaces; i++)
		{
			// ��������� 3 ����� ����� ��������, ����� �������� �������� ����
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// ������ �������� ������� ���������

			vVector1 = Vector(vPoly[0], vPoly[2]);  // ������ �������� (�� 2� ��� ������)
			vVector2 = Vector(vPoly[2], vPoly[1]);  // ������ ������ ��������

			vNormal = Cross(vVector1, vVector2);   // �������� cross product ��������
			pTempNormals[i] = vNormal;      // �������� ��������� ��-����������������� �������
											// ��� ������
			vNormal = Normalize(vNormal);      // ������������� cross product ��� �������� ��������

			pNormals[i] = vNormal;          // ��������� ������� � ������
		}

		//////////////// ������ �������� ��������� ������� /////////////////

		CVector3 vSum = { 0.0, 0.0, 0.0 };
		CVector3 vZero = vSum;
		int shared = 0;

		for (int i = 0; i < pObject->numOfVerts; i++)   // �������� ����� ��� �������
		{
			for (int j = 0; j < pObject->numOfFaces; j++)   // �������� ����� ��� ������������
			{               // ���������, ������������ �� ������� ������ ���������
				if (pObject->pFaces[j].vertIndex[0] == i ||
					pObject->pFaces[j].vertIndex[1] == i ||
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);    // ���������� ��-
																// ����������������� ������� ������� ��������
					shared++;       // ����������� ����� ��������� � ������ ���������
				}
			}

			// �������� ������� �������� �� ����� ����� ���������. ������ � �������.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// ����������� ������� ��� �������
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);

			vSum = vZero;           // ���������� �����
			shared = 0;         // � ����� ��������
		}

		// ����������� ������ ��������� ����������
		delete[] pTempNormals;
		delete[] pNormals;
	}
}
