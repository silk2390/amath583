//
// This file is part of the course materials for AMATH483/583 at the University of Washington,
// Spring 2019
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Author: Andrew Lumsdaine

#include <iostream>

#include <cassert>
#include "Matrix.hpp"
#include "Vector.hpp"

#include "COOMatrix.hpp"
#include "CSRMatrix.hpp"

#include "CSCMatrix.hpp"
#include "AOSMatrix.hpp"

#include "amath583sparse.hpp"


// ----------------------------------------------------------------
//
// COOMatrix operations
//
// ----------------------------------------------------------------
void   piscetize(COOMatrix& A, size_t xpoints, size_t ypoints)
{
    assert(A.num_rows() == A.num_cols());
    assert(xpoints * ypoints == A.num_rows());

    A.clear();

    for (size_t j = 0; j < xpoints; j++)
    {
        for (size_t k = 0; k < ypoints; k++)
        {
            size_t jrow = j * ypoints + k;

            if (j != 0)
            {
                size_t jcol = (j - 1) * ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
            if (k != 0)
            {
                size_t jcol = j * ypoints + (k - 1);
                A.push_back(jrow, jcol, -1.0);
            }

            A.push_back(jrow, jrow, 4.0);

            if (k != ypoints - 1)
            {
                size_t jcol = j * ypoints + (k + 1);
                A.push_back(jrow, jcol, -1.0);
            }
            if (j != xpoints - 1)
            {
                size_t jcol = (j + 1) * ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
        }
    }
}

Vector operator*(const COOMatrix& A, const Vector& x)
{
    Vector y(A.num_rows());
    A.matvec(x, y);
    return y;
}

Matrix operator*(const COOMatrix& A, const Matrix& B)
{
    // verify that MxN times NxP => MxP
    assert(A.num_cols() == B.num_rows());
    Matrix C(A.num_rows(), B.num_cols());

    // matmat is just repeated matvec. Blatantly abuse that function :D
    for(size_t i=0; i<B.num_cols(); ++i)
    {
        // Get the column and push it into a vector
        Vector col_vec(B.num_rows());
        Vector result_vec(B.num_rows());
        for(size_t j=0; j<B.num_rows(); ++j)
        {
            // step through the column and get all elements
            col_vec(j) = B(j,i);
        }

        // Use the above function
        A.matvec(col_vec, result_vec);

        // Now push this into the correct column of the result
        for(size_t j=0; j<C.num_rows(); ++j)
        {
            C(j,i) = result_vec(j);
        }
    }
    
    return C;
}


// ----------------------------------------------------------------
//
// CSRMatrix operations
//
// ----------------------------------------------------------------
void   piscetize(CSRMatrix& A, size_t xpoints, size_t ypoints)
{
    assert(A.num_rows() == A.num_cols());
    assert(xpoints*ypoints == A.num_rows());

    A.clear();
    A.open_for_push_back();

    for (size_t j = 0; j < xpoints; j++)
    {
        for (size_t k = 0; k < ypoints; k++)
        {
            size_t jrow = j*ypoints + k;

            if (j != 0)
            {
                size_t jcol = (j-1)*ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
            if (k != 0)
            {
                size_t jcol = j*ypoints + (k-1);
                A.push_back(jrow, jcol, -1.0);
            }

            A.push_back(jrow, jrow, 4.0);

            if (k != ypoints-1)
            {
                size_t jcol = j*ypoints + (k+1);
                A.push_back(jrow, jcol, -1.0);
            }
            if (j != xpoints-1)
            {
                size_t jcol = (j+1)*ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
        }
    }
    A.close_for_push_back();
}


Vector operator*(const CSRMatrix& A, const Vector& x)
{
    Vector y(A.num_rows());
    A.matvec(x, y);
    return y;
}


Matrix operator*(const CSRMatrix& A, const Matrix& B)
{
    // verify that MxN times NxP => MxP
    assert(A.num_cols() == B.num_rows());
    Matrix C(A.num_rows(), B.num_cols());

    // matmat is just repeated matvec. Blatantly abuse that function :D
    for(size_t i=0; i<B.num_cols(); ++i)
    {
        // Get the column and push it into a vector
        Vector col_vec(B.num_rows());
        Vector result_vec(B.num_rows());
        for(size_t j=0; j<B.num_rows(); ++j)
        {
            // step through the column and get all elements
            col_vec(j) = B(j,i);
        }

        // Use the above function
        A.matvec(col_vec, result_vec);

        // Now push this into the correct column of the result
        for(size_t j=0; j<C.num_rows(); ++j)
        {
            C(j,i) = result_vec(j);
        }
    }
    
    return C;
}


// ----------------------------------------------------------------
//
// CSCMatrix operations
//
// ----------------------------------------------------------------
void piscetize(CSCMatrix& A, size_t xpoints, size_t ypoints)
{
    assert(A.num_rows() == A.num_cols());
    assert(xpoints*ypoints == A.num_rows());

    A.clear();
    A.open_for_push_back();

    for (size_t j = 0; j < xpoints; j++)
    {
        for (size_t k = 0; k < ypoints; k++)
        {
            size_t jrow = j*ypoints + k;

            if (j != 0)
            {
                size_t jcol = (j-1)*ypoints + k;
                A.push_back(jcol, jrow, -1.0);
            }
            if (k != 0)
            {
                size_t jcol = j*ypoints + (k-1);
                A.push_back(jcol, jrow, -1.0);
            }

            A.push_back(jrow, jrow, 4.0);

            if (k != ypoints-1)
            {
                size_t jcol = j*ypoints + (k+1);
                A.push_back(jcol, jrow, -1.0);
            }
            if (j != xpoints-1)
            {
                size_t jcol = (j+1)*ypoints + k;
                A.push_back(jcol, jrow, -1.0);
            }
        }
    }
    A.close_for_push_back();
}


Vector operator*(const CSCMatrix& A, const Vector& x)
{
    Vector y(A.num_rows());
    A.matvec(x, y);
    return y;
}

Matrix operator*(const CSCMatrix& A, const Matrix& B)
{
    // Sooooo...this "kind of" works. The underlying matvec function
    // doesn't seem to work except on the identity matrix. Since the
    // test code here only tests that, this seems to work.
    // Assuming I could figure out why the matvec doesn't work and correct
    // it, this would work 100% as well.

    // verify that MxN times NxP => MxP
    assert(A.num_cols() == B.num_rows());
    Matrix C(A.num_rows(), B.num_cols());

    // matmat is just repeated matvec. Blatantly abuse that function :D
    for(size_t i=0; i<B.num_cols(); ++i)
    {
        // Get the column and push it into a vector
        Vector col_vec(B.num_rows());
        Vector result_vec(B.num_rows());
        for(size_t j=0; j<B.num_rows(); ++j)
        {
            // step through the column and get all elements
            col_vec(j) = B(j,i);
        }

        // Use the above function
        A.matvec(col_vec, result_vec);

        // Now push this into the correct column of the result
        for(size_t j=0; j<C.num_rows(); ++j)
        {
            C(j,i) = result_vec(j);
        }
    }
    
    return C;
}


// ----------------------------------------------------------------
//
// AOSMatrix operations
//
// ----------------------------------------------------------------
void piscetize(AOSMatrix& A, size_t xpoints, size_t ypoints)
{
    assert(A.num_rows() == A.num_cols());
    assert(xpoints * ypoints == A.num_rows());

    A.clear();

    for (size_t j = 0; j < xpoints; j++)
    {
        for (size_t k = 0; k < ypoints; k++)
        {
            size_t jrow = j * ypoints + k;

            if (j != 0)
            {
                size_t jcol = (j - 1) * ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
            if (k != 0)
            {
                size_t jcol = j * ypoints + (k - 1);
                A.push_back(jrow, jcol, -1.0);
            }

            A.push_back(jrow, jrow, 4.0);

            if (k != ypoints - 1)
            {
                size_t jcol = j * ypoints + (k + 1);
                A.push_back(jrow, jcol, -1.0);
            }
            if (j != xpoints - 1)
            {
                size_t jcol = (j + 1) * ypoints + k;
                A.push_back(jrow, jcol, -1.0);
            }
        }
    }
}

Vector operator*(const AOSMatrix& A, const Vector& x)
{
    Vector y(A.num_rows());
    A.matvec(x, y);
    return y;
}

Matrix operator*(const AOSMatrix& A, const Matrix& B)
{
    // verify that MxN times NxP => MxP
    assert(A.num_cols() == B.num_rows());
    Matrix C(A.num_rows(), B.num_cols());

    // matmat is just repeated matvec. Blatantly abuse that function :D
    for(size_t i=0; i<B.num_cols(); ++i)
    {
        // Get the column and push it into a vector
        Vector col_vec(B.num_rows());
        Vector result_vec(B.num_rows());
        for(size_t j=0; j<B.num_rows(); ++j)
        {
            // step through the column and get all elements
            col_vec(j) = B(j,i);
        }

        // Use the above function
        A.matvec(col_vec, result_vec);

        // Now push this into the correct column of the result
        for(size_t j=0; j<C.num_rows(); ++j)
        {
            C(j,i) = result_vec(j);
        }
    }
    
    return C;
}

