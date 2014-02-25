#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <stdexcept>


namespace MatrixExce{
  class DimensionsNotIdentical : public std::exception {
   public:

    DimensionsNotIdentical() {
      msg =  "MatrixExce: DimensionsNotIdentical\nDimensions did not agree.\n";
    }

    template <typename TL, typename TR>
    DimensionsNotIdentical(TL &l, TR &r) {
      std::ostringstream ss;

      ss  << "MatrixExce:\nDimensions did not agree.\n"
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

  class MatrixNotEmpty : public std::exception {
   public:
    MatrixNotEmpty(std::string s = "") {
      msg = "MatrixExce: DimensionsNotIdentical\n"
      "Matrix has to be resized down with flag!\n" + s + '\n';
    }

    ~MatrixNotEmpty() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };
}

/**
 * The Matrix class is en equivalent of a mathematical Matrix
 */
template <typename T> class Matrix {
  typedef std::vector<std::vector<T> > Vector2D;

 public:
  //Default Constructor 1 x 1
  Matrix() {
    data_ = new Vector2D(0, std::vector<T>(0));
  }

  //Specific Size Constructor m x m
  Matrix(const unsigned int &m) {
    data_ = new Vector2D(m, std::vector<T>(m));
  }

  //Specific Size Constructor m x n
  Matrix(const unsigned int &m, const unsigned int &n, const T &val = T()) {
    data_ = new Vector2D(m, std::vector<T>(n));

    for(unsigned int i = 0; i < m; ++i) {
      for(int j = 0; j < n; ++j) {
        data_->at(i).at(j) = val;
      }
    }
  }
  // copy constructor
  Matrix(const Matrix<T> &copy) {
   data_ = new Vector2D(copy.get_row_length(),
                        std::vector<T>(copy.get_col_length()));

   for(unsigned int i = 0; i < copy.get_row_length(); ++i) {
     for(unsigned int j = 0; j < copy.get_col_length(); ++j) {
       data_->at(i).at(j) = copy.get_data(i, j);
     }
   }
  }

  //Destructor
  ~Matrix() {
    delete data_;
  }

  // returns the matrix transposed
  Matrix<T> transpose() const {
    Matrix<T> ret = Matrix(data_->at(0).size(), data_->size());

    for(unsigned int i = 0; i < ret.get_row_length(); i++) {
      for(unsigned int j = 0; j < ret.get_col_length(); j++) {
        ret.set_data(i, j, data_->at(j).at(i));
      }
    }
    return ret;
  }

  //Adjusts the size of the matrix
  void set_size(const unsigned int &m,
                const unsigned int &n,
                const T &val = T(),
                const bool ignore_size = false) {

    if( ignore_size == true
        || (m >= get_row_length() && n >= get_col_length()) ) {

      if(m != get_row_length()) {
        data_->resize(m, std::vector<T>(n));
      }
      if(n != get_col_length()) {
        for(unsigned int i = 0; i < m; i++) {
          data_->at(i).resize(n, val);
        }
      }
    } else {
       throw MatrixExce::MatrixNotEmpty();
    }
  }

  // sets the data at the position
  void set_data(const long &pos, const T data) {
    unsigned int row = pos / get_col_length();
    unsigned int col = pos % get_col_length();

    if(row >= get_row_length() || col >= get_col_length()) {
      std::ostringstream ss;
      ss << "There was an attempt to set data in a Matrix via 'set_data(const T"
      << "&data, const long &pos)' with a bad position: Pos: " << pos 
      << "\nRow: " << row << "\tCol: " << col << "\nThe size of the matrix was:"
      << "\nRow: " << get_row_length() << "\tCol: " << get_col_length() << "\n";
      throw std::range_error(ss.str());
    }

    data_->at(row).at(col) = data;
  }


  //sets the data at m, n with value of data
  inline void set_data( const unsigned int &m,
                        const unsigned int &n,
                        const T &data) {
    data_->at(m).at(n) = data;
  }

  // returns the raw matrix / the vectors
  inline Vector2D get_raw() const {
    return *data_;
  }

  //returns the data at m, n
  inline T get_data(const unsigned int &m, const unsigned int &n) const {
    return data_->at(m).at(n);
  }

  //returns the length of the rows
  inline unsigned int get_row_length()  const {
    return data_->size();
  }

  //returns the length of the columns
  inline unsigned int get_col_length() const {
    if(data_->size() == 0) {
      return 0;
    }
    return data_->at(0).size();
  }

  // transforms the matrix to a sting readable for a human
  inline std::string to_string() const {
    std::ostringstream ss;

    for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
       ss  << get_data(i, j) << ' ';
      }
      ss << '\n';
    }
    ss << '\n';

    return ss.str();
  }

  // returns the total value of all cells
  inline T get_total_value() const {
    T res;

    for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
        res += get_data(i, j);
      }
    }
    return res;
  }

  // returns a Matrix with the expo of each cell
  inline Matrix<T> get_pow(int expo) const {
    Matrix<T> mat = Matrix<T>(*this);
    mat.pow(expo);
    return mat;
  }

  // calculates the power of each cell
  void pow(const int &expo) {
    if(expo != 0) {
      T res;  
      int conv_exp = (expo < 0) ? -expo : expo;
      
      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < get_col_length(); j++) {
          res = get_data(i, j);

          for(int k = conv_exp; k > 1; --k) {
            res *= get_data(i, j);
          }

          if(expo < 0) {
            res = 1 / res;
          }
          set_data(i, j, res);
        }
      }
    } else {
      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < get_col_length(); j++) {
          set_data(i, j, get_data(i, j) * 0 + 1);
        }
      }
    }
  }

  // returns the minimal cell
  T get_min() const {
    unsigned int j = 1;
    T res = get_data(0, 0);

    for(unsigned int i = 0; i < get_row_length(); i++) {
      for(;j < get_col_length(); j++) {
        if(res > get_data(i, j)) {
          res = get_data(i, j);
        }
      }
      j = 0;
    }

    return res;
  }

  // returns the maximal cell
  T get_max() const {
    unsigned int j = 1;
    T res = get_data(0, 0);

    for(unsigned int i = 0; i < get_row_length(); i++) {
      for(; j < get_col_length(); j++) {
        if(res < get_data(i, j)) {
          res = get_data(i, j);
        }
      }
      j = 0;
    }

    return res;
  }

  template <typename TParam>
  const Matrix<T> operator+=(const Matrix<TParam> &rhs) {
    if( get_row_length() == rhs.get_row_length()
        && get_col_length() == rhs.get_col_length()) {

      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < get_col_length(); j++) {
          set_data(i, j, (get_data(i, j) + rhs.get_data(i, j)));
        }
      }
      return *this;
    } else if(get_row_length() == 0) {
      Vector2D *temp = data_;
      data_ = new Vector2D( rhs.get_row_length(),
                            std::vector<T>(rhs.get_col_length()));      
      for(unsigned int i = 0; i < rhs.get_row_length(); ++i) {
        for(unsigned int j = 0; j < rhs.get_col_length(); ++j) {
          data_->at(i).at(j) = rhs.get_data(i, j);
        }
      }
      delete temp;
    }
    throw MatrixExce::DimensionsNotIdentical(*this, rhs);
  }

  template <typename TParam>
  friend Matrix<T> operator+(Matrix<T> lhs, const Matrix<TParam> &rhs) {
    lhs += rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator+=(const TParam &rhs) {
   for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
        set_data(i, j, get_data(i, j) + rhs);
      }
    }
    return *this;
  }

  template <typename TParam>
  friend Matrix<T> operator+(Matrix<T> lhs, const TParam& rhs) {
    lhs += rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator-=(const Matrix<TParam> &rhs) {
    if(get_row_length() == rhs.get_row_length()
      && get_col_length() == rhs.get_col_length()) {

      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < get_col_length(); j++) {
          set_data(i, j, (get_data(i, j) - rhs.get_data(i, j)));
        }
      }
      return *this;
    } else if(get_row_length() == 0) {
     Vector2D *temp = data_;

      data_ = new Vector2D( rhs.get_row_length(),
                            std::vector<T>(rhs.get_col_length()));
      for(unsigned int i = 0; i < rhs.get_row_length(); ++i) {
        for(unsigned int j = 0; j < rhs.get_col_length(); ++j) {
          data_->at(i).at(j) = rhs.get_data(i, j);
        }
      }
      delete temp;
    }
    throw MatrixExce::DimensionsNotIdentical(*this, rhs);
  }

  template <typename TParam>
  friend Matrix<T> operator-(Matrix<T> lhs, const Matrix<TParam> &rhs) {
    lhs -= rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator-=(const TParam &rhs) {
   for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
        set_data(i, j, get_data(i, j) - rhs);
      }
    }
    return *this;
  }

  template <typename TParam>
  friend Matrix<T> operator-(Matrix<T> lhs, const TParam& rhs) {
    lhs -= rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator*=(const Matrix<TParam> &rhs) {
    if(get_col_length() == rhs.get_row_length()) {
      T res;
      Vector2D *temp = data_;
      data_ = new Vector2D( get_row_length(),
                            std::vector<T>(rhs.get_col_length())); 

      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < rhs.get_col_length(); j++) {
          res = T();
          for(unsigned int k = 0; k < get_col_length(); k++) {
            res += temp->at(i).at(k) * rhs.get_data(k, j);
          }
          data_->at(i).at(j) = res;
        }
      }

      delete temp;
      return *this;
    }
    throw MatrixExce::DimensionsNotIdentical(*this, rhs);
  }

  template <typename TPara>
  friend Matrix<T> operator*(Matrix<T> lhs, const Matrix<TPara> &rhs) {
    lhs *= rhs;
    return lhs;
  }

  //Operator overloading for multiplication
  template <typename TParam>
  const Matrix<T> operator*=(const TParam &multiplier) {
    for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
        set_data(i, j, get_data(i, j) * multiplier);
      }
    }
    return *this;
  }

  template <typename TParam>
  friend Matrix<T> operator*(Matrix<T> lhs, const TParam &rhs) {
    lhs *= rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator/=(const Matrix<TParam> &rhs) {
    if( get_row_length() == rhs.get_row_length()
        && get_col_length() == rhs.get_col_length()) {

      for(unsigned int i = 0; i < get_row_length(); i++) {
        for(unsigned int j = 0; j < get_col_length(); j++) {
          set_data(i, j, (get_data(i, j) / rhs.get_data(i, j)));
        }
      }
      return *this;
    } else if(get_row_length() == 0) {
      Vector2D *temp = data_;
      data_ = new Vector2D( rhs.get_row_length(),
                            std::vector<T>(rhs.get_col_length()));

      for(unsigned int i = 0; i < rhs.get_row_length(); ++i) {
        for(unsigned int j = 0; j < rhs.get_col_length(); ++j) {
          data_->at(i).at(j) = rhs.get_data(i, j);
        }
      }
      delete temp;
    }
    throw MatrixExce::DimensionsNotIdentical(*this, rhs);
  }

  template <typename TParam>
  const inline friend Matrix<T> operator/(Matrix<T> lhs,
                                          const Matrix<TParam> &rhs) {
    lhs /= rhs;
    return lhs;
  }

  template <typename TParam>
  const Matrix<T> operator/=(const TParam &rhs) {
   for(unsigned int i = 0; i < get_row_length(); i++) {
      for(unsigned int j = 0; j < get_col_length(); j++) {
        set_data(i, j, get_data(i, j) / rhs);
      }
    }
    return *this;
  }

  template <typename TParam>
  inline friend Matrix<T> operator/(Matrix<T> lhs, const TParam& rhs) {
    lhs /= rhs;
    return lhs;
  }
  
  Matrix<T> operator=(const Matrix<T> &rhs) {
    if(this != &rhs) {
      Vector2D *temp = data_;
      data_ = new  Vector2D(rhs.get_row_length(),
                            std::vector<T>(rhs.get_col_length()));

      for(unsigned int i = 0; i < rhs.get_row_length(); ++i) {
        for(unsigned int j = 0; j < rhs.get_col_length(); ++j) {
          data_->at(i).at(j) = rhs.get_data(i, j);
        }
      }
      delete temp;
    }
    return *this;
  }

 protected:
  Vector2D *data_;
};
#endif
