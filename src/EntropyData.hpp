#ifndef ENTROPYSET_HPP
#define ENTROPYSET_HPP

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

/**
 * the class provides a container for data that is part of a entropy conversion
 */
template<typename T> class EntropyData {
public:
  /**
   * the function calculates the entropy based on the previously calculated possibility (weight) and the information content
   *
   *
   * @param  vec is the vector the information for the calculation is retrieved from
   * @return     the return value is the entropy of the data
   */
  static double calculate_entropy(std::vector<EntropyData<T>*> &vec) {
    double ret = 0.0;

    for(unsigned int i = 0; i < vec.size(); i++) {
      ret += vec.at(i)->get_possibility() * vec.at(i)->get_information_content();
    }

    return ret;
  };

  /**
   * constructor
   */
  EntropyData() {
    data_ = 0;
    digit_ = 0;
    upper_ = NULL;

    quantity_ = 0;
    data_count_ = 0;
    possibility_ = 0;
    information_content_ = 0;
  };

  /**
   * destructor
   */
  ~EntropyData() {};

  /**
   * @return the return value is the data the object represents
   */
  inline T get_data() const {
    return data_;
  };

  /**
   * @param data data to be set as new data
   */
  inline void set_data(T data) {
    data_ = data;
  };

  /**
   * sets the digit (either 0 or 1)
   *
   *
   * @param digit 0 or 1 / false or true
   */
  inline void set_digit(const bool &digit){
    digit_ = digit;
  };

  inline bool get_digit() const {
    return digit_;
  }

  /**
   * the function calculates the code of the object
   *
   *
   * @return the return value is the code of the object
   */
  std::vector<bool> calculate_code() {
    std::vector<bool> temp = std::vector<bool>();

    calculate_code(temp);

    return temp;
  };

  /**
   * @return the return value is the upper EntropyData object pointer
   */
  inline EntropyData *get_upper() {
    return upper_;
  };

  /**
   * @param upper sets the upper EntropyData object pointer
   */
  inline void set_upper(EntropyData *upper) {
    upper_ = upper;
  };

  /**
   * @return the return value is the quantity of the data the object represents
   */
  inline unsigned int get_quantity() const {
    return quantity_;
  };

  /**
   * @param quantity sets the quantity of the data the object represents
   */
  inline void set_quantity(int quantity) {
    quantity_ = quantity;
    calculate_values();
  };

  /**
   * @param value changes the quantity by the value
   */
  inline void change_quantity_by(int value) {
    quantity_ += value;
    calculate_values();
  };

  /**
   * @return the return value is the data count (how many data exists - 
   *         including doubles)
   */
  inline int get_data_count() const {
    return data_count_;
  };

  /**
   * @param data_count sets the data count
   */
  inline void set_data_count(int data_count) {
    data_count_ = data_count;
    calculate_values();
  };

  /**
   * @return the return value is the possibility (weight) of the data the object
   *         represents
   */
  inline double get_possibility() const {
    return possibility_;
  };

  /**
   * @return the return value is the information content of the data the object 
   *         represents
   */
  inline double get_information_content() const {
    return information_content_;
  };

private:
  /**
   * the method calculates the possibility and the information content of the 
   * data the object represents
   */
  void calculate_values() {
    if(data_count_ != 0) {
      possibility_ = (double) quantity_ / (double) data_count_;
      information_content_ = log2((1.0 / possibility_));
    }
  };

  /**
   * the function calculates the code of the object
   *
   *
   * @param temp is a vector that holds the code of the recursively call
   */
  void calculate_code(std::vector<bool> &temp) {
    if(upper_ != NULL && upper_->get_upper() != NULL) {
      upper_->calculate_code(temp);
    }
    temp.push_back(digit_);
  };

  // data the object represents
  T data_;
  // either 0 or 1 / left or right
  bool digit_;
  // Needed to call the upper dataset to get get it's code
  EntropyData *upper_;
  // quantity of the data the object represents
  unsigned int quantity_;
  // data count (how many data exists - including doubles)
  int data_count_;
  // possibility (weight) of the data the object represents
  double possibility_;
  // information content of the data the object represents
  double information_content_;
};
#endif
