struct node{
  int key;
  int sizeX, sizeY;
  node *left;
  node *right;
}

class windowtree{
  public:
    windowtree();
    ~windowtree();

    void insert(int key);
    node *search(int key);
    void destroytree();

  private:
    void destroytree(node* leaf);
    void insert(int key, node *leaf);
    node *search(int key, node leaf);
    
    node *root;
}