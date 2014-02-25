#ifndef Huffile_HPP
#define Huffile_HPP

#include <map>
#include <string>

#include "Huffman.hpp"

/**
 * The class is used for a Huffman encoding / decoding of files
 */
class Huffile: public Huffman<unsigned char> {
public:
  typedef std::map<unsigned char, std::vector<bool> > EncodeMap;
  typedef std::map<std::vector<bool>, unsigned char> DecodeMap;
  typedef Huffman super;

  Huffile();
  Huffile(const std::string &filename, const bool &encode);
  Huffile(const Huffile &copy);
  ~Huffile();
  void write_encoded_to(const std::string &filename) const;
  void write_decoded_to(const std::string &filename) const;
  void set_mode(const bool &encoding);
  bool get_mode() const;
  void set_data(std::vector<unsigned char> data_vector);
  std::string get_encoded_data() const;
  std::string get_decoded_data() const;
  double get_decoded_data_size() const;
  double get_encoded_data_size() const;
  Huffile operator=(const Huffile &copy);


private:
  void write_to(const std::string &filename, const std::string &out) const;
  void encode(const std::vector<unsigned char> &data_vector);
  void decode(const std::vector<unsigned char> &data_vector);
  void decode_header( const std::vector<unsigned char> &vec, 
                      unsigned int &pos,
                      DecodeMap &dec_map,
                      unsigned char &last_bit);
  void decode_body( const std::vector<unsigned char> &vec,
                    unsigned int &pos,
                    DecodeMap &dec_map,
                    const unsigned char &last_bit);
  void make_decoding_header();
  int next_number(const std::vector<unsigned char> &vec,
                  unsigned int &pos) const;

  static const int FILESIZE = 1; // Byte = 1 KB = 1024 MB = 1024000
  bool encoding_;
  double dec_file_size_;
  double enc_file_size_;
  std::string *decoded_data_;
  std::string *encoded_data_;
};
#endif
