#ifndef PPMFile_HPP
#define PPMFile_HPP

#include <stdlib.h>
#include <limits>
#include <fstream>
#include <exception>
#include <vector>
#include <sstream>

#include "Pixel.hpp"
#include "Matrix.hpp"

namespace PPMFileExce {
  class BadHeader : public std::exception {
   public:

    BadHeader() {
      msg =  "PPMFileExce: BadHeader\nBad header!\n";
    }

    BadHeader(const std::string &s) {
      msg =  "PPMFileExce: BadHeader\nBad header!\n" + s + '\n';
    }

    ~BadHeader() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };

  class UnexpectedFileEnd : public std::exception {
   public:

    UnexpectedFileEnd() {
      msg =  "PPMFileExce: UnexpectedFileEnd\nFile ended unexpected!\n";
    }

    UnexpectedFileEnd(const std::string &s) {
      msg =  "PPMFileExce: UnexpectedFileEnd\n"
      "File ended unexpected!\n" + s + '\n';
    }

    ~UnexpectedFileEnd() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };
  class BadDataType : public std::exception {
   public:

    BadDataType() {
      msg =  "PPMFileExce: BadDataTypen\nBad data type of PPMFile!\n";
    }

    BadDataType(const std::string &s) {
      msg =  "PPMFileExce: BadDataType\nBad data type of PPMFile!\n" + s + '\n';
    }

    ~BadDataType() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };
}

///////////////////////////////////
//            PPMFile            //
///////////////////////////////////

/**
 * the class is used to represent a ppm picture / file
 */
template <typename T> class PPMFile {
public:
  /**
   * Default constructor
   */
  PPMFile() {
    magic_number_ = 0;
    color_depth_ = 0;
    mat_pic_ = new Matrix<Pixel<T> >();
  }

  /**
   * @param filename is the full path to the file that shall be read
   */
  PPMFile(const std::string &filename) {
    std::fstream file;

    file.open(filename.c_str(), std::ios::in|std::ios::binary);

    if(file.is_open()) {
      unsigned char *buffer;
      std::vector<unsigned char> *vec;

      file.seekg (0, file.end);
      file_size_ = file.tellg();

      buffer = new unsigned char [(int)file_size_];
      vec = new std::vector<unsigned char>((int)file_size_);

      file.seekg (0, file.beg);
      file.read(reinterpret_cast<char *>(buffer), (int)file_size_);
      file.close();

      for(unsigned int i = 0; i < (int)file_size_; ++i) {
        (*vec)[i] = buffer[i];
      }
      delete[] buffer;

      magic_number_ = 0;
      color_depth_ = 0;
      mat_pic_ = NULL;
      unsigned int vec_pos = 0;

      read(*vec, vec_pos);

      delete vec;
    } else {
      throw PPMFileExce::UnexpectedFileEnd("The file wasn't open ...");
    }
  }

  /**
   * Copy Constructor
   */
  PPMFile(const PPMFile& copy) {
    file_size_ = copy.get_file_size();
    magic_number_ = copy.get_magic_number();
    color_depth_ = copy.get_color_depth();
    mat_pic_ = new Matrix<Pixel<T> >(copy.get_mat_pic());
  }

  /**
   * Destructor
   */
  ~PPMFile() {
    delete mat_pic_;
  }

  /**
   * the function writes the data stored by the data vector in the file
   *
   *
   * @param filename is the full path to the file that shall be read
   */
  inline void write_to(const std::string &filename) const {
    std::fstream file;

    file.open(filename.c_str(), std::ios::out|std::ios::binary);
    file << to_string();
    file.close();
  }

  /**
   * @return returns the file as a string
   */
  std::string to_string() const {
    std::ostringstream ss;
    char bit_temp = 0;
    unsigned char bit_pos = 7;

    ss << 'P';
    ss << (int) magic_number_;
    ss << '\n';
    ss << mat_pic_->get_col_length();
    ss << ' ';
    ss << mat_pic_->get_row_length();
    ss << '\n';

    if(magic_number_ != 1 && magic_number_ != 4) {
      ss << color_depth_;
      ss << '\n';
    }

    Pixel<T> p;
    T temp;
    unsigned int ss_previous_length = 0;
    unsigned int line_length = (unsigned int)ss.tellp() + 70;
    for(unsigned int i = 0; i < mat_pic_->get_row_length(); ++i) {
      for(unsigned int j = 0; j < mat_pic_->get_col_length(); ++j) {
        p = mat_pic_->get_data(i, j);

        for(unsigned int k = 0; k < p.size(); ++k) {
          switch(magic_number_) {
          case 1:
            temp = p.get_pixel(k);

            if(temp > 1) {
              temp = 1;
            } else if(temp < 0) {
              temp = 0;
            }
            ss << (unsigned int) temp;

            if((unsigned int) ss.tellp() - ss_previous_length >= line_length) {
              if(line_length != 70) {
                line_length = 70;
              }
              ss << '\n';
              ss_previous_length = ss.tellp();
            }
            break;
          case 2:
          case 3:
            temp = p.get_pixel(k);

            if(temp > 255) {
              temp = 255;
            } else if(temp < 0) {
              temp = 0;
            }
            ss << (unsigned int) temp;
            ss << ' ';

            if((unsigned int) ss.tellp() - ss_previous_length >= line_length) {
              if(line_length != 70) {
                line_length = 70;
              }
              ss << '\n';
              ss_previous_length = ss.tellp();
            }
            break;
          case 4:            
            bit_temp |= ((int)mat_pic_->get_data(i, j).get_pixel(0)) << bit_pos;

            if(bit_pos == 0) {
              ss << bit_temp;
              bit_temp = 0;
              bit_pos = 7;
            } else {
              --bit_pos;
            }
            break;
          case 5:
          case 6:
            if(sizeof(T) > 1) {
              if(color_depth_ > 255) {
                unsigned char mask = (1<<8)-1;

                ss << ((((int)p.get_pixel(k)) >> 8) & mask);
                ss << ((((int)p.get_pixel(k))) & mask);
              } else {
                temp = p.get_pixel(k);

                if(temp > 255) {
                  temp = (unsigned char) 255;
                } else if(temp < 0) {
                  temp = (unsigned char) 0;
                }
                ss << (unsigned char) temp;
              }
            } else {
              temp = p.get_pixel(k);

              if(temp > 255) {
                temp = (unsigned char) 255;
              } else if(temp < 0) {
                temp = (unsigned char) 0;
              }
              ss << (unsigned char) temp;
            }
          }
        }
      }
    }
    if(bit_pos != 7) {
      ss << bit_temp;
    }
   return ss.str();
  }

  /**
   * @return the return value is the magic number of the ppm picture
   */
  inline unsigned char get_magic_number() const  {
    return magic_number_;
  }

  /**
   * @return the return value is the magic number of the ppm picture
   */
  inline void set_magic_number(unsigned char magic_number) {
    magic_number_ = magic_number; // TODO Exception
  }

  /**
   * @return the retrun value is the colour depth of the picture
   */
  inline unsigned int get_color_depth() const {
    return color_depth_;
  }

    /**
   * @return the retrun value is the colour depth of the picture
   */
  inline void set_color_depth(unsigned int color_depth) {
    color_depth_ = color_depth; // TODO Exception
  }

  /**
   * @return the return value is a vector that holds all pixels of the ppm file
   */
  inline Matrix<Pixel<T> > get_mat_pic() const {
    return *mat_pic_;
  }

  /**
   * @return the return value is a vector that holds all pixels of the ppm file
   */
  inline void set_mat_pic(const Matrix<Pixel<T> > &mat_pic, const int &color_depth) {
    delete mat_pic_;
    mat_pic_ = new Matrix<Pixel<T> >(mat_pic);

    color_depth_ = color_depth;

    if(mat_pic_->get_data(0, 0).size() > 1) {
      magic_number_ = 6;
    } else if(color_depth == 1) {
      magic_number_ = 4;
    } else {
      magic_number_ = 5;
    }
  }

  /**
   * Sets a vector of data that will be read as if it is a file
   * @param vec determines the vector the information is read from
   */
  inline void set_data(const std::vector<unsigned char> &vec) {
    unsigned int pos = 0;

    read(vec, pos);
  }

  /**
   * @return returns the amount of subpixels based on the magic number
   */
  inline unsigned int get_subpixels() const {
    return (magic_number_ == 3 || magic_number_ == 6) ? 3 : 1;
  }

  /**
   * @param size determines the file size of the PPM file
   */
  inline void set_file_size(const double &size) {
    file_size_ = size;
  }

  /**
   * @return returns the file size of the PPM file
   */
  inline double get_file_size() const {
    return file_size_;
  }

  /**
   * the assignment operator
   */
  PPMFile<T> operator=(const PPMFile<T>& copy) {
    if(this != &copy) {
      file_size_ = copy.get_file_size();
      magic_number_ = copy.get_magic_number();
      color_depth_ = copy.get_color_depth();
      delete mat_pic_;
      mat_pic_ = new Matrix<Pixel<T> >(copy.get_mat_pic());
    }
    return *this;
  }


protected:
  /**
   * The function will return the next valid character of a ppm file
   * if there is a comment (indicated by a '#') the complete line will be
   * ignored
   *
   *
   * @param       file is the file the char is read from
   * @return      the return value is the next valid character or if the file is
   *              not good 0 is returned
   */
  char next_valid_char( const std::vector<unsigned char> &vec,
                        unsigned int &vec_pos) {
    char buffer = vec.at(vec_pos);
    vec_pos++;

    switch(buffer) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'P':
      case ' ':
      case '\t':
      case '\n':
      case '\0':
        return buffer;
      case '#':
        while(buffer != '\n'){
          if(vec_pos >= vec.size()) {
            std::ostringstream ss;
            ss << "The file End was at Pos: " << vec_pos;

            throw PPMFileExce::UnexpectedFileEnd(ss.str());
          } else {
            buffer = vec.at(vec_pos);
            vec_pos++;
          }
        }
        return next_valid_char(vec, vec_pos);
      default:
        std::ostringstream ss;
        ss << "The unallowed character was: " << buffer << " at Pos: " 
        << vec_pos;
        throw PPMFileExce::BadHeader(ss.str());
    }
  }


  /**
   * the function checks if the character is a whitespace
   *
   *
   * @param  c is the character to check
   * @return   the retrun value is true if the character is a whitespace
   */
  bool is_whitespace(const char &c) {
    switch(c) {
      case ' ':
      case '\t':
      case '\n':
      case '\0':
        return true;
      default:
        return false;
    }
  }

  /**
   * the function checks if the character is a digit
   *
   *
   * @param  c is the character to check
   * @return   the return value is true if the character is a digit
   */
  bool is_digit(const char &c) {
    switch(c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return true;
      default:
        return false;
    }
  }

  /**
   * the function reads the header of the ppm file
   *
   *
   * @param file               is the file read from
   * @param data               is the vector the read characters are pushed in
   *                           to
   * @param header_part_length is the array that holds the position of the
 *                             different informations from the header
   */
  void read_header( const std::vector<unsigned char> &vec,
                    unsigned int &vec_pos,
                    unsigned int &width,
                    unsigned int &height) {
    bool skip_null = true;
    bool end_of_sequence = false;
    char buffer;
    unsigned char part_nr = 0;
    std::ostringstream ss;

    if(vec.size() != 0) {
      buffer = next_valid_char(vec, vec_pos);
      if(buffer != 'P') {
        throw PPMFileExce::BadHeader("No Magic Number");
      }
    }

    while(part_nr < 4 && vec_pos < vec.size()) {
      buffer = next_valid_char(vec, vec_pos);

      if(is_whitespace(buffer) && !skip_null) {
        end_of_sequence = true;
      } else if(!is_whitespace(buffer)) {
        skip_null = false;
      }

      if(!skip_null) {
        ss << buffer;

        if(end_of_sequence) {
          switch(part_nr) {
            case 0: magic_number_ = atoi(ss.str().c_str());
              if(magic_number_ < 1 || magic_number_ > 6) {
                ss.clear();
                ss.str("Magic number was: ");
                ss << (int)magic_number_;
                throw  PPMFileExce::BadHeader(ss.str());
              }
              break;
            case 1: width = atof(ss.str().c_str());
              break;
            case 2: height = atof(ss.str().c_str());
              if(magic_number_ == 1 || magic_number_ == 4) {
                part_nr ++;
              }
              break;
            case 3:; color_depth_ = atof(ss.str().c_str());
              if(color_depth_ < 1 || color_depth_ > 65535) {
                ss.clear();
                ss.str("Color depth was: ");
                ss << (int)magic_number_;
                throw  PPMFileExce::BadHeader(ss.str());
              }
          }

          part_nr++;
          end_of_sequence = false;
          skip_null = true;
          ss.clear();
          ss.str("");
        }
      }
    }
  } 

  /**
   * the function reads the mat_pic of a ppm file in binary form
   *
   *
   * @param file               is the file read from
   * @param data               is the vector the read characters are pushed in
   *                           to
   * @param header_part_length is the array that holds the position of the
   *                           different informations from the header
   */
  void read_binary( const std::vector<unsigned char> &vec,
                    unsigned int &vec_pos) {
    bool subpixel = magic_number_ == 6;
    bool one_byte = color_depth_ < 256; // TODO Find ppm with more than 255
    unsigned int buffer = 0;
    unsigned int mat_pos = 0;
    Pixel<T> pixel;

    if(sizeof(T) < 2 && !one_byte) {
      throw PPMFileExce::BadDataType();
    }

    if(subpixel) {
      pixel = Pixel<T>(3);
    } else {
      pixel = Pixel<T>(1);
    }

    for(unsigned int i = vec_pos; i < vec.size();) {
      if(subpixel) {
        if(one_byte) {
          for(int j = 0; j < 3; ++j, ++i) {
            pixel.set_pixel(j, vec.at(i));
          }
        } else {
          buffer = 0; 

          for(int j = 0; j < 3; ++j, ++i) {
            buffer |= vec.at(i);
            ++i;
            buffer <<= 8;
            buffer |= vec.at(i);
            ++i;
            pixel.set_pixel(0, buffer);

            buffer = 0;
          }
        }
      } else {
        if(one_byte) {
          if(magic_number_ != 4) {
            pixel.set_pixel(0, vec.at(i));
          } else {
            for(int j = 7; j >= 0; --j) {
              pixel.set_pixel(0, (0 != (int)(vec.at(i) & (1 << j))));
              if(j != 0) {
                mat_pic_->set_data(mat_pos, pixel);
                mat_pos++;
              }
            }
          }
          ++i;
        } else {
          buffer = 0;

          buffer |= vec.at(i);
          ++i;
          buffer <<= 8;
          buffer |= vec.at(i);
          ++i;

          pixel.set_pixel(0, buffer);
        }
      }
      mat_pic_->set_data(mat_pos, pixel);
      mat_pos++;
    }

    unsigned int last_row = mat_pos / mat_pic_->get_col_length();
    unsigned int last_col = mat_pos % mat_pic_->get_col_length();

    if(last_row - mat_pic_->get_row_length() != 0 && last_col != 0) {
      std::ostringstream ss;

      ss << "The source file for the PPMFile ended before the whole matrix was "
      << "filled. Last read Pixel:\nRow: " << last_row << "\tCol: " << last_col
      << "From:\nRows: " << mat_pic_->get_row_length() << "\tCols: "
      << mat_pic_->get_col_length() << "\n";

      throw PPMFileExce::UnexpectedFileEnd(ss.str());
    } 
  }

  /**
   * Reads a ASCII PPM body
   * @param vec     determines the vector the information is stored in 
   * @param vec_pos determines the position the "reader" is in the vector (file)
   */
  void read_ascii(const std::vector<unsigned char> &vec,
                  unsigned int &vec_pos) {
    bool skip_null = false;
    bool end_of_sequence = false;
    bool subpixel = magic_number_ == 3;
    char buffer = 0;
    unsigned char pixel_nr = 0;
    unsigned int mat_pos = 0;
    Pixel<T> pixel;
    std::ostringstream ss;

    if(subpixel) {
      pixel = Pixel<T>(3);
    } else {
      pixel = Pixel<T>(1);
    }

    while(vec_pos < vec.size()) {
      buffer = next_valid_char(vec, vec_pos);

      if(is_whitespace(buffer)) {
        if(!skip_null) {
          end_of_sequence = true;
        }
      } else {
        skip_null = false;
      }

      if(!skip_null) {
        ss << buffer;

        if(end_of_sequence || magic_number_ == 1) {
          pixel.set_pixel(pixel_nr, atof(ss.str().c_str()));
          pixel_nr++;
          ss.clear();
          ss.str("");
          skip_null = true;
          end_of_sequence = false;

          if((pixel_nr == 3 && subpixel) || (pixel_nr == 1 && !subpixel)) {
            mat_pic_->set_data(mat_pos, pixel);
            pixel_nr = 0;
            mat_pos ++;
          }
        }
      }
    }

    unsigned int last_row = mat_pos / mat_pic_->get_col_length();
    unsigned int last_col = mat_pos % mat_pic_->get_col_length();

    if(last_row - mat_pic_->get_row_length() != 0 && last_col != 0) {
      std::ostringstream ss;

      ss << "The source file for the PPMFile ended before the whole matrix was "
      << "filled. Last read Pixel:\nRow: " << last_row << "\tCol: " << last_col
      << "From:\nRows: " << mat_pic_->get_row_length() << "\tCols: "
      << mat_pic_->get_col_length() << "\n";

      throw PPMFileExce::UnexpectedFileEnd(ss.str());
    } 
  }

  /*
   * the function reads the the ppm file.
   * it checks if the file is a ppm file by looking at the "magic number"
   *
   *
   * @param file               is the file read from
   * @param data               is the vector the read characters are pushed in
   *                           to
   * @param header_part_length is the array that holds the position of the 
   *                           different informations from the header
   */
  void read(const std::vector<unsigned char> &vec, unsigned int &vec_pos) {

    unsigned int width = 0;
    unsigned int height = 0;

    read_header(vec, vec_pos, width, height);

    Matrix<Pixel<T> > *temp = mat_pic_;
    mat_pic_ = new Matrix<Pixel<T> >(height, width);

    if(magic_number_ < 4) {
      read_ascii(vec, vec_pos);
    } else {
      read_binary(vec, vec_pos);
    }
    delete temp;
  }

  double file_size_;
  Matrix<Pixel<T> > *mat_pic_;
  unsigned char magic_number_;
  unsigned int color_depth_;
};
#endif
