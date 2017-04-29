#pragma once
/*
Класс отвечает за афинные преобразования, а именно за генерацию и рассчёт переходных матриц
*/
#include "includes.h"
#include "PCMatrix.h"
namespace pCore {
	class AffineTransformations {
		float pi = 3.14159265358979323846;
	public:
		matrix<float>*T;
		matrix<float>*Rx;
		matrix<float>*Ry;
		matrix<float>*Rz;
		matrix<float>*S;
		matrix<float>*result;
		AffineTransformations() {
			T = new matrix<float>(4, 4, true);
			Rx = new matrix<float>(4, 4, true);
			Ry = new matrix<float>(4, 4, true);
			Rz = new matrix<float>(4, 4, true);
			S = new matrix<float>(4, 4, true);
			result = new matrix<float>(4, 4, true);
		}

		void setT(float x, float y, float z) {
			T->setData(0, 3, x);
			T->setData(1, 3, y);
			T->setData(2, 3, z);
		}

		void setS(float x, float y, float z) {
			S->setData(0, 0, x);
			S->setData(1, 1, y);
			S->setData(2, 2, z);
		}
		/*Х и У местами поменять*/

		void setRy(float ugol) {
			Ry->setData(0, 0, cos(-ugol*pi / 180));
			Ry->setData(0, 2, -sin(-ugol*pi / 180));
			Ry->setData(2, 0, sin(-ugol*pi / 180));
			Ry->setData(2, 2, cos(-ugol*pi / 180));
			/*
			C	*	-S	*
			*	*	*	*
			S	*	C	*
			*	*	*	*
			*/
		}

		void setRx(float ugol) {
			Rx->setData(0, 0, cos(-ugol*pi / 180));
			Rx->setData(0, 1, -sin(-ugol*pi / 180));
			Rx->setData(1, 0, sin(-ugol*pi / 180));
			Rx->setData(1, 1, cos(-ugol*pi / 180));
			/*
			C	-S	*	*
			S	C	*	*
			*	*	*	*
			*	*	*	*
			*/
		}

		void setRz(float ugol) {
			Rz->setData(1, 1, cos(-ugol*pi / 180));
			Rz->setData(1, 2, -sin(-ugol*pi / 180));
			Rz->setData(2, 1, sin(-ugol*pi / 180));
			Rz->setData(2, 2, cos(-ugol*pi / 180));
			/*
			*	*	*	*
			*	C	-S	*
			*	S	C	*
			*	*	*	*
			*/
		}

		void initCoordsMatrix(matrix<float> &COORDS, float x, float y, float z) {
			COORDS.setData(0, 0, x);
			COORDS.setData(1, 0, y);
			COORDS.setData(2, 0, z);
			COORDS.setData(3, 0, 1);
		}

		void getResult(matrix<float> &a) {
			*result *= *S;
			*result *= *T;
			*result *= *Rx;
			*result *= *Ry;
			*result *= *Rz;
			for (int i = 0; i < result->getA();i++) {
				for (int j = 0; j < result->getB();j++) {
					a.setData(i, j, result->getData(i, j));
				}
			}
			result->DeOnce();
		}
		~AffineTransformations() {
			delete T;
			delete result;
			delete S;
			delete Rx;
			delete Ry;
			delete Rz;
		}
	};
}