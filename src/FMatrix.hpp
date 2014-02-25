#ifndef FMATRIX_H
#define FMATRIX_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Matrix.hpp"

namespace FMatrixExce{
  class FileNotOpen : public std::exception {
   public:
    FileNotOpen(std::string s = "") {
      msg = s + "\nFile was not open!\n";
    }

    ~FileNotOpen() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };

  class MatrixFileIncomplete : public std::exception {
   public:
    MatrixFileIncomplete(std::string s = "") {
      msg = s + "\nFile was incomplete!\n";
    }

    ~MatrixFileIncomplete() throw() {
    }

    const char* what() const throw() {
      return msg.c_str();
    }

   private:
    std::string msg;
  };
}

// The class provides the option to create a Matrix from a txt file, or write a
// Matrix to one
// The class needs some serious rework on Exceptions for unexpected use!
template <typename T> class FMatrix: public Matrix<T> {
 public:
  typedef Matrix<T> super;

  // Constructor tries to read matrix information from a file
  // string file - name of file
  FMatrix(const std::string &filename) : Matrix<T>() {
    read_file(filename);
  }

  // Default Constructor 0 x 0
  FMatrix() : Matrix<T>() {
  }

  // Specific Size Constructor m x m
  FMatrix(int m)  : Matrix<T>(m) {
  }

  // Specific Size Constructor m x n
  FMatrix(int m, int n, T val = T())  : Matrix<T>(m, n, val) {
  }

  // Destructor
  ~FMatrix() {};

  // returns the matrix as a string - readable for a human
  std::string to_string() const {
    std::ostringstream ss;

    for(unsigned int m = 0; m < super::get_row_length(); m++) {
      for(int n = 0; n < super::get_col_length(); n++) {
        ss << super::get_data(m, n) << ' ';
      }
      ss << '\n';
    }
    return ss.str();
  }

  // writes the matrix to a file - readable for a human
  // string s - Defines the name of the .txt file
  void write_to(const std::string &s = "matrix") {
    std::ofstream out (s.c_str());
    out << to_string();
  }

 private:
  // reads the file and runs parse afterwards 
  inline void read_file(const std::string &filename) {
    std::fstream file;
    char *buffer = NULL;

    file.open(filename.c_str(), std::ios::in|std::ios::binary);

    if(file.is_open()) {
      unsigned int length;

      file.seekg (0, file.end);
      length = file.tellg();

      buffer = new char[length];

      file.seekg (0, file.beg);
      file.read(buffer, length);
      file.close();

      parse(buffer, length);

      delete[] buffer;
    } else {
      delete[] buffer;
      throw FMatrixExce::FileNotOpen();
    }
  }

  // parses the array, and creates a 2D Vector
  void parse(const char *file_buffer, const unsigned int &length) {
    unsigned int file_pos = 0;
    unsigned int row_length = 1;
    unsigned int col_length = 1;
    unsigned int col_position = 0;
    char buffer;    
    std::ostringstream number_buffer;

    super::set_size(row_length, col_length);

    while(file_pos < length) {
      buffer = next_valid_char(file_buffer, file_pos);
      
      if(is_whitespace(buffer)) {
        if(row_length == 1 && col_position == col_length) {
          ++col_length;

          super::set_size(row_length, col_length);
        } else if(row_length != 1 && col_position == col_length) {
          std::ostringstream ss;

          ss << "There was an additional column at Row: " << row_length << ". "
          << "The Matrix has to be even e.g. The first Row has to have the same"
          << " amount of columns as the rest of the Matrix.";
          throw FMatrixExce::MatrixFileIncomplete(ss.str());
        }

        super::set_data(row_length - 1,
                        col_position,
                        atof(number_buffer.str().c_str()));

        ++col_position;
        number_buffer.clear();
        number_buffer.str("");
        
        if(buffer == '\n' && file_pos+1 < length) {
          ++row_length;
          col_position = 0;

          super::set_size(row_length, col_length);
        }
      } else {
        number_buffer << buffer;
      }
    }
  } 

  // returns the next valid character
  char next_valid_char(const char *file_buffer, unsigned int &file_pos) const {
    char buffer = file_buffer[file_pos];
    file_pos++;

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
      case ' ':
      case '\t':
      case '\n':
      case '\0':
        return buffer;
      default:
        std::ostringstream ss;
        ss << "There was an unallowed character: " << buffer << " at Pos: " 
        << file_pos;
        throw FMatrixExce::MatrixFileIncomplete(ss.str());
    }
  }

  // returns true if the char is a whitespace
  bool is_whitespace(const char &c) const {
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

  // returns true if the char is a number
  bool is_digit(const char &c) const {
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
};
#endif
