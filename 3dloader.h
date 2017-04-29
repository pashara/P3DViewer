#pragma once
#include "3ddefindes.h"
#include <vector>
#include <math.h>
#include <fstream>



// Класс содержит весь код загрузки
class CLoad3DS
{
public:

	// Хранит информацию о chunk-е
	struct tChunk
	{
		unsigned short int ID;          // ID chunk-а
		unsigned int length;            // Длинна chunk-а
		unsigned int bytesRead;         // Число читаемых байт для этого chunk-а
	};

	// Структура для индексов 3DS (так как .3ds хранит 4 unsigned short)
	struct tIndices {
		unsigned short a, b, c, bVisible;   // Это хранит индексы для точки 1,2,3 массива
											// вершин, плюс флаг видимости
	};


	CLoad3DS();     // Тут будут инициализироватся все данные

					// Эта ф-я и будет вызыватся для загрузки 3DS
	bool Import3DS(t3DModel *pModel, std::string strFileName);

private:
	// Читает строку и сохраняет её в переданный массив char-ов
	int GetString(char *);

	// Читает следующий chunk
	void ReadChunk(tChunk *);

	// Читает следующий длинный chunk
	void ProcessNextChunk(t3DModel *pModel, tChunk *);

	// Читает chunk-и обьекта
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);

	// Читает chunk-и материала
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);

	// Читает RGB-значение цвета обьекта
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);

	// Читает вершины обьекта
	void ReadVertices(t3DObject *pObject, tChunk *);

	// Читает информацию полигонов обьекта
	void ReadVertexIndices(t3DObject *pObject, tChunk *);

	// Читает текстурные координаты обьекта
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);

	// Читает имя материала, присвоенного обьекту, и устанавливает materialID
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);

	// Рассчитывает нормали вершин обьекта
	void ComputeNormals(t3DModel *pModel);

	// This frees memory and closes the file
	// Освобождает память и закрывает файл
	void CleanUp();

	fstream file;
};