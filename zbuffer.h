#pragma once



#include <vector>



class PointTo3D {
public:
	float x, y, z;
	PointTo3D() {

		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	void copy(const PointTo3D &obj) {
		this->x = obj.x;
		this->y = obj.y;
		this->z = obj.z;
	}


	PointTo3D(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void init(float x, float y, float z) {

		this->x = x;
		this->y = y;
		this->z = z;
	}
	PointTo3D operator+(const PointTo3D& b) {
		PointTo3D temp;
		temp.x = this->x + b.x;
		temp.y = this->y + b.y;
		temp.z = this->z + b.z;
		return temp;
	}
	PointTo3D operator-(const PointTo3D& b) {
		PointTo3D temp;
		temp.x = this->x - b.x;
		temp.y = this->y - b.y;
		temp.z = this->z - b.z;
		return temp;
	}
};


typedef float MyTypeExample;


#define MAXDIST 10000.0 //Максимальная глубина сцены
#define MAXYLINES 10000 //Максимальное количество линий в сцене.
#define clBk 3; //Цвет по умолчанию





//Структура для точки в 3-мерном пространстве.
/*struct POINT3D {
float x, y, z;
};*/


//Класс треугольников.
class Triangle {
public:
	int color;
	PointTo3D p[3];
	Triangle() {};
	Triangle(PointTo3D p1, PointTo3D p2, PointTo3D p3, int c) {
		init(p1, p2, p3, c);
	}
	void init(PointTo3D p1, PointTo3D p2, PointTo3D p3, int c) {
		p[0] = p1; p[1] = p2; p[2] = p3;
		color = c;
	}
};

//Структура ячейки, из которых будет состоять Z-буфер.
struct CELL {
	MyTypeExample z;
	int color;
	//int numberOfTr;
	Triangle tr;
};


//Класс Z-буфера.
class ZBuffer {
public:
	CELL *buff[MAXYLINES];
	int sX, sY;	// Размер Z-Буфера
	ZBuffer(int, int);
	~ZBuffer();
	void PutTriangle(Triangle&, int);
	void Clear();
};
