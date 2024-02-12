extern "C"{
#include <X11/Xlib.h>
}

enum nodetype {horizontal, veritcal, full};

struct node{
  int key;
  int sizeX, sizeY;
  Window *w;

  node *left;
  node *right;
  node *pred;

  enum nodetype t;
}

class windowtree{
  public:
    windowtree();
    ~windowtree();

    void insert(Window *w);
    node *search(Window *w);
    void destroytree();
    void calcSizes(int X, int Y, node root);
    void switchMode(node n, enum nodetype t);

  private:
    void destroytree(node* leaf);
    void insert(Window *w, node *leaf);
    node *search(Window *w, node *leaf);
    
    node *root;
}