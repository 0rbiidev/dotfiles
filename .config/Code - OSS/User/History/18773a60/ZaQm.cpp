#include "window_tree.hpp"

extern "C"{
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
}


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

