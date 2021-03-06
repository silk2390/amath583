#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/sequence.h>

#include "Timer.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

// Sequential norm for verification purposes
// Confirmed working!
double norm(const thrust::host_vector<float>& v) {
  double sum = 0.0;
  for (size_t i = 0; i < v.size(); ++i){
    sum += v[i] * v[i];
  }
  return std::sqrt(sum);
}

template <typename VectorType, typename T>
void randomize(VectorType &x, T scale) {
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(-scale, scale);
  static auto dice = std::bind(distribution, generator);

  for (size_t i = 0; i < x.size(); ++i) {
    x[i] = dice();
  }
}


template <typename T>
struct square
{
  // The __host__ __device__ is required to get rid of a MASSIVE template warning
  // Per the github documentation
  __host__ __device__ T operator()(const T& x)
  {
    return x * x;
  }
};


int main(int argc, char* argv[]) {

  size_t exponent           = 27;
  size_t num_trips          = 1;

  if (argc >= 2) exponent   = std::stol(argv[1]);
  if (argc >= 3) num_trips  = std::stol(argv[2]);

  size_t num_elements = 1 << exponent;
  
  thrust::host_vector<float> x(num_elements);
  randomize(x, 10.0f);
  thrust::device_vector<float> device_x(num_elements);
  thrust::copy(x.begin(), x.end(), device_x.begin());

  float                init = 0.0;
  float                result = 0.0;
  square<float>        unary_op;
  thrust::plus<float> binary_op;

  DEF_TIMER(gpu_norm);
  START_TIMER(gpu_norm);

  cudaDeviceSynchronize();
  for (size_t i = 0; i < num_trips; ++i) {
    /* write me -- use transform reduce (?) with unary op and binary op defined above */
    result = thrust::transform_reduce(device_x.begin(), device_x.end(), unary_op, init, binary_op);
    cudaDeviceSynchronize();
    result = std::sqrt(result);
  }

  double cuda_time = STOP_TIMER_QUIETLY(gpu_norm);
  std::cout << exponent << "\t" << num_trips << "\t" << cuda_time << "\t" << result << std::endl;

  return 0;
}
