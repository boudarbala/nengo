#include "nengo/transforms.hpp"
#include <stdexcept>
#include <cmath>

namespace nengo {

// Dense Transform Implementation

Dense::Dense(std::pair<int, int> shape, double init) 
    : shape_(shape), init_value_(init), has_explicit_matrix_(false) {
    
    if (shape_.first <= 0 || shape_.second <= 0) {
        throw std::invalid_argument("Transform shape dimensions must be positive");
    }
    
    // Initialize matrix based on shape and init value
    matrix_ = Eigen::MatrixXd::Zero(shape_.first, shape_.second);
    
    // For square matrices, create diagonal matrix with init value
    if (shape_.first == shape_.second) {
        matrix_ = Eigen::MatrixXd::Identity(shape_.first, shape_.second) * init_value_;
    } else {
        // For non-square matrices, initialize with init value
        matrix_.setConstant(init_value_);
    }
}

Dense::Dense(int size_out, int size_in, double init) 
    : Dense(std::make_pair(size_out, size_in), init) {}

Dense::Dense(const Eigen::MatrixXd& matrix) 
    : shape_(matrix.rows(), matrix.cols()), 
      matrix_(matrix), 
      init_value_(0.0),
      has_explicit_matrix_(true) {}

Eigen::MatrixXd Dense::sample() const {
    return matrix_;
}

void Dense::setMatrix(const Eigen::MatrixXd& matrix) {
    if (matrix.rows() != shape_.first || matrix.cols() != shape_.second) {
        throw std::invalid_argument("Matrix dimensions do not match transform shape");
    }
    matrix_ = matrix;
    has_explicit_matrix_ = true;
}

// Convolution Transform Implementation

Convolution::Convolution(int n_filters, std::tuple<int, int, int> input_shape, 
                        int kernel_size, int strides, const std::string& padding)
    : n_filters_(n_filters), input_shape_(input_shape), 
      kernel_size_(kernel_size), strides_(strides), padding_(padding) {
    
    if (n_filters <= 0 || kernel_size <= 0 || strides <= 0) {
        throw std::invalid_argument("Convolution parameters must be positive");
    }
    
    computeOutputDimensions();
}

void Convolution::computeOutputDimensions() {
    auto [channels, height, width] = input_shape_;
    
    if (padding_ == "valid") {
        output_height_ = (height - kernel_size_) / strides_ + 1;
        output_width_ = (width - kernel_size_) / strides_ + 1;
    } else if (padding_ == "same") {
        output_height_ = std::ceil(static_cast<double>(height) / strides_);
        output_width_ = std::ceil(static_cast<double>(width) / strides_);
    } else {
        throw std::invalid_argument("Padding must be 'valid' or 'same'");
    }
    
    if (output_height_ <= 0 || output_width_ <= 0) {
        throw std::invalid_argument("Invalid convolution parameters result in non-positive output dimensions");
    }
}

int Convolution::getSizeIn() const {
    auto [channels, height, width] = input_shape_;
    return channels * height * width;
}

int Convolution::getSizeOut() const {
    return n_filters_ * output_height_ * output_width_;
}

Eigen::MatrixXd Convolution::sample() const {
    // This is a simplified implementation that creates a random convolution matrix
    // In a full implementation, this would properly implement convolution as matrix multiplication
    int size_in = getSizeIn();
    int size_out = getSizeOut();
    
    // Create a sparse-like matrix for convolution
    Eigen::MatrixXd conv_matrix = Eigen::MatrixXd::Zero(size_out, size_in);
    
    // Simplified: just create some connections based on kernel size
    // This is not a proper convolution implementation
    for (int i = 0; i < size_out; ++i) {
        for (int j = 0; j < std::min(kernel_size_ * kernel_size_, size_in); ++j) {
            if (i * size_in / size_out + j < size_in) {
                conv_matrix(i, i * size_in / size_out + j) = 1.0 / (kernel_size_ * kernel_size_);
            }
        }
    }
    
    return conv_matrix;
}

// Sparse Transform Implementation

Sparse::Sparse(std::pair<int, int> shape, 
               const std::vector<std::pair<int, int>>& indices,
               const std::vector<double>& values)
    : shape_(shape), indices_(indices), values_(values) {
    
    if (indices.size() != values.size()) {
        throw std::invalid_argument("Indices and values must have the same size");
    }
    
    // Check that indices are within bounds
    for (const auto& [row, col] : indices) {
        if (row < 0 || row >= shape_.first || col < 0 || col >= shape_.second) {
            throw std::invalid_argument("Index out of bounds");
        }
    }
}

Eigen::MatrixXd Sparse::sample() const {
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(shape_.first, shape_.second);
    
    for (size_t i = 0; i < indices_.size(); ++i) {
        auto [row, col] = indices_[i];
        matrix(row, col) = values_[i];
    }
    
    return matrix;
}

} // namespace nengo