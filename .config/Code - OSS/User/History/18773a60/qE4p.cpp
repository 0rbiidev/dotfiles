#include "window_tree.hpp"

extern "C"{
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
}

#include "glog/logging.h"

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

void windowtree::insert(Window *newwin, node *leaf){
  if(leaf->left!=NULL){
    if(leaf->right!=NULL){
      insert(newwin, leaf->left)
    }
    else{
      leaf->right=new node;
      leaf->right->w = leaf->w;
      leaf->right->pred = leaf;
      leaf->w = NULL;
      leaf->t = vertical;
      leaf->right->left= new node;
      leaf->right->left->w = newwin;
      leaf->right->left->pred = leaf->right;
      leaf->right->left->left = NULL;
      leaf->right->left->right = NULL;
    }
  }
  else{
    leaf->left = new node;
    leaf->left->w = leaf->w;
    leaf->left->t = full;
  }
}

void windowtree:calcSizes(int X, int Y, node root){
  root->sizeX = X;
  root->sizeY = Y;

  if (root->t != full){
    if (node->t == horizontal){
      X = X/2;
    }
    else{
      Y = Y/2;
    }

    calcSizes(X, Y, root->left);
    calcSizes(X, Y, root->right);
  }
}

void windowTree:switchMode(node n, enum nodetype t){
  if (n->t != t && n != full){
    n->t = t;
    calcSizes(n->sizeX, n->sizeY, n);
  }
  else if (n->left != NULL && n->right != NULL){
    LOG(ERROR) << "Cannot fullscreen container with two children"; 
  }
}