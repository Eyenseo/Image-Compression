#include <iostream>

#include "DCTFile.hpp"
#include "Huffile.hpp"

// Default constructor
DCTFile::DCTFile() : DCT() {
  file_size_ = 0;
}

// Constructor for a image matrix and a color depth
DCTFile::DCTFile(const Matrix<Pixel<double> > &data, const unsigned int &color_depth): DCT(data, color_depth) {
  file_size_ = 0; 
}

//constructor to read a DCTFile file and decode it 
DCTFile::DCTFile(const std::string &filename) : DCT() {
  file_size_ = 0;

  Huffile *h = new Huffile(filename, false);
 
  parse(h->get_decoded_data());

  delete h;
}

// Destructor
DCTFile::~DCTFile() {
}

//makes a string of the File - not readable for the human
std::string DCTFile::to_string() {
  std::ostringstream ss;
  unsigned char subpixels = mat_pic_->get_data(0,0).size();

  ss << "Humdi"
  << super::get_quality()
  << subpixels << get_color_depth() << ' '
  << mat_pic_->get_row_length() << ' ' << mat_pic_->get_col_length() << ' ';
  
  for(unsigned int i = 0, i_end = quantization_->get_row_length();
      i < i_end; ++i) {
    for(unsigned int j = 0, j_end = quantization_->get_col_length();
        j < j_end; ++j) {
      ss << quantization_->get_data(i, j);
    }
  }

  //TODO replace with real ZigZag
  signed char last_data = 0;
  signed char next_data = 0; 
  unsigned char data_count = 0;
  unsigned int row_pos = 0;
  unsigned int row_length = mat_pic_->get_row_length();
  unsigned int col_pos = 0;
  unsigned int col_length = mat_pic_->get_col_length();
  Pixel<double> pixel;
  
  for(unsigned int i = 0; i < row_length; i += 8) {
    for(unsigned int j = 0; j < col_length; j += 8) {
      for(unsigned int k = 0; k < 8; ++k) {
        for(unsigned int l = 0; l <= k; ++l) {
          row_pos = i + k - l;
          col_pos = j + l;

          if(row_pos < row_length && col_pos < col_length) { 
            pixel = mat_pic_->get_data(row_pos, col_pos);

            for(unsigned char m = 0; m < subpixels; ++m) {
              next_data = pixel.get_pixel(m);

              if(next_data == last_data && data_count < 255) {
                ++data_count;
              } else {
                if(data_count != 0) {
                  ss << data_count << last_data;
                }
                data_count = 1;
                last_data = next_data;
              }
            }
          }
        }
      }
      for(unsigned int k = 1; k < 8; ++k) {
        for(unsigned int l = 0; l < 8 - k; ++l) {
          row_pos = i + 7 - l;
          col_pos = j + l + k;

          if(row_pos < row_length && col_pos < col_length) { 
            pixel = mat_pic_->get_data(row_pos, col_pos);

            for(unsigned char m = 0; m < subpixels; ++m) {
              next_data = pixel.get_pixel(m);

              if(next_data == last_data && data_count < 255) {
                ++data_count;
              } else {
                if(data_count != 0) {
                  ss << data_count << last_data;
                }
                data_count = 1;
                last_data = next_data;
              }
            }
          }
        }
      }
    }
  } 
  if(data_count != 0) {
    ss << data_count << last_data;
    data_count = 0;
    last_data = next_data;
  }
  return ss.str();
}

// writes the encoded file to the location
void DCTFile::write_to(const std::string &filename = "Humdidum.humdi") {
  std::string buffer = to_string();
  std::vector<unsigned char> *vec = new std::vector<unsigned char>();
  
  for(unsigned int i = 0; i < buffer.size(); ++i) {
    vec->push_back(buffer.at(i));
  }
  
  Huffile *h = new Huffile();
  h->set_data(*vec); 
  h->write_encoded_to(filename);
  file_size_ = h->get_encoded_data_size();

  delete vec;
  delete h;
} 

// parses the file (the string holds all data of the file)
void DCTFile::parse(const std::string &s) {
  char buffer = 0;
  unsigned char subpixels;
  unsigned int rows;
  unsigned int cols;
  unsigned int pos = 5;
  std::ostringstream ss;

  if(s.compare(0, pos, "Humdi") != 0) {
    throw DCTFileExce::BadHeader("In DCTFile::parse");
  }  
  super::set_quality(s.at(pos));
  ++pos;
  subpixels = s.at(pos);
  ++pos;

  while(buffer != ' ') {
    if(buffer != 0) {
      ss << buffer;
    }
    buffer = s.at(pos++);
  }
  set_color_depth(atof(ss.str().c_str()));
  ss.clear();
  ss.str("");
  buffer = 0;

  while(buffer != ' ') {
    if(buffer != 0) {
      ss << buffer;
    }
    buffer = s.at(pos++);
  }
  rows = atof(ss.str().c_str());
  ss.clear();
  ss.str("");
  buffer = 0;

  while(buffer != ' ') {
    if(buffer != 0) {
      ss << buffer;
    }
    buffer = s.at(pos++);
  }
  cols = atof(ss.str().c_str());

  delete quantization_;
  quantization_ = new Matrix<unsigned char>(8);

  for(unsigned int i = 0; i < 8; ++i) {
    for(int j = 0; j < 8; ++j) {
      quantization_->set_data(i, j, s.at(pos++)); 
    }
  }  

  delete mat_pic_;
  mat_pic_ = new Matrix<Pixel<double> >(rows, cols);


  signed char data = 0;
  unsigned char data_count = 0;
  unsigned int row_pos = 0;
  unsigned int col_pos = 0;
  Pixel<double> p = Pixel<double>(subpixels);

  for(unsigned int i = 0; i < rows; i += 8) {
    for(unsigned int j = 0; j < cols; j += 8) {
      for(unsigned int k = 0; k < 8; ++k) {
        for(unsigned int l = 0; l <= k; ++l) {
          row_pos = i + k - l;
          col_pos = j + l;

          if(row_pos < rows && col_pos < cols) { 
            for(unsigned char m = 0; m < subpixels; ++m) {
              if(data_count == 0) {
                data_count = s.at(pos++);
                data = s.at(pos++);
              }
              p.set_pixel(m, data);
              --data_count;
            }
            mat_pic_->set_data(row_pos, col_pos, p);
          }
        }
      }
      for(unsigned int k = 1; k < 8; ++k) {
        for(unsigned int l = 0; l < 8 - k; ++l) {
          row_pos = i + 7 - l;
          col_pos = j + l + k;
         
          if(row_pos < rows && col_pos < cols) { 
            for(unsigned char m = 0; m < subpixels; ++m) {
              if(data_count == 0) {
                data_count = s.at(pos++);
                data = s.at(pos++);
              }
              p.set_pixel(m, data);
              --data_count;
            }
            mat_pic_->set_data(row_pos, col_pos, p);
          }
        }
      }
    }
  } 
}

/**
 * @return the return value is the file size of the encoded file
 */
double DCTFile::get_file_size() const {
  return file_size_;
}
