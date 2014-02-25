#ifndef DCTFILE_HPP
#define DCTFILE_HPP 

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <exception>

#include "DCT.hpp"

namespace DCTFileExce {
  class BadHeader : public std::exception {
   public:

    BadHeader() {
      msg =  "Bad header!\n";
    }

    BadHeader(const std::string &s) {
      msg =  "Bad header!\n" + s + '\n';
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
      msg =  "File ended unexpected!\n";
    }

    UnexpectedFileEnd(const std::string &s) {
      msg =  "File ended unexpected!\n" + s + '\n';
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
      msg =  "Bad data type of DCTFile!\n";
    }

    BadDataType(const std::string &s) {
      msg =  "Bad data type of DCTFile!\n" + s + '\n';
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
// the class provides the ability to write and read a file that was created by 
// transforming an image matrix
class DCTFile : public DCT {
public:
  typedef DCT super;

  DCTFile();
  DCTFile(const std::string &filename);
  DCTFile(const Matrix<Pixel<double> > &data, const unsigned int &color_depth);
  ~DCTFile();

  std::string to_string();
  void write_to(const std::string &filename);
  double get_file_size() const;

private:
  void parse(const std::string &s);
  unsigned int color_depth_;
  double file_size_;
};

#endif
