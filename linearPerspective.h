#pragma once
#include "zbuffer.h"
#include "3dloader.h"
#include "perspective.h"

class linearPerspective : virtual public perspective {
public:
	t3DModel *g_3DModel;
	int LastTriangle = 0;
	int LastDot = 0;


	void init(PointTo3D *a, PointTo3D*b, PointTo3D*c) {
		_Original1Dot = a;
		_Original2Dot = b;
		_Original3Dot = c;

		t3DObject *obj;
		LastTriangle = 0;
		vector<int> LastDot;
		LastDot.reserve(g_3DModel->numOfObjects);
		int prevVal = 0;

		for (int j = 0; j < g_3DModel->numOfObjects; j++) {
			obj = &(g_3DModel->pObject[j]);
			for (int i = 0; i < obj->numOfVerts; i++) {
				PointTo3D dot(obj->pVerts[i].x, obj->pVerts[i].y, obj->pVerts[i].z);
				this->dots.insert(this->dots.end(), dot);
				//cout << obj->pVerts[i].x << " " << obj->pVerts[i].y << " " << obj->pVerts[i].z << endl;
			}

			LastDot.insert(LastDot.end(), prevVal);
			prevVal+= obj->numOfVerts;
		}

		calcAllDots();

		for (int j = 0; j < g_3DModel->numOfObjects; j++) {
			obj = &(g_3DModel->pObject[j]);
			bool t;
			for (int i = 0; i < obj->numOfFaces; i++) {
				
				Triangle t1(dots[obj->pFaces[i].vertIndex[0] + LastDot[j]], dots[obj->pFaces[i].vertIndex[1] + LastDot[j]], dots[obj->pFaces[i].vertIndex[2] + LastDot[j]], 100 + 2 * i);

				t = true;
				//t = findNormals(dots[obj->pFaces[i].vertIndex[0] + LastDot], dots[obj->pFaces[i].vertIndex[1] + LastDot], dots[obj->pFaces[i].vertIndex[2] + LastDot]);
				if (t)
				{
					buffer->PutTriangle(t1, 1);
					triangles.insert(this->triangles.end(), t1);
				}
			}
		}


	}


	linearPerspective(t3DModel *g_3DModel, ZBuffer * zb, int param) {
		//this->triangles.reserve(obj->numOfFaces);
		this->param = param;
		//this->dots.reserve(0);



		this->buffer = zb;
		this->g_3DModel = g_3DModel;
	}



	virtual void inPerspective(PointTo3D* point) {
		double kof;
		kof = point->z = point->z;
		point->x = point->x*(param / kof) + SCREEN_WIDTH / 2;
		point->y = point->y*(param / kof) + SCREEN_HEIGHT / 2;
		point->z = point->z;
	}

	~linearPerspective() {
		/*А ничего удалять не надо! Работали только с указателями!!*/
	}
};