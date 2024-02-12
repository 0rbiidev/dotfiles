#include "window_tree.hpp"

windowtree::windowtree(){
  root=NULL;
}

windowtree::~windowtree(){
  destroytree();
}

void windowtree::destroytree(node *leaf){
  if (leaf!=NULL){
    destroytree(leaf->left);
    destroytree(leaf->right);
    delete leaf;
  }
}

