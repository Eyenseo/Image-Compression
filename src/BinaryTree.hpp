#ifndef BINARYTREE_HPP
#define BINARYTREE_HPP

#include <iostream>
#include <stdlib.h>

// the class provides a data structure of a binary tree
template <typename T> class BinaryTree {
public:
  // constructor - no object pointer
  BinaryTree(T* data) {
    data_ = data;
    left_tree_ = NULL;
    right_tree_ = NULL;
  };

  // destructor
  ~BinaryTree() {
    delete data_;   // try to delete pointer
    data_ = NULL;
    delete left_tree_;
    left_tree_ = NULL;
    delete right_tree_;
    right_tree_ = NULL;
  };

  // returns the stored data
  T &get_data() {
    return *data_;
  };

  // returns the left tree
  BinaryTree &get_left_unit() {
    return *left_tree_;
  };

  // sets the 'left' BinaryTree if it is already set the previous tree will be 
  // deleted
  void set_left_unit(BinaryTree<T> *tree) {
    if(left_tree_ != NULL) {
      delete left_tree_;
    }

    left_tree_ = tree;
  };

  // returns the right tree
  BinaryTree &get_right_unit() {
    return *right_tree_;
  };

  // sets the 'right' BinaryTree if it is already set the previous tree will be
  // deleted
  void set_right_unit(BinaryTree *tree) {
    if(right_tree_ != NULL) {
      delete right_tree_;
    }

    right_tree_ = tree;
  };

  unsigned int get_tree_height() const {
   return get_tree_height(this);
  }

private:
   unsigned int get_tree_height(const BinaryTree *tree) const {
    if(tree == NULL) {
      return 0;
    }
    unsigned int left = get_tree_height(left_tree_);
    unsigned int right = get_tree_height(right_tree_);
    return 1 + (left > right ? left : right);
  }

  // data stored by the Tree
  T* data_;
  // tree to the 'left'
  BinaryTree *left_tree_;
  // tree to the 'right'
  BinaryTree *right_tree_;
};
#endif
