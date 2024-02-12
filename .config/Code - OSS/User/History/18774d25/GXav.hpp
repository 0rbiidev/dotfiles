struct node{
  int windowid;
  int sizeX, sizeY;
  node *left;
  node *right;
}

class windowtree{
  public:
    windowtree(int rootX, int rootY);
    ~windowtree();

    void insert(int windowid);
    node *search(int windowid);
    void destroytree();

  private:
    void destroytree(node* leaf);
    void insert(int windowid, node *leaf);
    node *search(int windowid, node leaf);
    
    node *root;
}