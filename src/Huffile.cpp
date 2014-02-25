#include <sstream>
#include <fstream>

#include "Huffile.hpp" 


/**
 * default constructor
 */
Huffile::Huffile(){ 
  dec_file_size_ = 0;
  enc_file_size_ = 0;
  encoded_data_ = new std::string("");
  decoded_data_ = new std::string("");
  encoding_ = true;
}

/**
 * Constructor with a complete path to a file
 *
 *
 * @param filename complete path to a file
 * @param encoding if true the file will be encoded if false the file will be
 *                 decoded
 */
Huffile::Huffile(const std::string &filename, const bool &encoding) {
  dec_file_size_ = 0;
  enc_file_size_ = 0;
  encoded_data_ = new std::string("");
  decoded_data_ = new std::string("");
  encoding_ = true;

  encoding_ = encoding;
  std::fstream file;
  file.open(filename.c_str(), std::ios::in|std::ios::binary);

  if (!file.is_open()) {
    std::cout << "File isn't open!" << std::endl; //TODO Throw exception
  } else {
    file.seekg (0, file.end);
    unsigned int length = file.tellg();
    file.seekg (0, file.beg);
    unsigned char * file_buffer = new unsigned char [length];

    file.read(reinterpret_cast<char *>(file_buffer), length);
    file.close();

    std::vector<unsigned char> *vec = new std::vector<unsigned char>();
    for(unsigned int i = 0; i < length; ++i) {
      vec->push_back(file_buffer[i]);
    }
    delete[] file_buffer;

    if(encoding) {
      encode(*vec);
    } else {
      decode(*vec);
    }
    delete vec;
  }
}

/**
 * Copy constructor
 */
Huffile::Huffile(const Huffile &copy) {
  dec_file_size_ = copy.get_encoded_data_size();
  enc_file_size_ = copy.get_decoded_data_size();
  encoded_data_ = new std::string(copy.get_encoded_data());
  decoded_data_ = new std::string(copy.get_decoded_data());
  encoding_ = copy.get_mode();
}


/**
 * Destructor
 */
Huffile::~Huffile() {
  delete encoded_data_;
  delete decoded_data_;
}
/**
 * the function will write the encoded data in the file
 *
 *
 * @param filename is the full path to the destination file
 */
void Huffile::write_encoded_to(const std::string &filename) const {
  write_to(filename, *encoded_data_);
}

/**
 * the function will write the decoded data in the file
 *
 *
 * @param filename is the full path to the destination file
 */
void Huffile::write_decoded_to(const std::string &filename) const {
  write_to(filename, *decoded_data_);
}

/**
 * the function sets the mode the object is in
 * there is no effect until data is inserted
 *
 * when true new data is encoded
 * when false new data is decoded
 * Default is true
 *
 *
 * @param encoding either true for encoding or false for decoding
 */
void Huffile::set_mode(const bool &encoding) {
  encoding_ = encoding; 
}

/**
 * @return returns the mode the Huffile is in - true encoding, false decoding
 */
bool Huffile::get_mode() const {
  return encoding_;
}

/**
 * the function will set the the data to be processed
 * the right mode might need to be set - the default mode is encode
 *
 *
 * @param data_vector is a vector that holds all characters of the file to be 
 *                    processed
 */
void Huffile::set_data(std::vector<unsigned char> data_vector) {
  if(encoding_) {
    encode(data_vector);
  } else {
    decode(data_vector);
  }
}

/**
 * @return the return value is the encoded data
 */
std::string Huffile::get_encoded_data() const {
  return *encoded_data_;
}

/**
 * @return the return value is the decoded data
 */
std::string Huffile::get_decoded_data() const {
  return *decoded_data_;
}

/**
 * @return the return value is the file size of the decoded / original file
 */
double Huffile::get_decoded_data_size() const {
  return dec_file_size_;
}

/**
 * @return the return value is the file size of the encoded file
 */
double Huffile::get_encoded_data_size() const {
  return enc_file_size_;
}

Huffile Huffile::operator=(const Huffile &copy) {
  if(this != &copy) {
    dec_file_size_ = copy.get_encoded_data_size();
    enc_file_size_ = copy.get_decoded_data_size();
    delete encoded_data_;
    encoded_data_ = new std::string(copy.get_encoded_data());
    delete decoded_data_;
    decoded_data_ = new std::string(copy.get_decoded_data());
    encoding_ = copy.get_mode();
  }
  return *this;
}

/**
 * the function will write in the file the string.
 *
 *
 * @param filename is the full path to the destination file
 * @param out      is the string to be written in the file
 */
void Huffile::write_to( const std::string &filename,
                        const std::string &out) const {
  std::fstream file;

  file.open(filename.c_str(), std::ios::out|std::ios::binary);
  // file.write(out.c_str(), out.size());
  file << out;
  file.close();
}  

/**
 * the function encodes the file, read into the char array, by applying the Huffman algorithm
 *
 * @param vec          is a vector that holds all characters from the
 *                     original file
 */
void Huffile::encode(const std::vector<unsigned char> &vec) {
  super::set_data_vector(vec);
  make_decoding_header();

  std::ostringstream ss;

  for(unsigned int i = 0; i < vec.size(); ++i) {
   ss << vec.at(i);
  }

  decoded_data_->append(ss.str());
  encoded_data_->append(super::get_encoded_data());

  dec_file_size_ = sizeof(char) * vec.size() / (double)FILESIZE;
  enc_file_size_ =  sizeof(char) * (encoded_data_->size() + 1) 
                    / (double)FILESIZE;
}

/**
 * the function will decode the file
 *
 * @param vec  is a vector that holds all characters from the original file
 * @param pos  is the position in the vector
 */
void Huffile::decode(const std::vector<unsigned char> &vec) {
  unsigned char last_bit;
  unsigned int pos = 0;
  unsigned char *file_buffer = new unsigned char[vec.size()];
  DecodeMap dec_map = DecodeMap();

  decode_header(vec, pos, dec_map, last_bit);
  decode_body(vec, pos, dec_map, last_bit);

  for(unsigned int i = 0; i < vec.size(); ++i) {
    file_buffer[i] = vec.at(i);
  }

  encoded_data_->append(reinterpret_cast<char *>(file_buffer));

  dec_file_size_ =  sizeof(char) * (decoded_data_->size() + 1) 
                    / (double)FILESIZE;
  enc_file_size_ = sizeof(char) * vec.size() / (double)FILESIZE;

  delete[] file_buffer;
}

/**
 * the function decodes the header, that was written on top of the encoded file
 *
 * the format has to be like this:
 * 
 *  the very first number has to be the length of the last Byte e.g. how many
 *  Bits are used of it 1-8 as plain character
 *  
 *    afterwards the sequence is repetitive:
 *    
 *      length of the binary (Bit) code of the character (written in characters)
 *      with a '#' at the end of the number sequence
 *      
 *      the '#' is followed by the binary code
 *      
 *      the binary code is followed by the character the code shall represent
 *      
 *  the end of the header marks a new line
 *
 *
 * @param vec         is a vector that holds all characters from the
 *                    original file
 * @param pos         is the position in the vector
 * @param dec_map     is a Map which will have the all information needed to
 *                    decode the body of the file
 * @param last_bit    will hold how many Bits are used from the last Byte of the
 *                    body
 */
void Huffile::decode_header(const std::vector<unsigned char> &vec,
                            unsigned int &pos,
                            Huffile::DecodeMap &dec_map,
                            unsigned char &last_bit) {
  bool header_end = false;
  int bits_to_read = 0;
  unsigned char buffer;
  std::vector<bool> temp_vec;
  last_bit = vec.at(pos++) - '0';

  while(!header_end) {
    temp_vec = std::vector<bool>();
    bits_to_read = next_number(vec, pos); // Address length

    if(bits_to_read != -1) {
      while(bits_to_read > 0){                              // Address
        buffer =  vec.at(pos++);

        for(int i = 7; i >= 0 && bits_to_read > 0; i--, bits_to_read--) {
          // 1 shifted left by i Bits with an binary and with the char -
          // converted to int e.g. either 0, 2, 4, 8, 16, 32, 64, or 128
          temp_vec.push_back(0 != (int)(buffer & (1 << i))); 
        }
      }
      dec_map[temp_vec] = vec.at(pos++);        // get represented char
    } else {
      header_end = true;
    }
  }
}

/**
 * the function will decode the body of the encoded file
 * the format of the file is a Bit sequence with one address followed directly
 * by another - any address that appears has to be in the header
 *
 *
 * @param vec          is a vector that holds all characters from the
 *                     original file
 * @param pos          is the position in the vector
 * @param dec_map      is a Map which will have the all information needed to 
 *                     decode the body of the file
 * @param last_bit     is the number of how many Bits are used from the last 
 *                     Byte of the body
 */
void Huffile::decode_body(const std::vector<unsigned char> &vec,
                          unsigned int &pos,
                          Huffile::DecodeMap &dec_map,
                          const unsigned char &last_bit) {
  unsigned char buffer;
  std::ostringstream ss;
  std::vector<bool> temp_vec = std::vector<bool>();

  while(pos < vec.size()){
    buffer = vec.at(pos++);

    for(int i = 7; i >= 0 && (pos < vec.size() || i >= last_bit); i--) {
      // 1 shifted left by i Bits with an binary and with the char - converted
      // to int e.g. either 0, 2, 4, 8, 16, 32, 64, or 128
      temp_vec.push_back(0 != (int)(buffer & (1 << i))); 

      if(dec_map.find(temp_vec) != dec_map.end()){
        ss << dec_map[temp_vec];

        temp_vec.clear();
      }
    }
  }

  if(dec_map.find(temp_vec) != dec_map.end()) {
    ss << dec_map[temp_vec]; // Write last character
  }
  *decoded_data_ += ss.str();
}

/**
 * the function crates the header of the encodes file
 *
 * the format is:
 *  the very first number is the length of the last Byte e.g. how many Bits are
 *  used of it 1-8 as plain character
 *  
 *    afterwards the sequence is repetitive:
 *    
 *      length of the binary (Bit) code of the character (written in characters)
 *      with a '#' at the end of the number sequence
 *      
 *      the '#' is followed by the binary code
 *      
 *      the binary code is followed by the character the code shall represent
 *      
 *  the end of the header marks a new line
 */
void Huffile::make_decoding_header() {
  std::ostringstream ss;
  unsigned char buffer;
  signed char bits;
  ss << (int)last_bit_;

  for(Huffile::EncodeMap::iterator i = encode_map_->begin();
    i != encode_map_->end(); i++) {

    buffer = 0;
    bits = 7;

    ss << i->second.size();
    ss << '#';
    for(unsigned int j = 0; j < i->second.size(); j++) {
      if(bits < 0) {
        ss << buffer;
        buffer = 0;
        bits = 7;
      }
      // the Bit is shifted bits times to the left and set with an binary or
      buffer |= i->second.at(j) << bits; // bool = Bit 
      bits--;
    }
    ss << buffer;
    ss << i->first;
  }
  ss << '\n';

  *encoded_data_ += ss.str();
}

/**
 * the function returns the next number in the char array that is constructed of
 * the numbers 0-9 as character the sequence of the numbers from 0-9 may not be
 * broken with any other characters than a '#'
 *
 *
 * @param vec         is a vector that holds all characters from the
 *                    original file
 * @param pos         is the position in the vector
 * @return            the return value is the next number in the char array that
 *                    is constructed of the numbers 0-9 as character
 */
int Huffile::next_number( const std::vector<unsigned char> &vec,
                          unsigned int &pos) const {
  char buffer = 0;
  int ret = 0;

  while(buffer != '#' && pos < vec.size()) { 
    buffer = vec.at(pos++);
    if(buffer - '0' >= 0 && buffer - '0' < 10) {
      ret *= 10;
      ret += buffer - '0';
    } else if(buffer == '#') {  // End of the number
      return ret;
    } else {                    // The read character was not a number and not #
      return -1;
    }
  }
  return ret;
}
