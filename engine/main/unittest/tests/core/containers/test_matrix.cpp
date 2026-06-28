#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class MatrixTest : public TestSuite
{
};

TEST_SUITE(MatrixTest)

TEST_CASE(MatrixTest, MatrixInitialization)
{
	Matrix<i32, 3, 3> matrix;

	// Check that all elements are initialized to zero
	for (i32 row = 0; row < 3; ++row)
	{
		for (i32 col = 0; col < 3; ++col)
		{
			TEST_EQUAL(matrix(row, col), 0);
		}
	}
}

TEST_CASE(MatrixTest, AssignmentAndAccess)
{
	Matrix<i32, 2, 2> matrix;

	// Assign values to the matrix
	matrix(0, 0) = 1;
	matrix(0, 1) = 2;
	matrix(1, 0) = 3;
	matrix(1, 1) = 4;

	// Check that the values are correctly assigned and accessible
	TEST_EQUAL(matrix(0, 0), 1);
	TEST_EQUAL(matrix(0, 1), 2);
	TEST_EQUAL(matrix(1, 0), 3);
	TEST_EQUAL(matrix(1, 1), 4);
}

TEST_CASE(MatrixTest, MatrixAddition)
{
	Matrix<i32, 2, 1> matrixA;
	Matrix<i32, 2, 1> matrixB;

	// Assign values to matrixA
	matrixA(0, 0) = 1;
	matrixA(1, 0) = 3;

	// Assign values to matrixB
	matrixB(0, 0) = 5;
	matrixB(1, 0) = 7;

	// Perform matrix addition
	Matrix<i32, 2, 1> result = matrixA + matrixB;

	// Check the result of the addition
	TEST_EQUAL(result(0, 0), 6);
	TEST_EQUAL(result(1, 0), 10);
}

TEST_CASE(MatrixTest, MatrixSubtraction)
{
	Matrix<i32, 2, 1> matrixA;
	Matrix<i32, 2, 1> matrixB;

	// Assign values to matrixA
	matrixA(0, 0) = 5;
	matrixA(1, 0) = 7;

	// Assign values to matrixB
	matrixB(0, 0) = 2;
	matrixB(1, 0) = 3;

	// Perform matrix subtraction
	Matrix<i32, 2, 1> result = matrixA - matrixB;

	// Check the result of the subtraction
	TEST_EQUAL(result(0, 0), 3);
	TEST_EQUAL(result(1, 0), 4);
}

TEST_CASE(MatrixTest, MatrixMultiplication)
{
	Matrix<i32, 2, 2> matrixA;
	Matrix<i32, 2, 2> matrixB;

	// Assign values to matrixA
	matrixA(0, 0) = 1;
	matrixA(0, 1) = 2;
	matrixA(1, 0) = 3;
	matrixA(1, 1) = 4;
	/**
	 * [[1, 2],
	 *  [3, 4]]
	 */

	// Assign values to matrixB
	matrixB(0, 0) = 5;
	matrixB(0, 1) = 6;
	matrixB(1, 0) = 7;
	matrixB(1, 1) = 8;
	/**
	 * [[5, 6],
	 *  [7, 8]]
	 */

	// Perform matrix multiplication
	Matrix<i32, 2, 2> result = matrixA * matrixB;

	// Check the result of the multiplication
	TEST_EQUAL(result(0, 0), 5);  // (1*5)
	TEST_EQUAL(result(0, 1), 12); // (2*6)
	TEST_EQUAL(result(1, 0), 21); // (3*7)
	TEST_EQUAL(result(1, 1), 32); // (4*8)
}

TEST_CASE(MatrixTest, EqualAdd)
{
	Matrix<i32, 2, 1> matrixA;
	Matrix<i32, 2, 1> matrixB;

	// Assign values to matrixA
	matrixA(0, 0) = 1;
	matrixA(1, 0) = 3;

	// Assign values to matrixB
	matrixB(0, 0) = 5;
	matrixB(1, 0) = 7;

	// Perform equal addition
	matrixA += matrixB;

	// Check the result of the equal addition
	TEST_EQUAL(matrixA(0, 0), 6);
	TEST_EQUAL(matrixA(1, 0), 10);
}

TEST_CASE(MatrixTest, Transpose)
{
	Matrix<i32, 2, 3> matrix;

	// Assign values to the matrix
	matrix(0, 0) = 1;
	matrix(0, 1) = 2;
	matrix(0, 2) = 3;
	matrix(1, 0) = 4;
	matrix(1, 1) = 5;
	matrix(1, 2) = 6;
	/**
	 * [[1, 2, 3],
	 *  [4, 5, 6]]
	 */

	// Perform transpose
	Matrix<i32, 3, 2> transposed = matrix.Transpose();
	// Check the result of the transpose
	TEST_EQUAL(transposed(0, 0), 1);
	TEST_EQUAL(transposed(0, 1), 4);
	TEST_EQUAL(transposed(1, 0), 2);
	TEST_EQUAL(transposed(1, 1), 5);
	TEST_EQUAL(transposed(2, 0), 3);
	TEST_EQUAL(transposed(2, 1), 6);
	/**
	 * [[1, 4],
	 *  [2, 5],
	 *  [3, 6]]
	 */
}

TEST_CASE(MatrixTest, DotProduct)
{
	Matrix<i32, 2, 3> matrixA;
	Matrix<i32, 3, 2> matrixB;

	// Assign values to matrixA
	matrixA(0, 0) = 1;
	matrixA(0, 1) = 2;
	matrixA(0, 2) = 3;
	matrixA(1, 0) = 4;
	matrixA(1, 1) = 5;
	matrixA(1, 2) = 6;
	/**
	 * [[1, 2, 3],
	 *  [4, 5, 6]]
	 */

	// Assign values to matrixB
	matrixB(0, 0) = 7;
	matrixB(0, 1) = 8;
	matrixB(1, 0) = 9;
	matrixB(1, 1) = 10;
	matrixB(2, 0) = 11;
	matrixB(2, 1) = 12;
	/**
	 * [[7, 8],
	 *  [9,10],
	 * [11,12]]
	 */

	// Perform dot product
	Matrix<i32, 2, 2> result = matrixA.dot(matrixB);

	// Check the result of the dot product
	TEST_EQUAL(result(0, 0), (1 * 7 + 2 * 9 + 3 * 11));	 // Row1 of A dot Col1 of B
	TEST_EQUAL(result(0, 1), (1 * 8 + 2 * 10 + 3 * 12)); // Row1 of A dot Col2 of B
	TEST_EQUAL(result(1, 0), (4 * 7 + 5 * 9 + 6 * 11));	 // Row2 of A dot Col1 of B
	TEST_EQUAL(result(1, 1), (4 * 8 + 5 * 10 + 6 * 12)); // Row2 of A dot Col2 of B

	/**
	 * [[58, 64],
	 *  [139, 154]]
	 */
}

TEST_CASE(MatrixTest, AccessVector)
{
	Vec3i vector;
	vector[0] = 1;
	vector[1] = 2;
	vector[2] = 3;

	TEST_EQUAL(vector[0], 1);
	TEST_EQUAL(vector[1], 2);
	TEST_EQUAL(vector[2], 3);
}

TEST_CASE(MatrixTest, AccessScalar)
{
	Vec3i vector;
	vector[0] = 1;
	vector[1] = 2;
	vector[2] = 3;

	auto scalar = vector.Transpose().dot(vector);

	TEST_EQUAL((i32)scalar[0], 14); // 1*1 + 2*2 + 3*3 = 14
}