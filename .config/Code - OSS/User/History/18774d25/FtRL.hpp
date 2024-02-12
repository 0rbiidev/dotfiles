extern "C"{
#include <X11/Xlib.h>
}

enum nodetype {horizontal, veritcal};

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
    void calcSizes(int rootsize);

  private:
    void destroytree(node* leaf);
    void insert(Window *w, node *leaf);
    node *search(Window *w, node *leaf);
    
    node *root;
}