#pragma once
#include <iostream>


class PExeptions {
public:
	class TooBigMatrixSize {
	public:
		std::string what() {
			return "Слишком большой размер матрицы";
		}
	};
	class TooLittleMatrixSize {
	public:
		std::string what() {
			return "Слишком маленький размер матрицы";
		}
	};
	class OutOfDataRange {
	public:
		std::string what() {
			return "Выход за пределы матрицы";
		}
	};
	class MatrixDoNotSq {
	public:
		std::string what() {
			return "Матрица не является квадратной";
		}
	};
	class Adjungierte {
	public:
		std::string what() {
			return "Матрицы не сопряжены";
		}
	};
};

namespace pCore {
	/*
	Работает с матрицами до 50х50
	*/
	template<class TYPE = float>
	class matrix {
		int a, b;






	public:

		TYPE** data;
		/*matrix() : a(0), b(0) {
		cout << "\nОбычный конструктор\n";
		}*/
		matrix(int a, int b, bool isOnce = false) : a(a), b(b) {
			//cout << "\nКонструктор с параметрами\n";
			if (a > 50 || b > 50)
				throw new PExeptions::TooBigMatrixSize;
			if (a <= 0 || b <= 0)
				throw new PExeptions::TooLittleMatrixSize;

			data = new TYPE*[a];
			for (int i = 0; i < a;i++) {
				data[i] = new TYPE[b];
			}
			if (isOnce == true) {
				this->DeOnce();
			}
			else {
				this->DeZero();
			}
			//cout << "\nОбычный конструктор\n";
		}
		matrix(const matrix<TYPE> &obj) {
			//cout << "\nКонструктор копирования\n";
			this->a = obj.a;
			this->b = obj.b;
			if (this->a > 50 || this->b > 50)
				throw new PExeptions::TooBigMatrixSize;
			if (this->a <= 0 || this->b <= 0)
				throw new PExeptions::TooLittleMatrixSize;
			this->data = new TYPE*[a];
			for (int i = 0; i < this->a;i++) {
				this->data[i] = new TYPE[b];
				for (int j = 0; j < this->b;j++) {
					setData(i, j,
						obj.data[i][j]);
				}
			}
		}



		void DeZero() {
			try {
				for (int i = 0; i < this->a;i++) {
					for (int j = 0; j < this->b;j++) {
						this->setData(i, j, (TYPE)0);
					}
				}
			}
			catch (PExeptions::OutOfDataRange *a) {
				cout << a->what() << endl;
			}
		}
		void DeOnce() {
			if (this->a != this->b)
				throw new PExeptions::MatrixDoNotSq;
			this->DeZero();
			for (int i = 0;i < this->a; i++) {
				this->setData(i, i, (TYPE)1);
			}
		}



		void show() {
			for (int i = 0; i < this->a;i++) {
				for (int j = 0; j < this->b;j++) {
					cout << this->getData(i, j) << "  ";
				}
				cout << endl;
			}
			cout << "========\n";
		}

		matrix<float>& operator*=(const matrix<float>& rhs)
		{
			int x1 = this->getA();
			int x2 = rhs.a;
			int y1 = this->getB();
			int y2 = rhs.b;
			matrix<TYPE>* temp = new matrix<float>(*this);
			if (y1 != x2) {
				throw new PExeptions::Adjungierte;
			}
			//Очищаю текущую матрицу
			if (b != 0)
				for (int i = 0; i < b;i++) {
					delete[] this->data[i];
				}
			if (a != 0)
				delete[] this->data;
			this->a = x1;
			this->b = y2;
			//Выделяю память под новую матрицу
			this->data = new TYPE*[this->a];
			for (int i = 0; i < this->a; i++) {
				this->data[i] = new TYPE[this->b];
			}
			TYPE B;
			for (int row = 0; row<x1;row++) {
				for (int col = 0;col<y2;col++) {
					B = 0;
					for (int inner = 0; inner < x2; inner++) {
						B += temp->data[row][inner] * rhs.data[inner][col];
					}
					this->data[row][col] = B;
				}
			}
			delete temp;
			return *this;
		}


		void multi(matrix  &bMat) {
			int x1 = this->getA();
			int x2 = bMat.getA();
			int y1 = this->getB();
			int y2 = bMat.getB();

			matrix<TYPE> *temp = new matrix<TYPE>(*this);

			if (y1 != x2) {
				return;
			}

			this->a = x1;
			this->b = y2;
			TYPE C, B;
			for (int row = 0; row<x1;row++) {
				for (int col = 0;col<y2;col++) {
					B = 0;
					for (int inner = 0; inner < x2; inner++) {
						B += temp->getData(row, inner) * bMat.getData(inner, col);
					}
					this->data[row][col] = B;
				}
			}
		}

		float getData(int a, int b) {
			if (!(a >= this->a || b >= this->b))
				return this->data[a][b];
			else
				throw new PExeptions::OutOfDataRange;
		}
		void setData(int a, int b, TYPE value) {
			if (!(a >= this->a || b >= this->b))
				this->data[a][b] = value;
			else
				throw new PExeptions::OutOfDataRange;
		}

		~matrix() {
			/*if (b == 1) {
			delete this->data[0];
			}else */
			if (a != 0)
				for (int i = 0; i < a;i++) {
					delete[] this->data[i];
				}
			if (b != 0)
				delete[] this->data;
			//	cout << "\nДестркуктор у матрицы\n";
		}

		int getA() {
			return this->a;
		}
		int getB() {
			return this->b;
		}
	};

	/*/
	class matrix {
	int a, b;
	TYPE** data;
	public:
	matrix() {
	cout << "STD:";
	}
	//a - строка, b- столбец
	matrix(int a, int b, bool IsNull = false, bool isE = true) {
	this->a = a;
	this->b = b;
	data = new TYPE*[a];
	for (int i = 0; i < a; i++) {
	data[i] = new TYPE[b];
	for (int j = 0; j < b;j++) {
	data[i][j] = i + j;
	}
	}
	if (IsNull)
	this->setNullAll();
	if (isE)
	this->setOnceMatrix();
	}
	//a - строка, b- столбец
	TYPE getData(int a, int b) {
	TYPE aa = this->data[a][b];
	return this->data[a][b];
	}
	//a - строка, b- столбец
	void setData(int a, int b, TYPE value) {
	this->data[a][b] = value;
	}

	void setNullAll() {
	for (int i = 0;i < a;i++)
	for (int j = 0;j < b;j++)
	this->setData(i,j, (TYPE)0);
	}
	void setOnceMatrix() {
	this->setNullAll();
	if (a == b) {
	for (int i = 0; i < a;i++)
	this->setData(i, i, 1);
	}
	}
	int getA() {
	return this->a;
	}
	int getB() {
	return this->b;
	}

	void multi(matrix  *bMat) {
	int x1 = this->getA();
	int x2 = bMat->getA();
	int y1 = this->getB();
	int y2 = bMat->getB();

	if (y1 != x2) {
	return;
	}

	this->a = x1;
	this->b = y2;
	TYPE C,B;
	for (int row = 0; row<x1;row++) {
	for (int col = 0;col<y2;col++) {
	B = 0;
	for (int inner = 0; inner < x2; inner++) {
	B += this->getData(row, inner) * bMat->getData(inner, col);
	}
	this->data[row][col] = B;
	}
	}
	}



	void show() {
	for (int i = 0; i < a; i++) {
	for (int j = 0; j < b; j++) {
	cout << this->getData(i, j) << "   ";
	}
	cout << endl;
	}

	cout <<"================================================="<< endl;
	}

	~matrix() {
	cout << "~matrix()";
	//cout << "BB: " << a << " " << b << endl;
	for (int i = 0; i < a; i++) {
	delete this->data[i];
	}
	delete[] this->data;
	/*for (int i = 0; i < a;i++) {
	delete[] data[i];
	}
	//delete[] data;
	}
	};
	*/
}