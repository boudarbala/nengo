#pragma once

#include <Eigen/Dense>
#include <memory>
#include <tuple>

namespace nengo {

/**
 * A base class for connection transforms.
 */
class Transform {
public:
    virtual ~Transform() = default;
    
    /**
     * Returns concrete weights to implement the specified transform.
     * 
     * Returns
     * -------
     * Eigen::MatrixXd
     *     Transform weights
     */
    virtual Eigen::MatrixXd sample() const = 0;
    
    /**
     * Expected size of input to transform.
     */
    virtual int getSizeIn() const = 0;
    
    /**
     * Expected size of output from transform.
     */
    virtual int getSizeOut() const = 0;
    
    /**
     * Get the shape of the transform matrix: (size_out, size_in).
     */
    virtual std::pair<int, int> getShape() const {
        return {getSizeOut(), getSizeIn()};
    }
};

/**
 * A dense matrix transformation between an input and output signal.
 */
class Dense : public Transform {
public:
    /**
     * Constructor for Dense transform.
     * 
     * Parameters
     * ----------
     * shape : pair of int
     *     The shape of the dense matrix: (size_out, size_in).
     * init : double, optional
     *     Initialization value. Default is 1.0 for identity-like behavior.
     */
    Dense(std::pair<int, int> shape, double init = 1.0);
    
    /**
     * Constructor with explicit dimensions.
     */
    Dense(int size_out, int size_in, double init = 1.0);
    
    /**
     * Constructor with matrix initialization.
     */
    explicit Dense(const Eigen::MatrixXd& matrix);
    
    Eigen::MatrixXd sample() const override;
    int getSizeIn() const override { return shape_.second; }
    int getSizeOut() const override { return shape_.first; }
    std::pair<int, int> getShape() const override { return shape_; }
    
    // Set the matrix directly
    void setMatrix(const Eigen::MatrixXd& matrix);
    
    // Get the current matrix
    const Eigen::MatrixXd& getMatrix() const { return matrix_; }

private:
    std::pair<int, int> shape_;
    Eigen::MatrixXd matrix_;
    double init_value_;
    bool has_explicit_matrix_;
};

/**
 * A transform that implements convolutional operations.
 * Simplified version for basic convolution support.
 */
class Convolution : public Transform {
public:
    /**
     * Constructor for Convolution transform.
     * 
     * Parameters
     * ----------
     * n_filters : int
     *     Number of convolutional filters.
     * input_shape : tuple of int
     *     Shape of input (channels, height, width) or similar.
     * kernel_size : int
     *     Size of convolutional kernel.
     * strides : int, optional
     *     Stride of convolution. Default is 1.
     * padding : string, optional
     *     Padding type ("valid" or "same"). Default is "valid".
     */
    Convolution(int n_filters, std::tuple<int, int, int> input_shape, 
               int kernel_size, int strides = 1, 
               const std::string& padding = "valid");
    
    Eigen::MatrixXd sample() const override;
    int getSizeIn() const override;
    int getSizeOut() const override;

private:
    int n_filters_;
    std::tuple<int, int, int> input_shape_;  // (channels, height, width)
    int kernel_size_;
    int strides_;
    std::string padding_;
    
    void computeOutputDimensions();
    int output_height_;
    int output_width_;
};

/**
 * A sparse transformation matrix.
 * Simplified implementation using Eigen sparse matrices.
 */
class Sparse : public Transform {
public:
    /**
     * Constructor for Sparse transform.
     * 
     * Parameters
     * ----------
     * shape : pair of int
     *     The shape of the sparse matrix: (size_out, size_in).
     * indices : vector of pairs
     *     Non-zero indices as (row, col) pairs.
     * values : vector of double
     *     Values for the non-zero entries.
     */
    Sparse(std::pair<int, int> shape, 
           const std::vector<std::pair<int, int>>& indices,
           const std::vector<double>& values);
    
    Eigen::MatrixXd sample() const override;
    int getSizeIn() const override { return shape_.second; }
    int getSizeOut() const override { return shape_.first; }

private:
    std::pair<int, int> shape_;
    std::vector<std::pair<int, int>> indices_;
    std::vector<double> values_;
};

} // namespace nengo