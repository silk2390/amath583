//
// This file is part of the course materials for AMATH483/583 at the University of Washington,
// Spring 2018
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Author: Andrew Lumsdaine
//
#ifndef __CSRMATRIX_HPP
#define __CSRMATRIX_HPP

#include <fstream>

#include "Vector.hpp"
#include "Matrix.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <future>

class CSRMatrix {
public:
  CSRMatrix(size_t M, size_t N) : is_open(false), num_rows_(M), num_cols_(N), row_indices_(num_rows_ + 1, 0) {}

  void open_for_push_back() { is_open = true; }

  void close_for_push_back() {
    is_open = false;
    for (size_t i = 0; i < num_rows_; ++i) {
      row_indices_[i + 1] += row_indices_[i];
    }
    for (size_t i = num_rows_; i > 0; --i) {
      row_indices_[i] = row_indices_[i - 1];
    }
    row_indices_[0] = 0;
  }

  void push_back(size_t i, size_t j, double value) {
    assert(is_open);
    assert(i < num_rows_ && i >= 0);
    assert(j < num_cols_ && j >= 0);

    ++row_indices_[i];
    col_indices_.push_back(j);
    storage_.push_back(value);
  }

  void clear() {
    col_indices_.clear();
    storage_.clear();
    std::fill(row_indices_.begin(), row_indices_.end(), 0);
  }

  size_t num_rows() const { return num_rows_; }
  size_t num_cols() const { return num_cols_; }
  size_t num_nonzeros() const { return storage_.size(); }

  void stream_coordinates(std::ostream& output_file) const {
    for (size_t i = 0; i < num_rows_; ++i) {
      for (size_t j = row_indices_[i]; j < row_indices_[i+1]; ++j) {
	output_file << i               << " ";
	output_file << col_indices_[j] << " ";
	output_file <<     storage_[j]       ;
	output_file << std::endl;
      }
    }
  }

  void matvec(const Vector& x, Vector& y) const {
    for (size_t i = 0; i < num_rows_; ++i) {
      for (size_t j = row_indices_[i]; j < row_indices_[i+1]; ++j)  {
	y(i) += storage_[j] * x(col_indices_[j]);
      }
    }
  }


  void omp_matvec(const Vector& x, Vector& y) const {
    size_t i = 0;
    size_t j = 0;
#pragma omp parallel shared(y) private(i,j)
    {
#pragma omp for schedule(static)
    for (i = 0; i < num_rows_; ++i) {
      for (j = row_indices_[i]; j < row_indices_[i+1]; ++j)  {
	y(i) += storage_[j] * x(col_indices_[j]);
      }
    }
    }
  }


  void partition_by_nnz(size_t parts) {
    std::fstream stats_file;
    stats_file.open("part_stats.csv", std::ios::out | std::ios::app);
    assert(parts > 0);
    num_partitions_ = parts;
    size_t xsize = num_nonzeros() / parts;
    size_t xrem = num_nonzeros() % parts;
    partitions_.resize(parts+1);

    // Need to split into roughly equal size portions...by rows
    // For instance, if rows 0 and 1 have n elements each and row 1 has 2n
    // elements, 0 and 1 would be in partition A and row 2 in partition B.
    // Will pinch off a partition once the number of elements accumulated
    // meets or exceeds the ideal size. At the last partition, just
    // stuffs all remaining in.
    size_t total = 0;
    size_t running_total = 0;
    size_t partition_count = 0;
    stats_file << "\nNumber of elements per partition,";
    for (size_t i = 0; i < row_indices_.size()-2; ++i) {
      size_t num_nonzeros_in_row = row_indices_[i+1]-row_indices_[i];
      total += num_nonzeros_in_row;
      if (total >= xsize+xrem) {
        // we've accumulated enough for this partition
        assert(partition_count < partitions_.size() - 1);
        partitions_[partition_count] = i;
        stats_file << total << ",";
        running_total += total;
        total = 0;
        if (xrem > 0) { --xrem; }
      }
    }
    partitions_[partitions_.size()-1] = row_indices_.size()-1;
    // TODO: add an appropriate assert to validate total number of nonzero
    // elements is correct
    stats_file.close();
  }

  void partition_by_row(size_t parts) {
    num_partitions_ = parts;
    size_t xsize = num_rows_ / parts;
    size_t xrem = num_rows_ % parts;
    partitions_.resize(parts+1);
    std::fill(partitions_.begin()+1, partitions_.end(), xsize);
    for (size_t i = 1; i <= xrem; ++i) {
      ++partitions_[i];
    }
    std::partial_sum(partitions_.begin(), partitions_.end(), partitions_.begin());
    assert(partitions_[parts] == num_rows_);
  }

  void matvec_part(const Vector& x, Vector& y, size_t part) const {
    for (size_t i = partitions_[part]; i < partitions_[part+1]; ++i) {
      for (size_t j = row_indices_[i]; j < row_indices_[i+1]; ++j)  {
	y(i) += storage_[j] * x(col_indices_[j]);
      }
    }
  }
  
  void par_matvec(const Vector& x, Vector& y) const {

    std::vector<std::future<void>> futures_;
    for (size_t p = 0; p < partitions_.size()-1; ++p) {    
      futures_.emplace_back(std::async(std::launch::async, &CSRMatrix::matvec_part, this, std::cref(x), std::ref(y), p));
    }
    for (size_t p = 0; p < partitions_.size()-1; ++p) {
      futures_[p].wait();
    }
  }



private:
  bool                is_open;
  size_t              num_rows_, num_cols_;
  std::vector<size_t> row_indices_, col_indices_;
  std::vector<double> storage_;


  size_t num_partitions_;
  std::vector<size_t> partitions_;
};

#endif    // __CSRMATRIX_HPP
