// Implement only this file. See avl.h and the assignment for contracts.
#include "avl.h"
/*return how tall a subtree is by returning a node's height var*/
int nodeHeight(const Node* node) {
    if (node == nullptr)
    {
        return -1; //no tree at all
    }
    return node->height;
}

/*look at current node's left/right subtree heights
  and add 1 to the greater height to calculate
  new height of current node*/
void updateHeight(Node* node) {
    if (node != nullptr)
    {
        int leftTreeHeight = nodeHeight(node->left);
        int rightTreeHeight = nodeHeight(node->right);

        node->height = 1 + (leftTreeHeight > rightTreeHeight ? leftTreeHeight : rightTreeHeight);
    }   
}

/*find the difference between a node's 
left and right subtrees and return the value*/
int balanceFactor(const Node* node) {
    if (node == nullptr)
    {
        return 0;
    }
    return nodeHeight(node->right) - nodeHeight(node->left);
}

/*when an imbalance on a nodes right subtree is detected
rearrange the nodes of the right child and the root. 
Move the root into the right child's
left spot and the move the old left subtree into the 
new left child's(was the root) right spot 
update node heights
return the original right child which is now the updated root*/
Node* rotateLeft(Node* root) {
    Node* rightChild = root->right;
    Node* rightChildLeftSubtree = rightChild->left;

    //perform rotation
    rightChild->left = root;
    root->right = rightChildLeftSubtree;

    //update heights
    updateHeight(root);
    updateHeight(rightChild);
    
    return rightChild;
}

/*when an imbalance on a nodes left subtree is detected
rearrange the nodes of the left child and the root.
Move the root into the left child's
right spot and the move the old right subtree into the 
new right child's(was the root) left spot 
update node heights
return the original right child which is now the updated root*/
Node* rotateRight(Node* root) {
    Node* leftChild = root->left;
    Node* leftChildRightSubtree = leftChild->right;

    //perform rotation
    leftChild->right = root;
    root->left = leftChildRightSubtree;

    //update heights
    updateHeight(root);
    updateHeight(leftChild);
    
    return leftChild;
}

/*check if we need to rebalance the tree and if so, which way
4 cases for rebalancing, cases are ordered:
1. Right Right
2. Left Left
3. Right Left
4. Left Right*/
Node* rebalance(Node* root) {
    if (balanceFactor(root) == 2 && balanceFactor(root->right) > 0)
    {
        root = rotateLeft(root);
    }
    else if (balanceFactor(root) == -2 && balanceFactor(root->left) < 0)
    {
        root = rotateRight(root);
    }
    else if (balanceFactor(root) == 2 && balanceFactor(root->right) < 0)
    {
        root->left = rotateRight(root->left);
        root = rotateLeft(root);
    }
    else if (balanceFactor(root) == -2 && balanceFactor(root->right) > 0)
    {
        root->right = rotateLeft(root->right);
        root = rotateRight(root);
    }
    return root;
} // TODO

/* recursively inserts a new flight node into the AVL 
tree while maintaining binary search tree ordering, 
updating heights, and rebalancing where necessary*/
Node* insertNode(Node* root, Node* newNode) {
    if (root->data.departure > newNode->data.departure)
    {
      if (root->left == nullptr)
      {
          root->left = newNode;
      }
      else
      {
          insertNode(root->left, newNode);
      } 
    }
    else
    {
        if (root->right == nullptr)
        {
             root->right = newNode;
        }
        else
        {
            insertNode(root->right, newNode);
        }
    }
    return rebalance(root);
}

/*helper func for deleteNode*/
static Node* findMin(Node *root)
{
    while (root->left != nullptr)
    {
        root = root->left;
    }
    return root;
}

/*recursively searches for and removes a flight node matching 
the given departure time, handling all cases (0, 1, or 2 children), 
freeing memory, and rebalancing the tree*/
Node* deleteNode(Node* root, int departure) {
    if (root == nullptr)
    {
        return nullptr;
    }

    if (departure != root->data.departure)
    {
        if (departure < root->data.departure)
        {
            root->left = delete(root->left, departure);
        }
        else
        {
            root->right = delete(root->right, departure);
        }
    }
    else
    {
        //case 1 and 2: delete node with zero or one child 
        if (root->left == nullptr)
        {
            Node* temp = root->right;
            delete root;
            return temp;
        }
        else if (root->right == nullptr)
        {
            Node* temp = root->left;
            delete root;
            return temp;
        }
        else //2 child case
        {
            Node* temp = findMin(root->right);
            root->data = temp->data;

            root->right = deleteNode(root->right, temp->data.departure);
        }
        
    }
    return rebalance(root);
}

/*recursively search the tree for a node which matches
passed in departure time, return the node or null 
whichever appears first*/
const Node* findNode(const Node* root, int departure) {
    if (root == nullptr)
    {
        return nullptr;
    }
    else if (departure == root->data.departure)
    {
        return root;
    }
    else if (departure < root->data.departure)
    {
        return findNode(root->left, departure);
    }
    else
    {
        return findNode(root->right, departure);
    }
}

/*recursively searches the AVL tree to find the earliest flight 
with a departure time at or after the specified target time.
eturns a const pointer to the matching node, or nullptr if none exist*/
const Node* earliestNode(const Node* root, int departure) {
    if (root == nullptr)
    {
        return nullptr;
    }
    else if (root->data.departure == departure)
    {
        return root;
    }
    else if (root->data.departure < departure)
    {
        return earliestNode(root->right, departure);
    }
    else
    {
        const Node* leftResult = earliestNode(root->left, departure);
        return (leftResult != nullptr) ? leftResult : root;
    }
}

/*findRange performs a pruned 'in-order' traversal of the AVL tree
to collect all flights falling within the inclusive [start, end] 
departure time range stores matching node pointers in the results 
array and increments count.*/
void findRange(const Node* root, int start, int end,
               const Node* results[], int& count) {
    //default case
    if (root == nullptr) return;

    /*1. If current node's departure is greater than start 
    there might be valid flights in the left subtree.*/ 
    if (root->data.departure > start) 
    {
        findRange(root->left, start, end, results, count);
    }

    // 2. current node falls within the range, include it
    if (root->data.departure >= start && root->data.departure <= end) 
    {
        results[count++] = root;
    }

    /*3. If current node's departure is less than end 
    there might be valid flights in the right subtree.*/ 
    if (root->data.departure < end) 
    {
        findRange(root->right, start, end, results, count);
    }
}
