#pragma once

#include "libs.h"
#include "PCMatrix.h"
#include "zbuffer.h"

class perspective {

	int Descr = 1;


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
		double A[3], B[3];
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

			//calc(&this->dots[i]);

			float X, Y, Z;

			X = MAT_1->data[0][0] * this->dots[i].x + MAT_1->data[0][1] * this->dots[i].y + MAT_1->data[0][2] * this->dots[i].z + MAT_1->data[0][3];
			Y = MAT_1->data[1][0] * this->dots[i].x + MAT_1->data[1][1] * this->dots[i].y + MAT_1->data[1][2] * this->dots[i].z + MAT_1->data[1][3];
			Z = MAT_1->data[2][0] * this->dots[i].x + MAT_1->data[2][1] * this->dots[i].y + MAT_1->data[2][2] * this->dots[i].z + MAT_1->data[2][3];
			this->dots[i].x = X;
			this->dots[i].y = Y;
			this->dots[i].z = Z;
			///doos[i] = this->dots[i];
			//doos->at(i) = this->dots[i];
			this->inPerspective(&(this->dots[i]));
		}

		return;


		double BADD[3][3];
		BADD[0][0] = this->dots.at(0).x - this->dots.at(1).x;
		BADD[1][0] = this->dots.at(1).x - this->dots.at(2).x;
		BADD[2][0] = this->dots.at(2).x - this->dots.at(0).x;

		BADD[0][1] = this->dots.at(0).y - this->dots.at(1).y;
		BADD[1][1] = this->dots.at(1).y - this->dots.at(2).y;
		BADD[2][1] = this->dots.at(2).y - this->dots.at(0).y;

		BADD[0][2] = this->dots.at(0).z - this->dots.at(1).z;
		BADD[1][2] = this->dots.at(1).z - this->dots.at(2).z;
		BADD[2][2] = this->dots.at(2).z - this->dots.at(0).z;


		/*_Original1Dot = new PointTo3D(BADD[0][0], BADD[0][1], BADD[0][2]);
		_Original2Dot = new PointTo3D(BADD[1][0], BADD[1][1], BADD[1][2]);
		_Original3Dot = new PointTo3D(BADD[2][0], BADD[2][1], BADD[2][2]);*/
		/*cout << "++::" << _Original1Dot->x << "::" << _Original2Dot->x << "::" << _Original3Dot->x << "::" << endl;
		float ugol;




		ugol = (_Original1Dot->x*BADD[0][0] + _Original1Dot->y*BADD[0][1]) /
		(sqrt(
		(_Original1Dot->x*_Original1Dot->x + _Original1Dot->y*_Original1Dot->y) *
		(BADD[0][0] * BADD[0][0] + BADD[0][1] * BADD[0][1])
		));
		cout << "OS` Z:" << acos(ugol) * 180 / 3.141592 << " +:: " << 180 - acos(ugol) * 180 / 3.141592 << endl;



		PointTo3D* testDOT = _Original1Dot;
		for (int ssd = 0; ssd < 3; ssd++) {
		//int ssd = 1;
		float ugol = (testDOT->x*BADD[ssd][0] + testDOT->y*BADD[ssd][1]) /
		(sqrt(
		(testDOT->x*testDOT->x + testDOT->y*testDOT->y) *
		(BADD[ssd][0] * BADD[ssd][0] + BADD[ssd][1] * BADD[ssd][1])
		));
		cout << acos(ugol) * 180 / 3.141592 << " +:: " << 180 - acos(ugol) * 180 / 3.141592 << endl;
		}
		testDOT = _Original2Dot;
		for (int ssd = 0; ssd < 3; ssd++) {
		//int ssd = 1;
		float ugol = (testDOT->x*BADD[ssd][0] + testDOT->y*BADD[ssd][1]) /
		(sqrt(
		(testDOT->x*testDOT->x + testDOT->y*testDOT->y) *
		(BADD[ssd][0] * BADD[ssd][0] + BADD[ssd][1] * BADD[ssd][1])
		));
		cout << acos(ugol) * 180 / 3.141592 << " :: " << 180 - acos(ugol) * 180 / 3.141592 << endl;
		}
		testDOT = _Original3Dot;
		for (int ssd = 0; ssd < 3; ssd++) {
		//int ssd = 1;
		float ugol = (testDOT->x*BADD[ssd][0] + testDOT->y*BADD[ssd][1]) /
		(sqrt(
		(testDOT->x*testDOT->x + testDOT->y*testDOT->y) *
		(BADD[ssd][0] * BADD[ssd][0] + BADD[ssd][1] * BADD[ssd][1])
		));
		cout << acos(ugol) * 180 / 3.141592 << " :: " << 180 - acos(ugol) * 180 / 3.141592 << endl;
		}



		for (int i = 0; i < this->dots.size(); i++) {
		this->inPerspective(&(this->dots[i]));
		}*/
	}
	virtual void setWorkMatrix(pCore::matrix<> *MAT_1) {
		this->MAT_1 = MAT_1;
	}


	virtual void inPerspective(PointTo3D*) = 0;


	void setDescr(int d) {
		if (d >= 1 && d <= 50) {
			Descr = d;
		}
	}

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








/*

НОРМАЛИ
double A[3], B[3];
A[0] = X[1] - X[0];
B[0] = X[2] - X[0];
A[1] = Y[1] - Y[0];
B[1] = Y[2] - Y[0];
A[2] = Z[1] - Z[0];
B[2] = Z[2] - Z[0];
//				}
double normalV[3];
normalV[0] = A[1] * B[2] - A[2] * B[1];
normalV[1] = A[2] * B[0] - A[0] * B[2];
normalV[2] = A[0] * B[1] - A[1] * B[0];

PointTo3D p[3];
p[0].init(DotNET1[0].x + SCREEN_WIDTH / 2 + 1, DotNET1[0].y + SCREEN_HEIGHT / 2, DotNET1[0].z);
p[1].init(DotNET1[1].x + SCREEN_WIDTH / 2 + 1, DotNET1[1].y + SCREEN_HEIGHT / 2, DotNET1[1].z);
p[2].init(DotNET1[2].x + SCREEN_WIDTH / 2 + 1, DotNET1[2].y + SCREEN_HEIGHT / 2, DotNET1[2].z);


*/













//
//
//
//
//double X[3], Y[3], Z[3];
//int RX[3], RY[3], RZ[3];
//float step[3][3];
//
//for (int j = 0; j < pObject->numOfFaces; j++) {
//	int index[3] = { pObject->pFaces[j].vertIndex[0],pObject->pFaces[j].vertIndex[1],pObject->pFaces[j].vertIndex[2] };
//	PointTo3D Center(
//		(pObject->pVerts[index[0]].x + lambda*((pObject->pVerts[index[1]].x + pObject->pVerts[index[2]].x) / 2)) / (lambda + 1),
//		(pObject->pVerts[index[0]].y + lambda*((pObject->pVerts[index[1]].y + pObject->pVerts[index[2]].y) / 2)) / (lambda + 1),
//		(pObject->pVerts[index[0]].z + lambda*((pObject->pVerts[index[1]].z + pObject->pVerts[index[2]].z) / 2)) / (lambda + 1)
//	);
//
//
//
//
//	/*
//	COORDS = new pCore::matrix<>(4, 1, false);
//	WORK = new pCore::matrix<>(*MAT_1);
//	COORDS->setData(0, 0, Center.x);
//	COORDS->setData(1, 0, Center.y);
//	COORDS->setData(2, 0, Center.z);
//	COORDS->setData(3, 0, 1);
//
//	*WORK *= *COORDS;
//	Center.x = WORK->getData(0, 0);
//	Center.y = WORK->getData(1, 0);
//	Center.z = WORK->getData(2, 0);
//
//
//	double kof1 = sqrt(Center.x * Center.x + Center.y * Center.y + Center.z * Center.z);
//	if (perspective1 == 1) {
//	Center.x = Center.x * radius / kof1;
//	Center.y = Center.y * radius / kof1;
//	}
//	else {
//	Center.x = Center.x * radius / Center.z;
//	Center.y = Center.y * radius / Center.z;
//	}
//
//	delete COORDS;
//	delete WORK;
//
//	*/
//
//
//
//
//
//}