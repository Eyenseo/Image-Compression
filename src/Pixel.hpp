#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <stdlib.h>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <limits>

/**
 * The Pixel class is used to represent a Pixel, with subpixels for e.g. RGB
 * it implements all mathematical operations and compare operations.
 */
template <typename T> class Pixel {
public:
  /**
   * Default constructor
   */
  Pixel() {
    size_ = 0;
    pixel_ = NULL;
  }

  /**
   * Copy Constructor
   */
  Pixel(const Pixel<T> &copy) {
    size_ = copy.size();
    pixel_ = new T[size_]();

    for(int i = 0; i < size_; ++i) {
      pixel_[i] = copy.get_pixel(i);
    }
  }

  /**
   * Constructor with a defined amount of subpixels
   *
   * @param subpixels Defines the amount of subpixels e.g. RGB
   */
  Pixel(const unsigned char &subpixels) {
    size_ = subpixels;
    pixel_ = new T[subpixels]();
  }

  /**
   * Constructor with a defined amount of subpixels and a certain value
   * 
   * @param subpixels Defines the amount of subpixels e.g. RGB
   * @param val       Defines the value for the subpixels
   */
  Pixel(const unsigned char &subpixels, const T &val) {
    size_ = subpixels;
    pixel_ = new T[subpixels]();

    T checked = get_valid_value(val);

    for(int i = 0; i < size_; ++i) {
      pixel_[i] = checked;
    }
  }

  /**
   * Destructor
   */
  ~Pixel() {
    delete[] pixel_;
  } 

  /**
   * @return returns the Pixels average gray value 
   */
  inline Pixel<T> get_grey() const {
    Pixel<T> p = Pixel<T>(*this);
    
    p.grey();

    return p;
  }

  /**
   * @return returns a reference to the calling object, the object has only one
   * subpixel that has the value of the average gray value
   */
  Pixel<T> &grey() {
    T *del = pixel_;
    pixel_ = new T[1]();
    
    for(unsigned char i = 0; i < size_; ++i) {
      pixel_[0] += del[i] / size_;
    }

    validate_value(pixel_[0]);

    delete[] del;
    size_ = 1;
    return *this;
  }

  /**
   * @return returns the size of the Pixel - the amount of subpixels
   */
  inline unsigned char size() const {
    return size_;
  }

  /**
   * @param  pixel specifies the number of the subpixel to get e.g 2 to get the
   *               blue subpixel of RGB
   * 
   * @return       returns the value of the specified subpixel
   */
  inline T get_pixel(const unsigned char &pixel) const {
    if(pixel >= size_) {
      std::ostringstream ss;
      ss << "Subpixel " << pixel << " was requestet from a Pixel of size: "
      << size_ << '\n';

      throw std::range_error(ss.str());
    }
    return pixel_[pixel];
  }

  /**
   * @param  pixel specifies the number of the subpixel to get e.g 2 to get the
   *               blue subpixel of RGB
   * @param val   value to be set at the specified subpixel
   */
  inline void set_pixel(const unsigned char &pixel, const T &val) {
    if(pixel >= size_) {
      std::ostringstream ss;
      ss << "Subpixel " << pixel << " was requestet from a Pixel of size: "
      << size_ << '\n';

      throw std::range_error(ss.str());
    }

    T checked = get_valid_value(val);

    pixel_[pixel] = checked;
  }

  /**
   * Mathematical operation +
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  Pixel<T> operator+=(const Pixel<T> &rhs) {
    if(size_ == rhs.size()) {
      for(unsigned char i = 0; i < size_; ++i) {
        pixel_[i] += rhs.get_pixel(i);

        validate_value(pixel_[i]);
      }
      return *this;
    } else if(size_ == 0) {
      size_ = rhs.size();
  
      delete[] pixel_; 
      
      T *temp = new T[size_];

      for(unsigned char i = 0; i < size_; ++i) {
        temp[i] = rhs.get_pixel(i);
      }

      
      pixel_ = temp;; 
      return *this;
    } 
    std::ostringstream ss;
    ss << "A Addition between pixels can only happen when they have the same "
    << "size!\t LHS: " << size_ << "\tRHS: "<< rhs.size() << '\n';

    throw std::range_error(ss.str());
  }

  /**
   * Mathematical operation +
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  inline friend const Pixel<T> operator+(Pixel<T> lhs, const Pixel<T>& rhs) {
    lhs += rhs;
    return lhs;
  }

  /**
   * Mathematical operation +
   * it additions the subpixels with the value 
   */
  Pixel<T> operator+=(const T &rhs) {
    for(unsigned char i = 0; i < size_; ++i) {
      pixel_[i] += rhs;
      validate_value(pixel_[i]);
    }
    return *this;
  }
  
  /**
   * Mathematical operation +
   * it additions the subpixels with the value 
   */
  inline friend const Pixel<T> operator+(Pixel<T> lhs, const T &rhs) {
    lhs += rhs;
    return lhs;
  }

  /**
   * Mathematical operation -
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  Pixel<T> operator-=(const Pixel<T> &rhs) {
    if(size_ == rhs.size()) {
      for(unsigned char i = 0; i < size_; ++i) {
        pixel_[i] -= rhs.get_pixel(i);
        validate_value(pixel_[i]);
      }
      return *this;
    } else if(size_ == 0) {
       size_ = rhs.size();
  
      delete[] pixel_; 
      
      T *temp = new T[size_];

      for(unsigned char i = 0; i < size_; ++i) {
        temp[i] = rhs.get_pixel(i);
      }

      pixel_ = temp;; 
      return *this;
    }
    std::ostringstream ss;
    ss << "A Subtraction between pixels can only happen when they have the same"
    << " size!\t LHS: " << size_ << "\tRHS: "<< rhs.size() << '\n';

    throw std::range_error(ss.str());
  }

  /**
   * Mathematical operation -
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  inline friend const Pixel<T> operator-(Pixel<T> lhs, const Pixel<T>& rhs) {
    lhs -= rhs;
    return lhs;
  }
  
  /**
   * Mathematical operation -
   * it subtracts the subpixels with the value 
   */
  Pixel<T> operator-=(const T &rhs) {
    for(unsigned char i = 0; i < size_; ++i) {
      pixel_[i] -= rhs;
      validate_value(pixel_[i]);
    }
    return *this;
  }

  /**
   * Mathematical operation -
   * it subtracts the subpixels with the value 
   */
  inline friend const Pixel<T> operator-(Pixel<T> lhs, const T& rhs) {
    lhs -= rhs;
    return lhs;
  }

  /**
   * Mathematical operation *
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  Pixel<T> operator*=(const Pixel<T> &rhs) {
    if(size_ == rhs.size()) {
      for(unsigned char i = 0; i < size_; ++i) {
        pixel_[i] *= rhs.get_pixel(i);
        validate_value(pixel_[i]);
      }
      return *this;
    } else if(size_ == 0) {
       size_ = rhs.size();
  
      delete[] pixel_; 
      
      T *temp = new T[size_];

      for(unsigned char i = 0; i < size_; ++i) {
        temp[i] = rhs.get_pixel(i);
      }

      
      pixel_ = temp;; 
      return *this;
    }
    std::ostringstream ss;
    ss << "A Multiplication between pixels can only happen when they have the "
    << "same size!\t LHS: " << size_ << "\tRHS: "<< rhs.size() << '\n';

    throw std::range_error(ss.str());
  }

  /**
   * Mathematical operation *
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  inline friend const Pixel<T> operator*(Pixel<T> lhs, const Pixel<T>& rhs) {
    lhs *= rhs;
    return lhs;
  }

  /**
   * Mathematical operation *
   * it multiplies the subpixels with the value 
   */
  Pixel<T> operator*=(const T &rhs) {
    for(unsigned char i = 0; i < size_; ++i) {
      pixel_[i] *= rhs;
      validate_value(pixel_[i]);
    }
    return *this;
  }

  /**
   * Mathematical operation *
   * it multiplies the subpixels with the value 
   */
  inline friend const Pixel<T> operator*(Pixel<T> lhs, const T& rhs) {
    lhs *= rhs;
    return lhs;
  }

  /**
   * Mathematical operation /
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  Pixel<T> operator/=(const Pixel<T> &rhs) {
    if(size_ == rhs.size()) {
      for(unsigned char i = 0; i < size_; ++i) {
        pixel_[i] /= rhs.get_pixel(i);
        validate_value(pixel_[i]);
      }
      return *this;
    } else if(size_ == 0) {
       size_ = rhs.size();
  
      delete[] pixel_; 
      
      T *temp = new T[size_];

      for(unsigned char i = 0; i < size_; ++i) {
        temp[i] = rhs.get_pixel(i);
      }

      
      pixel_ = temp;; 
      return *this;
    }
    std::ostringstream ss;
    ss << "A Division between pixels can only happen when they have the same "
    << "size!\t LHS: " << size_ << "\tRHS: "<< rhs.size() << '\n';

    throw std::range_error(ss.str());
  }

  /**
   * Mathematical operation /
   * if the size of the left hand side object is 0 the right hand side 
   * else the size of both pixels has to be the same
   */
  inline friend const Pixel<T> operator/(Pixel<T> lhs, const Pixel<T>& rhs) {
    lhs /= rhs;
    return lhs;
  }
  
  /**
   * Mathematical operation /
   * it divides the subpixels with the value 
   */
  Pixel<T> operator/=(const T &rhs) {
    for(unsigned char i = 0; i < size_; ++i) {
      pixel_[i] /= rhs;
      validate_value(pixel_[i]);
    }
    return *this;
  }
    /**
   * Mathematical operation /
   * it divides the subpixels with the value 
   */
  inline friend const Pixel<T> operator/(Pixel<T> lhs, const T& rhs) {
    lhs /= rhs;
    return lhs;
  }

  bool operator==(const Pixel<T> &rhs) const {
    if(size == rhs.size()) {
      for(unsigned char i = 0; i < size_; ++i) {
        if(pixel_[i] != rhs.get_pixel(i)) {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  bool operator!=(const Pixel<T> &rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const Pixel<T> &rhs) const {
   return get_grey().get_pixel(0) < rhs.get_grey().get_pixel(0);
  } 

  bool operator>=(const Pixel<T> &rhs) const {
   return !(*this < rhs);
  }

  bool operator>(const Pixel<T> &rhs) const {
   return get_grey().get_pixel(0) > rhs.get_grey().get_pixel(0);
  }

  bool operator<=(const Pixel<T> &rhs) const {
   return !(*this > rhs);
  } 

  Pixel operator=(const Pixel& copy) {
    if(this != &copy) {
      size_ = copy.size();

      T *temp = pixel_;

      pixel_ = new T[size_]();
      for(unsigned char i = 0; i < size_; ++i) {
        pixel_[i] = copy.get_pixel(i);
      }
      delete[] temp; 
    }
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& out, const Pixel<T>& p) {
    out << '(';
    for(unsigned char i = 0; i < p.size(); ++i) {
      if(i != 0) {
        out << ' ';
      }
      out << p.get_pixel(i);
    }
    out << ')';

    return out;
  }

private:
  /**
   * The method checks if the value to be assigned, is a valid value
   * 
   * @param  value value to check
   * @return       returns a valid value
   */
  inline T get_valid_value(T value) const {
    if(isinf(value)) {
      return std::numeric_limits<T>::max();
    } else if(isnan(value)) {
      return std::numeric_limits<T>::min();
    } else {
      return value;
    }
  }

  /**
   * changes the value to a valid value
   * 
   * @param value value to be checked / changed
   */
  inline void validate_value(T &value) const {
    if(isinf(value)) {
      value = std::numeric_limits<T>::max();
    } else if(isnan(value)) {
      value = std::numeric_limits<T>::min();
    }
  }

  unsigned char size_;
  T *pixel_;
};
#endif
