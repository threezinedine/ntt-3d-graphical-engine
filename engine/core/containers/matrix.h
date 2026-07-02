#pragma once

#include "common.h"
#include "utils/utils.h"

namespace ntt {

template <typename T, int Rows, int Cols>
struct Matrix
{
	T& operator()(i32 row, i32 col)
	{
		return m_Data[row][col];
	}

	const T& operator()(i32 row, i32 col) const
	{
		return m_Data[row][col];
	}

	T& operator[](i32 index)
	{
		return m_Data[index][0];
	}

	const T& operator[](i32 index) const
	{
		return m_Data[index][0];
	}

	operator T() const
	{
		return m_Data[0][0];
	}

	Matrix<T, Rows, Cols> operator+(const Matrix<T, Rows, Cols>& other) const;
	Matrix<T, Rows, Cols> operator-(const Matrix<T, Rows, Cols>& other) const;
	Matrix<T, Rows, Cols> operator*(const Matrix<T, Rows, Cols>& other) const;
	Matrix<T, Rows, Cols> operator/(const Matrix<T, Rows, Cols>& other) const;

	Matrix<T, Rows, Cols>& operator+=(const Matrix<T, Rows, Cols>& other);
	Matrix<T, Rows, Cols>& operator-=(const Matrix<T, Rows, Cols>& other);
	Matrix<T, Rows, Cols>& operator*=(const Matrix<T, Rows, Cols>& other);
	Matrix<T, Rows, Cols>& operator/=(const Matrix<T, Rows, Cols>& other);

	Matrix<T, Cols, Rows> Transpose() const;

	template <int OtherCols>
	Matrix<T, Rows, OtherCols> dot(const Matrix<T, Cols, OtherCols>& other) const;

	T m_Data[Rows][Cols] = {};
};

#define NTT_PRIMITIVE_MATRIX_OP(op)                                                                                    \
	template <typename T, int Rows, int Cols>                                                                          \
	Matrix<T, Rows, Cols> Matrix<T, Rows, Cols>::operator op(const Matrix<T, Rows, Cols>& other) const                 \
	{                                                                                                                  \
		Matrix<T, Rows, Cols> result;                                                                                  \
		for (i32 row = 0; row < Rows; ++row)                                                                           \
		{                                                                                                              \
			for (i32 col = 0; col < Cols; ++col)                                                                       \
			{                                                                                                          \
				result(row, col) = m_Data[row][col] op other.m_Data[row][col];                                         \
			}                                                                                                          \
		}                                                                                                              \
		return result;                                                                                                 \
	}

NTT_PRIMITIVE_MATRIX_OP(+)
NTT_PRIMITIVE_MATRIX_OP(-)
NTT_PRIMITIVE_MATRIX_OP(*)
NTT_PRIMITIVE_MATRIX_OP(/)

#define NTT_SELF_PRIMITIVE_MATRIX_OP(op)                                                                               \
	template <typename T, int Rows, int Cols>                                                                          \
	Matrix<T, Rows, Cols>& Matrix<T, Rows, Cols>::operator op##=(const Matrix<T, Rows, Cols>& other)                   \
	{                                                                                                                  \
		for (i32 row = 0; row < Rows; ++row)                                                                           \
		{                                                                                                              \
			for (i32 col = 0; col < Cols; ++col)                                                                       \
			{                                                                                                          \
				m_Data[row][col] op## = other.m_Data[row][col];                                                        \
			}                                                                                                          \
		}                                                                                                              \
		return *this;                                                                                                  \
	}

NTT_SELF_PRIMITIVE_MATRIX_OP(+)
NTT_SELF_PRIMITIVE_MATRIX_OP(-)
NTT_SELF_PRIMITIVE_MATRIX_OP(*)
NTT_SELF_PRIMITIVE_MATRIX_OP(/)

template <typename T, int Rows, int Cols>
Matrix<T, Cols, Rows> Matrix<T, Rows, Cols>::Transpose() const
{
	Matrix<T, Cols, Rows> result;
	for (i32 row = 0; row < Rows; ++row)
	{
		for (i32 col = 0; col < Cols; ++col)
		{
			result(col, row) = m_Data[row][col];
		}
	}
	return result;
}

template <typename T, int Rows, int Cols>
template <int OtherCols>
Matrix<T, Rows, OtherCols> Matrix<T, Rows, Cols>::dot(const Matrix<T, Cols, OtherCols>& other) const
{
	Matrix<T, Rows, OtherCols> result;
	for (i32 i = 0; i < Rows; ++i)
	{
		for (i32 j = 0; j < OtherCols; ++j)
		{
			T sum = 0;
			for (i32 k = 0; k < Cols; ++k)
			{
				sum += m_Data[i][k] * other(k, j);
			}
			result(i, j) = sum;
		}
	}
	return result;
}

typedef Matrix<i32, 2, 2> Mat2x2i;
typedef Matrix<i32, 3, 3> Mat3x3i;
typedef Matrix<i32, 4, 4> Mat4x4i;

typedef Matrix<f32, 2, 2> Mat2x2f;
typedef Matrix<f32, 3, 3> Mat3x3f;
typedef Matrix<f32, 4, 4> Mat4x4f;

typedef Matrix<i32, 2, 1> Vec2i;
typedef Matrix<i32, 3, 1> Vec3i;
typedef Matrix<i32, 4, 1> Vec4i;

typedef Matrix<f32, 2, 1> Vec2f;
typedef Matrix<f32, 3, 1> Vec3f;
typedef Matrix<f32, 4, 1> Vec4f;

} // namespace ntt