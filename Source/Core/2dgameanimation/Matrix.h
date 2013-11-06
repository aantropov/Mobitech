/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** Класс Матрицы в двумерном пространстве, 
** обеспечивает все манипуляции с матрицами:
** умножение матрицы на матрицу, на точку,
** операции сдвига, вращения и масштабирование.
**
**
** Класс абсолютно кроссплатформенен и зависит только от языка программирования.
**
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>

struct Matrix {
private:
	float _matrix[3][3];

	//
	// Внутреннее умножение матрицы на матрицу 
	// используется почти во всех внутренних вычислениях
	//
	void Mul(const float m[3][3]) {
		float tmp[3][3];
		float val;
		memcpy(tmp, _matrix, sizeof(_matrix));
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				val = 0;
				for (int v = 0; v < 3; v++) {
					val += m[i][v] * tmp[v][j];
				}
				_matrix[i][j] = val;
			}
		}
	}

public:
	//
	// Установка матрицы в единичную 
	// (по главное диагонали 1 - остальное 0)
	//
	void Unit() {
		_matrix[0][0] = 1.f; _matrix[0][1] = 0.f; _matrix[0][2] = 0.f;
		_matrix[1][0] = 0.f; _matrix[1][1] = 1.f; _matrix[1][2] = 0.f;
		_matrix[2][0] = 0.f; _matrix[2][1] = 0.f; _matrix[2][2] = 1.f;
	}

	//
	// Конструктор
	//
	Matrix() {
		Unit();
	}

	//
	// Накапливаемая транформация - поворот матрицы на угол в радианах
	// (т.к. sin & cos на входе принимают угол в радианах - 
	// при портировании на другой движек это надо учитывать
	// в файлах анимаций все углы храняться в радианах)
	//
	void Rotate(float angle) {
		float m[3][3];
		m[0][0] =  cos(angle); m[0][1] = sin(angle); m[0][2] = 0.f;
		m[1][0] = -sin(angle); m[1][1] = cos(angle); m[1][2] = 0.f;
		m[2][0] =         0.f; m[2][1] =        0.f; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// Накапливаемая транформация - масштабирование матрицы
	//
	void Scale(float scx, float scy) {
		float m[3][3];
		m[0][0] = scx; m[0][1] = 0.f; m[0][2] = 0.f;
		m[1][0] = 0.f; m[1][1] = scy; m[1][2] = 0.f;
		m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// Накапливаемая транформация - сдвиг матрицы
	//
	void Move(float x, float y) {
		float m[3][3];
		m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f;
		m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f;
		m[2][0] =   x; m[2][1] =   y; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// Накапливаемая транформация - умножение на произвольную матрицу
	//
	void Mul(const Matrix &transform) {
		Mul(transform._matrix);
	}

	//
	// Умножение точки на матрицу - результат пишется в (x, y)
	//
	inline void Mul(float sx, float sy, float &x, float &y) const {
		x = _matrix[0][0] * sx + _matrix[1][0] * sy + _matrix[2][0];
		y = _matrix[0][1] * sx + _matrix[1][1] * sy + _matrix[2][1];
	}

	//
	// Вычисление детерминанта матрицы 
	// испольуется при нахождении обратной матрицы
	//
	float Determinant() const {
		return _matrix[0][0]*_matrix[1][1]*_matrix[2][2] 
					- _matrix[0][0]*_matrix[1][2]*_matrix[2][1] 
					- _matrix[0][1]*_matrix[1][0]*_matrix[2][2] 
					+ _matrix[0][1]*_matrix[1][2]*_matrix[2][0] 
					+ _matrix[0][2]*_matrix[1][0]*_matrix[2][1] 
					- _matrix[0][2]*_matrix[1][1]*_matrix[2][0];
	}

	//
	// Транспонирование матрицы 
	// испольуется при нахождении обратной матрицы
	//
	void Flip() {
		float t;
		
		t = _matrix[1][0];
		_matrix[1][0] = _matrix[0][1];
		_matrix[0][1] = t;

		t = _matrix[1][2];
		_matrix[1][2] = _matrix[2][1];
		_matrix[2][1] = t;

		t = _matrix[2][0];
		_matrix[2][0] = _matrix[0][2];
		_matrix[0][2] = t;
	}

	//
	// Умножение всех элементов матрицы на число
	// испольуется при нахождении обратной матрицы
	//
	void Mul(float f) {
		_matrix[0][0] *= f;
		_matrix[1][0] *= f;
		_matrix[2][0] *= f;

		_matrix[0][1] *= f;
		_matrix[1][1] *= f;
		_matrix[2][1] *= f;

		_matrix[0][2] *= f;
		_matrix[1][2] *= f;
		_matrix[2][2] *= f;
	}

	//
	// Построение матрицы обратной к transform
	// нельзя передевать матрицу саму себе как параметр этого вызова!
	// вычисления будут некорректны
	//
	void MakeRevers(const Matrix &transform) {
		float det = transform.Determinant();

		_matrix[0][0] = (transform._matrix[1][1] * transform._matrix[2][2] - transform._matrix[2][1] * transform._matrix[1][2]);
		_matrix[1][0] = -(transform._matrix[0][1] * transform._matrix[2][2] - transform._matrix[2][1] * transform._matrix[0][2]);
		_matrix[2][0] = (transform._matrix[0][1] * transform._matrix[1][2] - transform._matrix[1][1] * transform._matrix[0][2]);

		_matrix[0][1] = -(transform._matrix[1][0] * transform._matrix[2][2] - transform._matrix[2][0] * transform._matrix[1][2]);
		_matrix[1][1] = (transform._matrix[0][0] * transform._matrix[2][2] - transform._matrix[2][0] * transform._matrix[0][2]);
		_matrix[2][1] = -(transform._matrix[0][0] * transform._matrix[1][2] - transform._matrix[1][0] * transform._matrix[0][2]);

		_matrix[0][2] = (transform._matrix[1][0] * transform._matrix[2][1] - transform._matrix[2][0] * transform._matrix[1][1]);
		_matrix[1][2] = -(transform._matrix[0][0] * transform._matrix[2][1] - transform._matrix[2][0] * transform._matrix[0][1]);
		_matrix[2][2] = (transform._matrix[0][0] * transform._matrix[1][1] - transform._matrix[1][0] * transform._matrix[0][1]);

		Flip();

		Mul(1.f / det);
	}
};

#endif//MATRIX_H