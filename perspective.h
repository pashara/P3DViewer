#pragma once

#include "libs.h"
#include "PCMatrix.h"
#include "zbuffer.h"
#include <fstream>
class perspective {
public:
	vector<Triangle> triangles;
	vector<PointTo3D> dots;
	vector<PointTo3D> normals;
	ZBuffer* buffer;
	int param;
	pCore::matrix<> *MAT_1;

	PointTo3D* _Original1Dot = NULL;
	PointTo3D* _Original2Dot = NULL;
	PointTo3D* _Original3Dot = NULL;


	int SCREEN_WIDTH = 800;
	int SCREEN_HEIGHT = 600;

	virtual void init(PointTo3D *a, PointTo3D*b, PointTo3D*c) = 0;
	bool findNormals(PointTo3D p0, PointTo3D p1, PointTo3D p2) {
		float A[3], B[3];
		A[0] = p1.x - p0.x;
		B[0] = p2.x - p0.x;
		A[1] = p1.y - p0.y;
		B[1] = p2.y - p0.y;
		A[2] = p1.z - p0.z;
		B[2] = p2.z - p0.z;
		float normalV[3];
		normalV[0] = A[1] * B[2] - A[2] * B[1];
		normalV[1] = A[2] * B[0] - A[0] * B[2];
		normalV[2] = A[0] * B[1] - A[1] * B[0];
		//return true;
		if ((p0.x - SCREEN_WIDTH / 2) * normalV[0] + (p0.y - SCREEN_HEIGHT / 2) * normalV[1] + p0.z * normalV[2] <= 0) {
			return true;
		}
		return false;
		PointTo3D t(A[1] * B[2] - A[2] * B[1], A[2] * B[0] - A[0] * B[2], A[0] * B[1] - A[1] * B[0]);
		normals.insert(normals.end(), t);

	}

	/*Пробегаемся по всем точкам и преобразовываем их в нужную перспективу*/
	void calcAllDots() {
		
		for (int i = 0; i < this->dots.size(); i++) {
			float X, Y, Z;

			X = MAT_1->data[0][0] * this->dots[i].x + MAT_1->data[0][1] * this->dots[i].y + MAT_1->data[0][2] * this->dots[i].z + MAT_1->data[0][3];
			Y = MAT_1->data[1][0] * this->dots[i].x + MAT_1->data[1][1] * this->dots[i].y + MAT_1->data[1][2] * this->dots[i].z + MAT_1->data[1][3];
			Z = MAT_1->data[2][0] * this->dots[i].x + MAT_1->data[2][1] * this->dots[i].y + MAT_1->data[2][2] * this->dots[i].z + MAT_1->data[2][3];
			this->dots[i].x = X;
			this->dots[i].y = Y;
			this->dots[i].z = Z;
			this->inPerspective(&(this->dots[i]));
		}
		return;
	}



	virtual void setWorkMatrix(pCore::matrix<> *MAT_1) {
		this->MAT_1 = MAT_1;
	}


	virtual void inPerspective(PointTo3D*) = 0;


	/*Предназначен для расчёта аффильных преобразований*/
	virtual void calc(PointTo3D* point) {
		pCore::matrix<>* COORDS, *WORK;
		COORDS = new pCore::matrix<>(4, 1, false);
		WORK = new pCore::matrix<>(*MAT_1);
		COORDS->setData(0, 0, point->x);
		COORDS->setData(1, 0, point->y);
		COORDS->setData(2, 0, point->z);
		COORDS->setData(3, 0, 1);
		*WORK *= *COORDS;
		point->x = WORK->getData(0, 0);
		point->y = WORK->getData(1, 0);
		point->z = WORK->getData(2, 0);

		delete COORDS;
		delete WORK;
	}

	virtual ~perspective() {
	}
};