#ifndef DIFF_PIC_HPP
#define DIFF_PIC_HPP 

#include <math.h>

#include "Matrix.hpp"
#include "Pixel.hpp"
#include "PPMFile.hpp"

namespace DiffPicExce{
  class DimensionsNotIdentical : public std::exception {
   public:

    DimensionsNotIdentical() {
      msg =  "DiffPicExce: DimensionsNotIdentical\nDimensions did not agree.\n";
    }

    template <typename TL, typename TR>
    DimensionsNotIdentical(TL &l, TR &r) {
      std::ostringstream ss;

      ss  << "DiffPicExce:\nDimensions did not agree.\n"
      << "LHS: row: " << l.get_row_length()
      << "\tcol: " << l.get_col_length()
      << "\nRHS: row: " << r.get_row_length()
      << "\tcol: " << r.get_col_length()
      << '\n';

      msg = ss.str();
    }

    ~DimensionsNotIdentical() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };
}

// The class provides the ability to compare to pictures - that have the same
// dimension and the same amount  of subpixels
// The class needs some serious rework on Exceptions for unexpected use!
class DiffPic {
public:
  // Default constructor
  DiffPic() {
    pic_one_ = NULL;    
    pic_two_ = NULL;
    diff_pic_ = NULL;
    rmse_ = 0;
  }

  // copy constructor
  DiffPic(const DiffPic &copy) {
    pic_one_ = new Matrix<Pixel<double> >(copy.get_pic_one());    
    pic_two_ = new Matrix<Pixel<double> >(copy.get_pic_two());
    diff_pic_ = new Matrix<Pixel<double> >(copy.get_diff_pic());
    rmse_ = get_rmse();
  }

  // Destructor
  ~DiffPic() {
    delete pic_one_; 
    delete pic_two_;
    delete diff_pic_;
  }

  // calculates the difference between the pictures
  void calculate() {
    if( pic_one_->get_row_length() != pic_two_->get_row_length()
        && pic_one_->get_col_length() != pic_two_->get_col_length()) {
      throw DiffPicExce::DimensionsNotIdentical(*pic_one_, *pic_two_);
    }
    diff_pic_ = new Matrix<Pixel<double> >((*pic_one_ - *pic_two_));
    Matrix<Pixel<double> > rmse_mat = *diff_pic_;

    rmse_mat.pow(2);
    rmse_ = sqrt(rmse_mat.get_total_value().get_grey().get_pixel(0) / 64);

    // *diff_pic_ -= diff_pic_->get_min().grey().get_pixel(0);
    // *diff_pic_ *= 255 / diff_pic_->get_max().grey().get_pixel(0);
    *diff_pic_ += 128 ;

    for(unsigned int i = 0; i < diff_pic_->get_row_length(); ++i) {
      for(unsigned int j = 0; j < diff_pic_->get_col_length(); ++j) {
        diff_pic_->set_data(i, j, diff_pic_->get_data(i, j).grey());
      }
    }
  } 

  //Wirtes the difference picture to a ppm file
  void write_to(const std::string filename) {  
    PPMFile<double> *p = new PPMFile<double>();

    p->set_mat_pic(*diff_pic_, 255);
    p->set_magic_number(5);
    p->write_to(filename);
    delete p;
  }

  inline void set_pic_one(const Matrix<Pixel<double> > &pic) {
    delete pic_one_;

    pic_one_ = new Matrix<Pixel<double> >(pic);
  }

  inline Matrix<Pixel<double> > get_pic_one() const {
    return *pic_one_;
  }

  inline void set_pic_two(const Matrix<Pixel<double> > &pic) {
    delete pic_two_;

    pic_two_ = new Matrix<Pixel<double> >(pic);
  }

  inline Matrix<Pixel<double> > get_pic_two() const {
    return *pic_one_;
  }

  inline Matrix<Pixel<double> > get_diff_pic() const {
    return *diff_pic_;
  }

  inline double get_rmse() const {
    return rmse_;
  }

  DiffPic operator=(const DiffPic &copy) {
    if(this != &copy) {
      delete pic_one_;
      pic_one_ = new Matrix<Pixel<double> >(copy.get_pic_one());    
      delete pic_two_;
      pic_two_ = new Matrix<Pixel<double> >(copy.get_pic_two());
      delete diff_pic_;
      diff_pic_ = new Matrix<Pixel<double> >(copy.get_diff_pic());
      rmse_ = get_rmse();
    }
    return *this;
  }

private: 
  Matrix<Pixel<double> > *pic_one_;
  Matrix<Pixel<double> > *pic_two_;
  Matrix<Pixel<double> > *diff_pic_;
  double rmse_;
};
#endif
