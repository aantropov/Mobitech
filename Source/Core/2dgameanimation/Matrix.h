/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** ����� ������� � ��������� ������������, 
** ������������ ��� ����������� � ���������:
** ��������� ������� �� �������, �� �����,
** �������� ������, �������� � ���������������.
**
**
** ����� ��������� ����������������� � ������� ������ �� ����� ����������������.
**
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>

struct Matrix {
private:
	float _matrix[3][3];

	//
	// ���������� ��������� ������� �� ������� 
	// ������������ ����� �� ���� ���������� �����������
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
	// ��������� ������� � ��������� 
	// (�� ������� ��������� 1 - ��������� 0)
	//
	void Unit() {
		_matrix[0][0] = 1.f; _matrix[0][1] = 0.f; _matrix[0][2] = 0.f;
		_matrix[1][0] = 0.f; _matrix[1][1] = 1.f; _matrix[1][2] = 0.f;
		_matrix[2][0] = 0.f; _matrix[2][1] = 0.f; _matrix[2][2] = 1.f;
	}

	//
	// �����������
	//
	Matrix() {
		Unit();
	}

	//
	// ������������� ������������ - ������� ������� �� ���� � ��������
	// (�.�. sin & cos �� ����� ��������� ���� � �������� - 
	// ��� ������������ �� ������ ������ ��� ���� ���������
	// � ������ �������� ��� ���� ��������� � ��������)
	//
	void Rotate(float angle) {
		float m[3][3];
		m[0][0] =  cos(angle); m[0][1] = sin(angle); m[0][2] = 0.f;
		m[1][0] = -sin(angle); m[1][1] = cos(angle); m[1][2] = 0.f;
		m[2][0] =         0.f; m[2][1] =        0.f; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// ������������� ������������ - ��������������� �������
	//
	void Scale(float scx, float scy) {
		float m[3][3];
		m[0][0] = scx; m[0][1] = 0.f; m[0][2] = 0.f;
		m[1][0] = 0.f; m[1][1] = scy; m[1][2] = 0.f;
		m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// ������������� ������������ - ����� �������
	//
	void Move(float x, float y) {
		float m[3][3];
		m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f;
		m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f;
		m[2][0] =   x; m[2][1] =   y; m[2][2] = 1.f;
		Mul(m);
	}

	//
	// ������������� ������������ - ��������� �� ������������ �������
	//
	void Mul(const Matrix &transform) {
		Mul(transform._matrix);
	}

	//
	// ��������� ����� �� ������� - ��������� ������� � (x, y)
	//
	inline void Mul(float sx, float sy, float &x, float &y) const {
		x = _matrix[0][0] * sx + _matrix[1][0] * sy + _matrix[2][0];
		y = _matrix[0][1] * sx + _matrix[1][1] * sy + _matrix[2][1];
	}

	//
	// ���������� ������������ ������� 
	// ����������� ��� ���������� �������� �������
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
	// ���������������� ������� 
	// ����������� ��� ���������� �������� �������
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
	// ��������� ���� ��������� ������� �� �����
	// ����������� ��� ���������� �������� �������
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
	// ���������� ������� �������� � transform
	// ������ ���������� ������� ���� ���� ��� �������� ����� ������!
	// ���������� ����� �����������
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