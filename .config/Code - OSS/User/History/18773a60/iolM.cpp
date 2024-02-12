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

void windowtree:calcSizes(int X, int Y, node root){ // Calculates absolute size of each tiled window
  root->sizeX = X;
  root->sizeY = Y;

  // Root divided evenly between subcontainers

  if (root->t != full){ // if has subcontainers
    if (node->t == horizontal){ // horizontally divide space
      X = X/2;
    }
    else{ // vertically divide space
      Y = Y/2;
    }

    // recurse depth-first through tree
    calcSizes(X, Y, root->left);
    calcSizes(X, Y, root->right);
  }
}

void windowTree:switchMode(node n, enum nodetype t){ // Changes how subtree is tiled
  if (n->t != t){ // skip if no change

    // ensures container with two subcontainers always divides space
    if (t == full &&
    n->left != NULL &&
    n->right != NULL){
      LOG(ERROR) << "Window can only span container if it is the only child."; 
      return;
    }

    // assign new mode and recalculate sizes for modified container
    n->t = t;
    calcSizes(n->sizeX, n->sizeY, n);
  }
}