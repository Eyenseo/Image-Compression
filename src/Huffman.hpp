#ifndef Huffman_HPP
#define Huffman_HPP

#include <cstring>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
// #include <sys/time.h>

#include "EntropyData.hpp"
#include "BinaryTree.hpp"

/**
 * the class provides the Huffman entropy Encoding algorithm
 */
template <typename T> class Huffman {
public:
  typedef std::map<T, EntropyData<T>*> EntropyDataMap;
  typedef typename std::map<T, EntropyData<T>*>::iterator EntropyDataMapInter;
  typedef std::map<T, std::vector<bool> > EncodeMap;
  typedef BinaryTree<EntropyData<T> > TreeDataSet;
  typedef std::vector<BinaryTree <EntropyData<T> >*> TreeDataSetVector;

  /**
   * Default constructor
   */
  Huffman() {
    encode_map_ = new EncodeMap();
    encoded_data_ = NULL;
    data_vector_ = NULL;
    tree_ = NULL;

    entropy_rate_ = 0;
    redundancy_ = 0;
  };

  /**
   * Destructor
   */
  ~Huffman() {
    delete encode_map_;
    delete encoded_data_;
    delete data_vector_;
    delete tree_;
  };

  /**
   * encodes the data that is stored in the vector - each vector element is seen
   * as a single element
   *
   *
   * @param data_vector is the vector the data is read from
   */
  void set_data_vector(const std::vector<T> &data_vector) {
    this->data_vector_ = new std::vector<T>(data_vector);

    make_char_code_map();
    encode_data();
  };

  /**
   * @return the return value is the entropy rate of the original data
   */
  double get_entropy_rate() const {
    return entropy_rate_;
  };

  /**
   * @return the return value is the redundancy of the original data
   */
  double get_redundancy() const {
    return redundancy_;
  };

  /**
   * @return the return value is a string that consists of ones and zeros and
   * represents a the binary encoded original data
   */
  std::string get_encoded_data() const {
    return *encoded_data_;
  };


  /**
   * @return the return value is the encode map
   */
  EncodeMap get_encode_map() const {
    return *encode_map_;
  }

  /**
   * @return returns the vector that stores all data
   */
  std::vector<T> get_data_vector() const {
    return *data_vector_;
  }

  /**
   * @return returns the binary tree 
   */
  TreeDataSet get_binary_tree()  const {
    return *tree_;
  }

protected:
  /**
   * the method converts the data in the data vector to a map that contains only
   * single EntropyData objects
   *
   *
   * @return the return value is a Map - the key is a character and the value is
   *             a EntropyData object that contains all information of the 
   *             character in the file
   */
  void data_to_entropy_data_set_map(EntropyDataMap &map) {
    EntropyData<T> *e;
    T temp_data;
    unsigned int data_count = data_vector_->size();

    for(unsigned int i = 0; i < data_count; i++) {
      temp_data = data_vector_->at(i);
      if(map.find(temp_data) ==  map.end()) {
        e = new EntropyData<T>();
        e->set_data(temp_data);
        e->set_quantity(1);
        e->set_data_count(data_count);
        map[temp_data] = e;
      } else {
        map[temp_data]->change_quantity_by(1);
      }
    }
  };

  /**
   * the method calculates the entropy of the data
   *
   *
   * @param map is the map that is made by data_to_entropy_data_set_map()
   */
  void calculate_entropy(EntropyDataMap &map) {
    std::vector<EntropyData<T>*> vec = std::vector<EntropyData<T>*>();

    for(EntropyDataMapInter i = map.begin(); i != map.end(); i++) {
       vec.push_back(i->second);
    }
    entropy_rate_ = EntropyData<T>::calculate_entropy(vec);
  }

  /**
   * the method calculates the redundancy of the data
   */
  void calculate_redundancy(EntropyDataMap &map) {
    double temp = 0;

    for(typename EncodeMap::iterator i = encode_map_->begin();
        i != encode_map_->end(); ++i) {
      temp += i->second.size() * map.at(i->first)->get_possibility();
    }
    redundancy_ = temp - entropy_rate_;
  }

  /**
   * the method returns the element of the vector with the lowest possibility
   * and removes it from the vector
   *
   *
   * @param  vec vector that contains TreeDataSet objects
   * @return     the return value is the TreeDataSet with the lowest possibility
   *             (possibility from EntropyData)
   */
  TreeDataSet *pop_lowest(std::vector<TreeDataSet*> &vec) {
    unsigned int pos = 0;
    unsigned int quantity = vec.at(pos)->get_data().get_quantity();
    TreeDataSet* unit;

    for(unsigned int i = 0; i < vec.size(); i++) {
      if(vec.at(i)->get_data().get_quantity() < quantity) {
        quantity = vec.at(i)->get_data().get_quantity();
        pos = i;
      }
    }

    unit = vec.at(pos);
    vec.erase(vec.begin() + pos);

    return unit;
  };

  /**
   * the method builds a binary tree based on the map
   * the Huffman algorithm is applied here
   *
   *
   * @param  map is the Map that holds all EntropyData objects of the file
   */
  TreeDataSet *build_tree(EntropyDataMap &map) {
    // timespec tS;
    // tS.tv_sec = 0;
    // tS.tv_nsec = 0;
    // clock_settime(CLOCK_PROCESS_CPUTIME_ID, &tS);

    TreeDataSetVector tree_vector = TreeDataSetVector();
    TreeDataSet *right_unit;
    TreeDataSet *left_unit;
    TreeDataSet *upper_unit;
    EntropyData<T> *upper_set;

    for(EntropyDataMapInter i = map.begin(); i != map.end(); i++) {
      tree_vector.push_back(new TreeDataSet(i->second));
    }

    while(tree_vector.size() > 1) {
      upper_set = new EntropyData<T>();

      left_unit = pop_lowest(tree_vector);
      right_unit = pop_lowest(tree_vector);
      upper_unit = new TreeDataSet(upper_set);

      left_unit->get_data().set_digit(0);
      left_unit->get_data().set_upper(upper_set);

      right_unit->get_data().set_digit(1);
      right_unit->get_data().set_upper(upper_set);

      upper_set->set_quantity(left_unit->get_data().get_quantity()
                              + right_unit->get_data().get_quantity());

      upper_unit->set_left_unit(left_unit);
      upper_unit->set_right_unit(right_unit);

      tree_vector.push_back(upper_unit);
    }

    // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tS);
    // std::cout << "build_tree:\nTime taken is: " << tS.tv_sec << " "
    // << tS.tv_nsec << std::endl;

    return tree_vector.at(0);
  };

  class TreeDataSetComperator {
  public:
    bool operator()(TreeDataSet *lhs, TreeDataSet *rhs) const {
        return lhs->get_data().get_quantity() > rhs->get_data().get_quantity();
    }
  };

  /**
   * !! the use of this variant of the function with the priority queue is NOT
   * !! encouraged since the io lag cases this to be actually SLOW
   * 
   * the method builds a binary tree based on the map
   * the Huffman algorithm is applied here
   *
   *
   * @param  map is the Map that holds all EntropyData objects of the file
   */
  TreeDataSet *build_tree_2(EntropyDataMap &map) {
    // timespec tS;
    // tS.tv_sec = 0;
    // tS.tv_nsec = 0;
    // clock_settime(CLOCK_PROCESS_CPUTIME_ID, &tS);

    std::priority_queue<TreeDataSet*, 
                        TreeDataSetVector,
                        TreeDataSetComperator> p_queue;
    TreeDataSet *right_unit;
    TreeDataSet *left_unit;
    TreeDataSet *upper_unit;
    EntropyData<T> *upper_set;

    for(EntropyDataMapInter i = map.begin(); i != map.end(); i++) {
      p_queue.push(new TreeDataSet(i->second));
    }

    while(p_queue.size() > 1) {
      upper_set = new EntropyData<T>();

      left_unit = p_queue.top();
      p_queue.pop();
      right_unit = p_queue.top();
      p_queue.pop();
      upper_unit = new TreeDataSet(upper_set);

      left_unit->get_data().set_digit(0);
      left_unit->get_data().set_upper(upper_set);

      right_unit->get_data().set_digit(1);
      right_unit->get_data().set_upper(upper_set);

      upper_set->set_quantity(left_unit->get_data().get_quantity()
                              + right_unit->get_data().get_quantity());

      upper_unit->set_left_unit(left_unit);
      upper_unit->set_right_unit(right_unit);

      p_queue.push(upper_unit);
    }

    // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tS);
    // std::cout << "build_tree_2:\nTime taken is: " << tS.tv_sec << " "
    // << tS.tv_nsec << std::endl;

    return p_queue.top();
  };

  /**
   * debug method - displays needed data to every unique data element
   *
   *
   * @param map map the information to be displayed is retrieved from
   */
  void debug(EntropyDataMap &map) {
    EntropyData<T> *debug;

    for(EntropyDataMapInter i = map.begin(); i != map.end(); i++) {
      debug = i->second;
      std::cout << "C: " << (int)debug->get_data() << "\tQ: " 
      << debug->get_quantity() << "\tP: " << debug->get_possibility()
      << "\tiC: " << debug->get_information_content() << "\tCode: " 
      << bool_to_string(debug->calculate_code()) << std::endl;
    }
    
    std::cout << "Sets: " << map.size() << std::endl
    << "DataCount: " << data_vector_->size() << std::endl
    << "Entropy: " << entropy_rate_ << std::endl
    << "Redundancy: " << redundancy_ << std::endl;
  };

  /**
   * calculates the entropy tree if there wasn't a file processed in the
   * constructor
   */
  void make_char_code_map() {
    EntropyDataMap map = EntropyDataMap();
    data_to_entropy_data_set_map(map);
    calculate_entropy(map);
    tree_ = build_tree(map);

    for(EntropyDataMapInter i = map.begin(); i != map.end(); i++) {
       (*encode_map_)[i->first] = i->second->calculate_code();
    }

    calculate_redundancy(map);

    // debug(map);
  };

  /**
   * the method encodes the original data
   */
  void encode_data() {
    signed char bits = 7;
    char buffer = 0;
    std::ostringstream ss;
    std::vector<bool> temp_vec;

    for(unsigned int i = 0; i < data_vector_->size(); i++) {
      temp_vec = (*encode_map_)[data_vector_->at(i)];

      for(unsigned int j = 0; j < temp_vec.size(); j++) {
        if(bits < 0) {
          ss << buffer;
          buffer = 0;
          bits = 7;
        }

        buffer |=  temp_vec.at(j) << bits;
        bits--;
      }
    }
    if(bits != 7) {
      ss << buffer;
    }

    last_bit_ = bits + 1;

    encoded_data_ = new std::string(ss.str());
  };

  std::string bool_to_string(const std::vector<bool> &temp) {
    std::ostringstream ss;
   
    for(std::vector<bool>::const_iterator i = temp.begin();
        i != temp.end(); i++) {
      ss << (int) *i;
    }
    return ss.str();
  }

  // vector the original data is stored in
  std::vector<T> *data_vector_;
  // map of the data elements an their codes
  EncodeMap *encode_map_;
  // string of the encoded original data
  std::string *encoded_data_;
  // the entropy rate of the original data
  double entropy_rate_;
  // the redundancy of the original data
  double redundancy_;

  signed char last_bit_;

  TreeDataSet *tree_;
};
#endif

