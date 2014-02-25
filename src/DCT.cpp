#include <math.h>

#include "DCT.hpp" 
#include "FMatrix.hpp"

// Default constructor
DCT::DCT() {
  quality_ = 1;
  mat_pic_ = NULL; 
  color_depth_= 0;
  quantization_ = new Matrix<unsigned char>(8);
  create_default_quantisation();
  transformation_ = new Matrix<double>(8);
  make_transformation_matrix();
}

// constructor with an image matrix and a color depth to transform the image
// matrix
DCT::DCT(const Matrix<Pixel<double> > &data, const unsigned int &color_depth) {
  quality_ = 1;
  color_depth_ = color_depth;
  mat_pic_ = new Matrix<Pixel<double> >(data);
  quantization_ = new Matrix<unsigned char>(8);
  create_default_quantisation();
  transformation_ = new Matrix<double>(8);
  make_transformation_matrix();
}

// copy constructor
DCT::DCT(const DCT &copy) { 
  quality_ = copy.get_quality(); 
  color_depth_ = copy.get_color_depth();
  mat_pic_ = new Matrix<Pixel<double> >(copy.get_mat_pic());
  quantization_ = new Matrix<unsigned char>(copy.get_quantization()); 
  transformation_ = new Matrix<double>(8);
  make_transformation_matrix();
}

//destructor
DCT::~DCT() {
  delete mat_pic_;
  delete transformation_;
  delete quantization_;
}
 
// applies the discret cosinuns transformation
void DCT::forward_dct() {
  unsigned int width = mat_pic_->get_col_length();
  unsigned int height = mat_pic_->get_row_length();
  Pixel<double> emptyPix = Pixel<double>(mat_pic_->get_data(0,0).size(), 0);
  Matrix<Pixel<double> > mat_buffer = Matrix<Pixel<double> >(8);
  Matrix<Pixel<double> > trans = Matrix<Pixel<double> >(8, 8, emptyPix);
  trans += *transformation_;

  for(unsigned int i = 0; i < height; i += 8) {
    for(unsigned int j = 0; j < width; j += 8) {

      for(unsigned int k = i, k_end = i + 8; k < k_end; ++k) {
        for(unsigned int l = j, l_end = j + 8; l < l_end; ++l) {
          if(k >= height || l >= width) { 
            mat_buffer.set_data(k - i, l - j, emptyPix);
          } else {
            mat_buffer.set_data(k - i, l - j, mat_pic_->get_data(k, l));
          }
        }
      }

      mat_buffer = mat_buffer - 128;

      mat_buffer = trans.transpose() * mat_buffer * trans;

      quantisation(mat_buffer, *quantization_, quality_); 

      round(mat_buffer);
      
      for(unsigned int k = i, k_end = i + 8; k < k_end; ++k) {
        for(unsigned int l = j, l_end = j + 8; l < l_end; ++l) {
          if(k < height && l < width) { 
            mat_pic_->set_data(k, l, mat_buffer.get_data(k - i, l - j));
          }
        }
      }
    }
  }
}

// applies the inverse cosinuns transformation
void DCT::inverse_dct() {
  unsigned int width = mat_pic_->get_col_length();
  unsigned int height = mat_pic_->get_row_length();
  Pixel<double> emptyPix = Pixel<double>(mat_pic_->get_data(0,0).size(), 0);
  Matrix<Pixel<double> > mat_buffer = Matrix<Pixel<double> >(8);
  Matrix<Pixel<double> > trans = Matrix<Pixel<double> >(8, 8, emptyPix); 
  trans += *transformation_;

  for(unsigned int i = 0; i < height; i += 8) {
    for(unsigned int j = 0; j < width; j += 8) {

      for(unsigned int k = i, k_end = i + 8; k < k_end; ++k) {
        for(unsigned int l = j, l_end = j + 8; l < l_end; ++l) {
          if(k >= height || l >= width) { 
            mat_buffer.set_data(k - i, l - j, emptyPix);
          } else {
            mat_buffer.set_data(k - i, l - j, mat_pic_->get_data(k, l));
          }
        }
      }

      inv_quantisation(mat_buffer, *quantization_, quality_); 

      mat_buffer = trans * mat_buffer * trans.transpose();

      mat_buffer = mat_buffer + 128;

      round(mat_buffer);

      for(unsigned int k = i, k_end = i + 8; k < k_end; ++k) {
        for(unsigned int l = j, l_end = j + 8; l < l_end; ++l) {
          if(k < height && l < width) { 
            mat_pic_->set_data(k, l, mat_buffer.get_data(k - i, l - j));
          }
        }
      }
    }
  }

}

void DCT::set_quality(const unsigned char &quality) {
  quality_ = ((quality > 100) ? 100 : quality);
  quality_ /=  50; 
}

unsigned char DCT::get_quality() const {
  return quality_ *  50.0;
}
void DCT::set_color_depth(const unsigned int &color_depth) { 
  color_depth_ = color_depth;
}

unsigned int DCT::get_color_depth() const {
  return color_depth_;
} 
 
Matrix<Pixel<double> > DCT::get_mat_pic() const {
  return *mat_pic_; 
}

void DCT::set_quantization(const Matrix<unsigned char> &quantization) {
  delete quantization_;
  quantization_ = new Matrix<unsigned char> (quantization);
}

Matrix<unsigned char> DCT::get_quantization() const { 
  return *quantization_;  
}  

DCT DCT::operator=(const DCT &copy) {
  if(this != &copy) {
    Matrix<double> *del_1;
    quality_ = copy.get_quality();
    color_depth_ = copy.get_color_depth();
    del_1 = transformation_;
    transformation_ = new Matrix<double>(8);
    delete del_1;
    Matrix<unsigned char> *del_2 = quantization_;
    quantization_ = new Matrix<unsigned char>(copy.get_quantization());  
    delete del_2; 
    Matrix<Pixel<double> > *del_3 = mat_pic_;
    mat_pic_ = new Matrix<Pixel<double> >(copy.get_mat_pic());
    delete del_3;

    make_transformation_matrix();
  }
  return *this;
}

// rounds the subpixels of the image matrix
void DCT::round(Matrix<Pixel<double> > &mat) {
  Pixel<double> pixel;
  double buffer;

  for(unsigned int i = 0; i < mat.get_row_length(); ++i) {
    for(unsigned int j = 0; j < mat.get_col_length(); ++j) {
      pixel = mat.get_data(i, j);

      for(unsigned int k = 0; k < pixel.size(); ++k) {
        buffer = pixel.get_pixel(k);

        if(buffer > 0) {
          buffer = buffer + 0.5;
        } else {
          buffer = buffer - 0.5;
        }
        pixel.set_pixel(k, (int)buffer);
      }
      mat.set_data(i, j,  pixel);
    }
  } 
}
void DCT::quantisation(Matrix<Pixel<double> > &pic_mat,
                  const Matrix<unsigned char> &quantization,
                  const double &quality) {
  Pixel<double> buffer;

  for(unsigned int i = 0; i < pic_mat.get_row_length(); ++i) {
    for(unsigned int j = 0; j < pic_mat.get_col_length(); ++j) {
      buffer = pic_mat.get_data(i, j) * quality / quantization.get_data(i, j);
      pic_mat.set_data(i, j, buffer);
    }
  }
}

void  DCT::inv_quantisation(Matrix<Pixel<double> > &pic_mat,
                      const Matrix<unsigned char> &quantization,
                      const double &quality) {
  Pixel<double> buffer;

  for(unsigned int i = 0; i < pic_mat.get_row_length(); ++i) {
    for(unsigned int j = 0; j < pic_mat.get_col_length(); ++j) {
      buffer = pic_mat.get_data(i, j) / quality * quantization.get_data(i, j);
      pic_mat.set_data(i, j, buffer);
    }
  }
} 

void  DCT::make_transformation_matrix() {
  double first_row = 1 / (2 * sqrt(2));

  for(unsigned int i = 0; i < 8; ++i) {
    transformation_->set_data(i, 0, first_row);

    for(int j = 1; j < 8; ++j) {
      transformation_->set_data(i, j, 0.5 * cos((2 * i + 1) * j 
                                      * ::pi() / 16)); 
    }
  } 
}

void DCT::create_default_quantisation() {
  unsigned int i = 0;
  unsigned int j = 0;

  quantization_->set_data(i, j++, 16);
  quantization_->set_data(i, j++, 11);
  quantization_->set_data(i, j++, 10);
  quantization_->set_data(i, j++, 16);
  quantization_->set_data(i, j++, 24);
  quantization_->set_data(i, j++, 40);
  quantization_->set_data(i, j++, 51);
  quantization_->set_data(i, j++, 61);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 12);
  quantization_->set_data(i, j++, 12);
  quantization_->set_data(i, j++, 14);
  quantization_->set_data(i, j++, 19);
  quantization_->set_data(i, j++, 26);
  quantization_->set_data(i, j++, 58);
  quantization_->set_data(i, j++, 60);
  quantization_->set_data(i, j++, 55);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 14);
  quantization_->set_data(i, j++, 13);
  quantization_->set_data(i, j++, 16);
  quantization_->set_data(i, j++, 24);
  quantization_->set_data(i, j++, 40);
  quantization_->set_data(i, j++, 57);
  quantization_->set_data(i, j++, 69);
  quantization_->set_data(i, j++, 56);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 14);
  quantization_->set_data(i, j++, 17);
  quantization_->set_data(i, j++, 22);
  quantization_->set_data(i, j++, 29);
  quantization_->set_data(i, j++, 51);
  quantization_->set_data(i, j++, 87);
  quantization_->set_data(i, j++, 80);
  quantization_->set_data(i, j++, 62);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 18);
  quantization_->set_data(i, j++, 22);
  quantization_->set_data(i, j++, 37);
  quantization_->set_data(i, j++, 56);
  quantization_->set_data(i, j++, 68);
  quantization_->set_data(i, j++, 109);
  quantization_->set_data(i, j++, 103);
  quantization_->set_data(i, j++, 77);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 24);
  quantization_->set_data(i, j++, 35);
  quantization_->set_data(i, j++, 55);
  quantization_->set_data(i, j++, 64);
  quantization_->set_data(i, j++, 81);
  quantization_->set_data(i, j++, 104);
  quantization_->set_data(i, j++, 113);
  quantization_->set_data(i, j++, 92);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 49);
  quantization_->set_data(i, j++, 64);
  quantization_->set_data(i, j++, 78);
  quantization_->set_data(i, j++, 87);
  quantization_->set_data(i, j++, 103);
  quantization_->set_data(i, j++, 121);
  quantization_->set_data(i, j++, 120);
  quantization_->set_data(i, j++, 101);
  ++i;
  j = 0;
  quantization_->set_data(i, j++, 72);
  quantization_->set_data(i, j++, 92);
  quantization_->set_data(i, j++, 95);
  quantization_->set_data(i, j++, 98);
  quantization_->set_data(i, j++, 112);
  quantization_->set_data(i, j++, 100);
  quantization_->set_data(i, j++, 103);
  quantization_->set_data(i, j++, 99);
}
