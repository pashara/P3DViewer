#pragma once

// Сначала включим новые хидеры:
#include <vector>
#include <math.h>
// Обьявим пространство имён:
using namespace std;

// Структура 3D-точки
struct CVector3 {
	float x, y, z;
};

// Структура 2D-точки
struct CVector2 {
	float x, y;
};

// Это структура полигона. Она используется для индексирования массивов координат
// вершин и текстур. Эта информация сообщает нам о том, какие номера вершин в массиве
// какому полигону принадлежат. То же самое касается текстурных координат.
struct tFace
{
	int vertIndex[3];           // indicies for the verts that make up this triangle
	int coordIndex[3];          // indicies for the tex coords to texture this face
};


// Эта структура хранит информацию о материале. Это может быть текстурная карта света.
// Некоторые значения не используются, но я оставил их, чтобы могли увидеть их для
// примера.
struct tMaterialInfo
{
	char  strName[255];         // Имя текстуры
	char  strFile[255];         // Имя файла текстуры
	unsigned char  color[3];             // Цвет обьекта (R, G, B)
	int   texureId;             // ID текстуры
	float uTile;                // u-tiling текстуры (Сейчас не используется)
	float vTile;                // v-tiling текстуры (Сейчас не используется)
	float uOffset;              // u-offset текстуры (Сейчас не используется)
	float vOffset;              // v-offset текстуры (Сейчас не используется)
};




// Содержит всю информацию о модели/сцене.
// В реальном проекте лучше оберните всё это в класс с 
// функциями вроде LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
	int  numOfVerts;            // Число вершин в модели
	int  numOfFaces;            // Число полигонов в модели
	int  numTexVertex;          // Число текстурных координат
	int  materialID;            // ID текстуры для использования, индекс массива текстур
	bool bHasTexture;           // TRUE если есть текстурная карта для этого обьекта
	char strName[255];          // Имя обьекта
	CVector3  *pVerts;          // Массив вершин обьекта
	CVector3  *pNormals;            // Нормали обьекта
	CVector2  *pTexVerts;           // Текстурные координаты
	tFace *pFaces;              // Полигоны обьекта
};



// Содержит информацию о модели. Тоже неплохо бы обернуть в класс. Мы будем использовать
// класс вектора из STL (Standart Template Library) чтобы уменьшить трудности при связывании
// параметров.
struct t3DModel
{
	int numOfObjects;           // Число обьектов в модели
	int numOfMaterials;         // Число материалов модели
	vector<tMaterialInfo> pMaterials;   // Число обьектов материалов (текстуры и цвета)
	vector<t3DObject> pObject;      // Список обьектов в модели
};
