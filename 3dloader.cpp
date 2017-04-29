#include "3dloader.h"
#include <iostream>
using namespace std;



//>------ Главный Chunk, в начале каждого 3ds-файла
#define PRIMARY       0x4D4D

//>------ Главнык Chunk-и
#define OBJECTINFO    0x3D3D            // Это предоставляет версию меша перед информацией об обьекте
#define VERSION       0x0002            // Предоставляет версию .3ds файла
#define EDITKEYFRAME  0xB000            // Хидер для всей информации о кадрах

//>------ под-дефайны OBJECTINFO
#define MATERIAL      0xAFFF        // Информация о текстурах
#define OBJECT        0x4000        // Полигоны, вершины, и т.д...

//>------ под-дефайны для MATERIAL
#define MATNAME       0xA000            // Название материала
#define MATDIFFUSE    0xA020            // Хранит цвет обьекта/материала
#define MATMAP        0xA200            // Хидер для нового материала
#define MATMAPFILE    0xA300            // Хранит имя файла текстуры

#define OBJECT_MESH   0x4100            // Даёт нам знать, что начинаем считывать новый обьект

//>------ под-дефайны для OBJECT_MESH
#define OBJECT_VERTICES     0x4110      // Вершины обьекта
#define OBJECT_FACES        0x4120      // Полигоны обьекта
#define OBJECT_MATERIAL     0x4130      // Дефайн находится, если обьект имеет материал, иначе цвет/текстура
#define OBJECT_UV       0x4140      // UV текстурные координаты





// Рассчитывает величину нормали (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))











int gBuffer[50000] = { 0 };   // Используется для чтения нежелательных данных

							  // В этом файле находится весь код, необходимый для загрузки файлов .3ds.
							  // Как оно работает: вы загружаете chunk, затем проверяете его ID.
							  // В зависимости от его ID, загружаете информацию, хранящуюся в chunk-е.
							  // Если вы не хотите читать эту информацию, читаете дальше неё.
							  // Вы знаете, как много байт нужно пропустить, так как каждый chunk хранит
							  // свою длинну в байтах.


							  ///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\*
							  /////
							  /////   Конструктор инициализирует данные tChunk
							  /////
							  ///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\*

CLoad3DS::CLoad3DS()
{
	//m_FilePointer = NULL;
}

///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\*
/////
/////   Вызывается клиентом для открытия, чтения и затем очистки .3ds
/////
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\*

bool CLoad3DS::Import3DS(t3DModel *pModel, std::string strFileName)
{
	char strMessage[255] = { 0 };
	tChunk currentChunk = { 0 };

	// Открываем .3ds файл
	//m_FilePointer = fopen(strFileName.c_str(), "rb");
	file.open(strFileName.c_str(), ios::in | ios::binary);

	// Убедимся, что указатель на файл верен (мы открыли файл)
	if (!file)
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Открыв файл, нужно прочитать хидер файла, чтобы убедится, что это 3DS.
	// Если это верный файл, то первым ID chunk-а будет PRIMARY

	// Читаем первый chunk файла, чтобы убедится, что это 3DS
	ReadChunk(&currentChunk);

	// Убедимся, что это 3DS
	if (currentChunk.ID != PRIMARY)
	{
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		//MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Теперь начинаем чтение данных. ProcessNextChunk() - рекурсивная функция

	// Начинаем загрузку обьектов вызовом рекурсивной функции
	ProcessNextChunk(pModel, &currentChunk);

	// После прочтения всего файла нам нужно рассчитать нормали вершин
	ComputeNormals(pModel);

	// В конце подчищаем всё
	CleanUp();

	return true;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\*
/////
/////   Функция чистит всю занятую память и закрывает файл
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\*

void CLoad3DS::CleanUp()
{
	/*if (m_FilePointer) {
	fclose(m_FilePointer);  // Закрываем файл
	m_FilePointer = NULL;
	}*/
	if (file.is_open())
		file.close();
}


///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\*
/////
/////   Функция читает главную секцию файла, затем рекурсивно идёт глубже
/////
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject = { 0 };      // Используется для добавления обьекта в список
	tMaterialInfo newTexture = { 0 };     // Используется для добавления материала

	tChunk currentChunk = { 0 };      // Текущий chunk для загрузки
	tChunk tempChunk = { 0 };         // Временный chunk для хранения данных

									  // Ниже проверяем ID chunk-a каждый раз при чтении нового. Затем,
									  // если нужно вытащить данные из chunk-а, делаем это. Если же этот chunk нам
									  // не нужен, просто читаем chunk в "мусорный" массив.

									  // Продолжаем читать подсекции, пока не дойдем до общей длинны файла.
									  // После чтения ЧЕГО УГОДНО, увеличиваем прочитанные байты и сравниваем
									  // их с общей длинной.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Читаем следующий chunk
		ReadChunk(&currentChunk);

		// Получаем chunk ID
		switch (currentChunk.ID)
		{
		case VERSION:           // Версия файла

								// Читаем версию файла и добавляем прочитанные байты в переменную bytesRead
								//fread(gBuffer, 1, currentChunk.length -	currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;

			// Если версия файла больше 3, выведем предупреждение, что могут
			// возникнуть проблемы.
			if ((currentChunk.length - currentChunk.bytesRead == 4) && (gBuffer[0] > 0x03)) {
				//MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly",
				//	"Warning", MB_OK);
			}
			break;

		case OBJECTINFO:        // Содержит версию меша
		{
			// Этот chunk содержит версию меша. Также это заголовок для chunk-ов MATERIAL
			// и OBJECT. Отсюда мы начинаем читать информацию материалов и обьектов.

			// Читаем следующий chunk
			ReadChunk(&tempChunk);

			// Получаем версию меша
			//fread(gBuffer, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), tempChunk.length - tempChunk.bytesRead);
			tempChunk.bytesRead += tempChunk.length - tempChunk.bytesRead;

			// Увеличиваем bytesRead на число прочитанных байт
			currentChunk.bytesRead += tempChunk.bytesRead;

			// Переходим к следующему chunk-у, это будет MATERIAL, затем OBJECT
			ProcessNextChunk(pModel, &currentChunk);
			break;
		}
		case MATERIAL:          // Содержит информацию о материале

								// Этот chunk - хидер для информации о материале

								// Увеличиваем число материалов
			pModel->numOfMaterials++;


			// Добавляем пустую структуру текстуры в наш массив текстур.
			pModel->pMaterials.push_back(newTexture);

			// Вызываем функцию, обрабатывающую материал
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case OBJECT:            // Хранит имя читаемого обьекта

								// Этот chunk - хидер для chunk-ов, хранящих информацию обьекта.
								// Также он хранит имя обьекта.

								// Увеличиваем счетчик обьектов
			pModel->numOfObjects++;

			// Добавляем новый элемент tObject к списку обьектов
			pModel->pObject.push_back(newObject);

			//pModel->pObject[pModel->numOfObjects - 1] = { 0 };
			// Инициализируем обьект и все его данные
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));

			// Получаем и сохраняем имя обьекта, затем увеличиваем счетчик прочитанных байт
			currentChunk.bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);

			// Переходим к чтению оставшейся информации обьекта
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), &currentChunk);
			break;

		case EDITKEYFRAME:

			// Так как я хотел сделать ПРОСТОЙ урок, насколько это возможно, я не включил
			// информацию о покадровой анимации. Этот chunk - хидер для всей информации
			// об анимации. В будущих уроках этот аспект будет детально описан.

			//ProcessNextKeyFrameChunk(pModel, currentChunk);

			// Читаем в "мусорный" контейнер ненужные данные и увеличиваем счетчик
			//fread(gBuffer, 1, currentChunk.length -	currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		default:

			// Остальные chunk-и, которые нам не нужны, будут обработаны здесь. Нам
			// всё ещё нужно прочитать в "мусорную" переменную неизвестные или игнорируемые
			// chunk-и и увеличить счетчик прочитанных байт.
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;
		}

		// Прибавим прочитанные байты последнего chunk-а к счетчику
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\*
/////
/////   Функция сохраняет всю информацию об обьекте
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Текущий chunk, с которым работаем
	tChunk currentChunk = { 0 };

	// Продолжаем читать эти chunk-и, пока не дошли до конца этой секции
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Читаем следующую секцию
		ReadChunk(&currentChunk);

		// Проверяем, что это за секция
		switch (currentChunk.ID)
		{
		case OBJECT_MESH:       // Даёт нам знать, что мы читаем новый обьект

								// Нашли новый обьект, прочитаем его информацию рекурсией
			ProcessNextObjectChunk(pModel, pObject, &currentChunk);
			break;

		case OBJECT_VERTICES:       // Вершины нашего обьекта
			ReadVertices(pObject, &currentChunk);
			break;

		case OBJECT_FACES:      // Полигоны обьекта
			ReadVertexIndices(pObject, &currentChunk);
			break;

		case OBJECT_MATERIAL:       // Имя материала обьекта

									// Эта секция хранит имя материала, связанного с текущим обьектом. Это может быть
									// цвет или текстурная карта. Эта секция также содержит полигоны, к которым
									// привязана текстура (Если например на обьекте несколько текстур, или просто
									// текстура наложено только на часть обьекта). Сейчас у нас будет только одна
									// текстура на весь обьект, так что получим только ID материала.

									// Теперь мы читаем имя материала, привязанного к обьекту
			ReadObjectMaterial(pModel, pObject, &currentChunk);
			break;

		case OBJECT_UV:     // Хранит текстурные координаты обьекта

							// Эта секция содержит все UV-координаты обьекта. Прочитаем их.
			ReadUVCoordinates(pObject, &currentChunk);
			break;

		default:

			// Read past the ignored or unknown chunks
			// Читаем игнорируемые/неизвестные данные в "мусорный" массив
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;
		}

		// Прибавляем прочитанные данные к счетчику
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\*
/////
/////   Эта функция хранит всю информацию о материале (текстуре)
/////
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	// Текущий chunk для работы
	tChunk currentChunk = { 0 };

	// Продолжаем читать эти chunk-и, пока не дошли до конца подсекции
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Читаем следующую секцию
		ReadChunk(&currentChunk);

		// Проверяем, что именно мы прочитали
		switch (currentChunk.ID)
		{
		case MATNAME:       // Эта секция хранит имя материала

							// читаем имя материала
							//fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(pModel->pMaterials[pModel->numOfMaterials - 1].strName), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		case MATDIFFUSE:        // Хранит RGB-цвет обьекта
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), &currentChunk);
			break;

		case MATMAP:            // Это хидер информации о текстуре

								// Читаем информацию информацию о материале
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case MATMAPFILE:        // Хранит имя файла материала

								// Читаем имя файла материала
								//fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile,1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(pModel->pMaterials[pModel->numOfMaterials - 1].strFile), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;
			break;

		default:

			// Читаем остальные данные в "мусор"
			//fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			file.read((char*)(gBuffer), currentChunk.length - currentChunk.bytesRead);
			currentChunk.bytesRead += currentChunk.length - currentChunk.bytesRead;

			break;
		}

		// Прибавляем прочитанные данные к счетчику
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\*
/////
/////   Функция читает ID chunk-а и его длинну в байтах
/////
///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadChunk(tChunk *pChunk)
{
	// Функция читает ID секции (2 байта).
	// ID chunk-а - это, например, OBJECT/MATERIAL. Это говорит нам,
	// какие данные могут быть прочитаны в этой секции.
	//fread(&pChunk->ID, 1, 2, m_FilePointer);
	file.read((char*)&(pChunk->ID), 2);
	pChunk->bytesRead = 2;
	// Затем читаем длинну секции (4 байта). Теперь мы знаем,
	// сколько данных нам нужно будет прочитать.
	//fread(&pChunk->length, 1, 4, m_FilePointer);
	file.read((char*)&(pChunk->length), 4);
	pChunk->bytesRead += 4;
}

///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\*
/////
/////   Читает строку в массив char-ов
/////
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\*

int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;

	// Читаем 1 байт данных, первую букву строки
	//fread(pBuffer, 1, 1, m_FilePointer);
	file.read((char*)(pBuffer), 1);
	// Цикл, пока не получаем NULL
	while (*(pBuffer + index++) != 0) {

		// Читаем символы всё время, пока не получим NULL
		//fread(pBuffer + index, 1, 1, m_FilePointer);
		file.read((char*)(pBuffer + index), 1);
	}

	// Вернём длинну строки, т.е. сколько байтов мы прочитали (включая NULL)
	return strlen(pBuffer) + 1;
}


///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\*
/////
/////   Читает данные RGB-цвета
/////
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{
	tChunk tempChunk = { 0 };

	// Читаем информацию о цвете
	ReadChunk(&tempChunk);

	// Читаем RGB-цвет (3 байта - от 0 до 255)
	//fread(pMaterial->color, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);
	file.read((char*)(pMaterial->color), tempChunk.length - tempChunk.bytesRead);
	tempChunk.bytesRead += tempChunk.length - tempChunk.bytesRead;

	// Увеличиваем счетчик
	pChunk->bytesRead += tempChunk.bytesRead;
}


///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\*
/////
/////   Функция читает индексы для массива вершин
/////
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;       // Используется для чтения индекса текущего полигона

									// Чтобы прочитать индексы вершин для обьекта, нужно сначала прочитать их
									// число, затем уже их самих. Запомните, нам нужно прочитать только 3 из
									// 4 значений для каждого полигона. Это четвертое значение - флаг видимости
									// для 3DS Max, которое ничего для нас не значит.

									// Читаем число полигонов этого обьекта
									//fread(&pObject->numOfFaces, 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numOfFaces), 2);
	pPreviousChunk->bytesRead += 2;

	// Выделяем достаточно памяти для полигонов и инициализируем структуру
	pObject->pFaces = new tFace[pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	// Проходим через все полигоны этого обьекта
	for (int i = 0; i < pObject->numOfFaces; i++)
	{
		// Далее читаем A-B-C индексы для полигона, но игнорируем 4-е значение.
		for (int j = 0; j < 4; j++)
		{
			// Читаем первый индекс вершины для текущего полигона
			//fread(&index, 1, sizeof(index), m_FilePointer);
			file.read((char*)&(index), sizeof(index));
			pPreviousChunk->bytesRead += sizeof(index);

			if (j < 3)
			{
				// Сохраняем индекс в структуру полигонов
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}


///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\*
/////
/////   Функция читает UV-координаты обьекта
/////
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Чтобы прочитать индексы UV-координат для обьекта, сначала нужно
	// прочитать их полное количество, потом уже их самих.

	// Читаем число UV-координат
	//fread(&pObject->numTexVertex, 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numTexVertex), 2);
	pPreviousChunk->bytesRead += 2;

	// Выделяем память для хранения UV-координат
	pObject->pTexVerts = new CVector2[pObject->numTexVertex];

	// Читаем текстурные координаты (массив из 2х float)
	//fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(pObject->pTexVerts), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;
}


///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\*
/////
/////   Функция читает вершины обьекта
/////
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Как и в большинстве chunk-ов, прежде чем читать сами вершины,
	// нужно найти их количество.

	// Читаем число вершин
	//fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);
	file.read((char*)&(pObject->numOfVerts), 2);
	pPreviousChunk->bytesRead += 2;

	// Выделяем память для вершин и инициализируем структуру
	pObject->pVerts = new CVector3[pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);

	// Читаем в массив вершин (массив из 3 float)
	//fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(pObject->pVerts), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;

	// Теперь все вершины прочитаны. Так как в моделях 3DS Max всегда перевёрнуты
	// оси, нужно поменять Y-значения и Z-значения наших вершин.

	// Проходим через все вершины и меняем y<->z
	for (int i = 0; i < pObject->numOfVerts; i++)
	{
		// Сохраняем старое знач-е Y
		float fTempY = pObject->pVerts[i].y;

		// Устанавливаем значение Y в Z
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// Устанавливаем значение Z в Y
		// И делаем его отрицательным, т.к. в 3ds max Z-ось перевернута
		pObject->pVerts[i].z = -fTempY;
	}
}


///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\*
/////
/////   Функция читает имя материала, наложенного на обьект, и устанавливает materialID
/////
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\*

void CLoad3DS::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = { 0 };        // Хранит имя материала

										  // *Что такое материал?* - Материал - это цвет + текстурная карта обьекта.
										  // Также он можетхранить другую информацию типа яркости, "блестящести" и т.д.
										  // Сейчас нам нужно только цвет или имя текстурной карты.

										  // Читаем имя материала, привязанного к текущему обьекту.
										  // strMaterial теперь должен содержать строку с именем материала, типа "Material #2" и т.д...
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Теперь, имея имя материала, нужно пройти через все материалы и проверять их
	// имена на совпадение с нашим. Когда найдем материал с только что прочитанным 
	// именем, привязываем materialID обьекта к индексу этого материала. 

	// Проходим через все материалы
	for (int i = 0; i < pModel->numOfMaterials; i++)
	{
		// Если только что прочитанный материал совпадает с именем данного
		if (strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			// Проверяем, есть ли текстурная карта. Если strFile содержит
			// строку с длинной >=1, текстура есть.
			if (strlen(pModel->pMaterials[i].strFile) > 0)
			{
				// Устанавливаем ID материала в текущий индекс 'i' и заканчиваем проверку
				pObject->materialID = i;

				// Устанавливаем флаг текстурирования в true
				pObject->bHasTexture = true;
			}
			break;
		}
		else
		{
			// Проверяем флаг, чтобы увидеть, есть ли уже текстура на этом обьекте
			if (pObject->bHasTexture != true)
			{
				// Устанавливаем ID материала в -1, чтобы указать, что материала для обьекта нет
				pObject->materialID = -1;
			}
		}
	}

	// Остальное читаем в "мусор"
	//fread(gBuffer, 1,pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
	file.read((char*)(gBuffer), pPreviousChunk->length - pPreviousChunk->bytesRead);
	pPreviousChunk->bytesRead += pPreviousChunk->length - pPreviousChunk->bytesRead;
}

// *Note*
//
// Ниже идут несколько математических функций, вычисляющих нормали вершин. Они нам
// нужны, чтобы эффект освещения рассчитывался верно. В прошлых уроках мы уже писали 
// эти функции, так что при желании можно просто подключить файлы 3dmath.h/.cpp

//////////////////////////////  Math Functions  ////////////////////////////////*

// Рассчитывает величину нормали (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// Рассчитывает векторы между 2 точками и возвращает результат
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;           // Хранит результирующий вектор

	vVector.x = vPoint1.x - vPoint2.x;
	vVector.y = vPoint1.y - vPoint2.y;
	vVector.z = vPoint1.z - vPoint2.z;

	return vVector;             // Вернём результирующий вектор
}

// This adds 2 vectors together and returns the result
// Складывает 2 вектора и возвращает результат
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;               // Хранит результирующий вектор

	vResult.x = vVector2.x + vVector1.x;
	vResult.y = vVector2.y + vVector1.y;
	vResult.z = vVector2.z + vVector1.z;

	return vResult;                 // Вернём результат
}

// Делит вектор на переданный номер и возвращает результат
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;

	vResult.x = vVector1.x / Scaler;
	vResult.y = vVector1.y / Scaler;
	vResult.z = vVector1.z / Scaler;

	return vResult;
}

// Возвращает скалярное произведение (dot product) двух векторов
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;

	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;
}

// Возвращает нормаль вектора
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
/////   Функция рассчитывает нормали для обьекта и его вершин
/////
///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\*

void CLoad3DS::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// Если обьектов нет, пропускаем этот шаг
	if (pModel->numOfObjects <= 0)
		return;

	// Что такое нормали вершин? Чем они отличаются от остальных нормалей? Если вы
	// нашли нормаль треугольника, это "нормаль полигона". Если вы передали OpenGL
	// нормаль полигона для освещения, ваш обьект будет выглядеть плоским и резким.
	// Если же вы найдете нормали для каждой вершины, освещенный обьект будет
	// выглядеть сглаженным, т.е. более реалистичным.

	// Проходим через все обьекты для вычисления их вершин
	for (int index = 0; index < pModel->numOfObjects; index++)
	{
		// Получим текущий обьект
		t3DObject *pObject = &(pModel->pObject[index]);

		// Выделяем память под нужные переменные
		CVector3 *pNormals = new CVector3[pObject->numOfFaces];
		CVector3 *pTempNormals = new CVector3[pObject->numOfFaces];
		pObject->pNormals = new CVector3[pObject->numOfVerts];

		// Проходим через все полигоны обьекта
		for (int i = 0; i < pObject->numOfFaces; i++)
		{
			// Сохраняем 3 точки этого полигона, чтобы избежать большого кода
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Теперь вычислим нормали полигонов

			vVector1 = Vector(vPoly[0], vPoly[2]);  // вектор полигона (из 2х его сторон)
			vVector2 = Vector(vPoly[2], vPoly[1]);  // Второй вектор полигона

			vNormal = Cross(vVector1, vVector2);   // получаем cross product векторов
			pTempNormals[i] = vNormal;      // временно сохраняем не-нормализированную нормаль
											// для вершин
			vNormal = Normalize(vNormal);      // нормализируем cross product для нормалей полигона

			pNormals[i] = vNormal;          // Сохраняем нормаль в массив
		}

		//////////////// Теперь получаем вершинные нормали /////////////////

		CVector3 vSum = { 0.0, 0.0, 0.0 };
		CVector3 vZero = vSum;
		int shared = 0;

		for (int i = 0; i < pObject->numOfVerts; i++)   // Проходим через все вершины
		{
			for (int j = 0; j < pObject->numOfFaces; j++)   // Проходим через все треугольники
			{               // Проверяем, используется ли вершина другим полигоном
				if (pObject->pFaces[j].vertIndex[0] == i ||
					pObject->pFaces[j].vertIndex[1] == i ||
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);    // Прибавляем не-
																// нормализированную нормаль другого полигона
					shared++;       // Увеличиваем число полигонов с общими вершиными
				}
			}

			// Получаем нормаль делением на сумму общих полигонов. Делаем её отрицат.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// Нормализуем нормаль для вершины
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);

			vSum = vZero;           // Сбрасываем сумму
			shared = 0;         // И общие полигоны
		}

		// Освобождаем память временных переменных
		delete[] pTempNormals;
		delete[] pNormals;
	}
}
