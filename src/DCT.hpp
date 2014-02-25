#ifndef DCT_HPP
#define DCT_HPP 

#include "Matrix.hpp"
#include "Pixel.hpp"

namespace {
  double pi() { 
    return 3.14159265358979;
  }
}

// The class provides the ability to use the Discrete Cousinus transformation an
// an image matrix
class DCT {
public:
  DCT();
  DCT(const Matrix<Pixel<double> > &data, const unsigned int &color_depth);
  DCT(const DCT &copy);
  ~DCT();
  void forward_dct();
  void inverse_dct();
  void set_quality(const unsigned char &quality);
  unsigned char get_quality() const;   
  void set_color_depth(const unsigned int &color_depth);
  unsigned int get_color_depth() const;
  Matrix<Pixel<double> > get_mat_pic() const;
  void set_quantization(const Matrix<unsigned char> &quantization);
  Matrix<unsigned char> get_quantization() const;
  DCT operator=(const DCT &copy);

protected:
  void round(Matrix<Pixel<double> > &mat);
  void quantisation(Matrix<Pixel<double> > &pic_mat,
                    const Matrix<unsigned char> &quantization,
                    const double &quality);
  void inv_quantisation(Matrix<Pixel<double> > &pic_mat,
                        const Matrix<unsigned char> &quantization,
                        const double &quality);
  void make_transformation_matrix();
  void create_default_quantisation();


  double quality_;
  unsigned int color_depth_;
  Matrix<Pixel<double> > *mat_pic_;
  Matrix<double> *transformation_;
  Matrix<unsigned char> *quantization_;
};

#endif
