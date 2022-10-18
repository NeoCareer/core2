#pragma once

#include "lib/All.h"

namespace core {

template <typename Key, typename Value> class AVLTree {
  struct AVLTreeNode {
    Key key;
    Value value;
    AVLTreeNode *left, *right;

    template <typename T_>
    AVLTreeNode(T_ &&value_)
        : value(std::forward<T_>(value_)), left(nullptr), right(nullptr) {}
  };

  AVLTreeNode *root = nullptr;

  AVLTree() {
    std::map<int, int> a;
    a.insert({1, 2});
  }
};

} // namespace core