#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <algorithm>

using namespace std;

struct Node {
    int key;
    bool isRed; 
    Node* left = nullptr, * right = nullptr;
    Node(int k) : key(k), isRed(true) {}
};

struct Node23 {
    vector<int> keys;
    vector<Node23*> children;
};

Node* rotateRight(Node* h) {
    Node* x = h->left; h->left = x->right; x->right = h;
    return x;
}
Node* rotateLeft(Node* h) {
    Node* x = h->right; h->right = x->left; x->left = h;
    return x;
}

Node* splay(Node* root, int key) {
    if (!root || root->key == key) return root;
    if (root->key > key) {
        if (!root->left) return root;
        if (root->left->key > key) {
            root->left->left = splay(root->left->left, key);
            root = rotateRight(root);
        }
        else if (root->left->key < key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right) root->left = rotateLeft(root->left);
        }
        return root->left ? rotateRight(root) : root;
    }
    else {
        if (!root->right) return root;
        if (root->right->key > key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left) root->right = rotateRight(root->right);
        }
        else if (root->right->key < key) {
            root->right->right = splay(root->right->right, key);
            root = rotateLeft(root);
        }
        return root->right ? rotateLeft(root) : root;
    }
}
Node* insertSplay(Node* root, int key) {
    if (!root) return new Node(key);
    root = splay(root, key);
    if (root->key == key) return root;
    Node* n = new Node(key);
    if (root->key > key) { n->right = root; n->left = root->left; root->left = nullptr; }
    else { n->left = root; n->right = root->right; root->right = nullptr; }
    return n;
}

bool isRed(Node* x) { return x && x->isRed; }
Node* insertLLRB(Node* h, int key) {
    if (!h) return new Node(key);
    if (key < h->key) h->left = insertLLRB(h->left, key);
    else if (key > h->key) h->right = insertLLRB(h->right, key);

    if (isRed(h->right) && !isRed(h->left)) { h = rotateLeft(h); h->isRed = h->left->isRed; h->left->isRed = true; }
    if (isRed(h->left) && isRed(h->left->left)) { h = rotateRight(h); h->isRed = h->right->isRed; h->right->isRed = true; }
    if (isRed(h->left) && isRed(h->right)) { h->isRed = true; h->left->isRed = h->right->isRed = false; }
    return h;
}

int countNegatives(Node* root) {
    if (!root) return 0;
    return (root->key < 0 ? 1 : 0) + countNegatives(root->left) + countNegatives(root->right);
}

void collectKeys(Node* root, set<int>& s) {
    if (!root) return;
    s.insert(root->key);
    collectKeys(root->left, s);
    collectKeys(root->right, s);
}

Node* copyTree(Node* root) {
    if (!root) return nullptr;
    Node* n = new Node(root->key); n->isRed = root->isRed;
    n->left = copyTree(root->left); n->right = copyTree(root->right);
    return n;
}

Node23* convertTo23(Node* r) {
    if (!r) return nullptr;
    Node23* n = new Node23();
    if (isRed(r->left)) {
        n->keys = { r->left->key, r->key };
        if (auto c = convertTo23(r->left->left)) n->children.push_back(c);
        if (auto c = convertTo23(r->left->right)) n->children.push_back(c);
        if (auto c = convertTo23(r->right)) n->children.push_back(c);
    }
    else {
        n->keys = { r->key };
        if (auto c = convertTo23(r->left)) n->children.push_back(c);
        if (auto c = convertTo23(r->right)) n->children.push_back(c);
    }
    return n;
}

void printTable(Node* root, bool showColor) {
    if (!root) { cout << "height: 0\n\n"; return; }
    cout << "node\tlevel\tparent\tside\tkey" << (showColor ? "\tcolor\n" : "\n");

    struct Item { Node* n; int id, lvl, pId; string s; };
    queue<Item> q; vector<Item> res; int id = 1;
    q.push({ root, id++, 1, 0, "-" });

    while (!q.empty()) {
        Item c = q.front(); q.pop(); res.push_back(c);
        if (c.n->left) q.push({ c.n->left, id++, c.lvl + 1, c.id, "left" });
        if (c.n->right) q.push({ c.n->right, id++, c.lvl + 1, c.id, "right" });
    }
    int maxLvl = 0;
    for (auto& i : res) {
        maxLvl = max(maxLvl, i.lvl);
        cout << i.id << "\t" << i.lvl << "\t" << (i.pId ? to_string(i.pId) : "-") << "\t" << i.s << "\t" << i.n->key;
        if (showColor) cout << "\t" << (i.n->isRed ? "red" : "black");
        cout << "\n";
    }
    cout << "height: " << maxLvl << "\n\n";
}

void print23Table(Node23* root) {
    if (!root) return;
    cout << "node\tlevel\tparent\tchild_idx\tkeys\n";
    struct Item { Node23* n; int id, lvl, pId, cIdx; };
    queue<Item> q; int id = 1;
    q.push({ root, id++, 1, 0, 0 });
    int maxLvl = 0;
    while (!q.empty()) {
        Item c = q.front(); q.pop(); maxLvl = max(maxLvl, c.lvl);
        cout << c.id << "\t" << c.lvl << "\t" << (c.pId ? to_string(c.pId) : "-") << "\t" << (c.cIdx ? to_string(c.cIdx) : "-") << "\t";
        for (int k : c.n->keys) cout << k << " ";
        cout << "\n";
        for (size_t i = 0; i < c.n->children.size(); ++i)
            q.push({ c.n->children[i], id++, c.lvl + 1, c.id, (int)i + 1 });
    }
    cout << "height: " << maxLvl << "\n\n";
}

int main() {
    vector<int> k1 = { 21, 12, 8, 13, -5, 1, 4, 9, 7, 25 }; 
    vector<int> k2 = { 8, 13, 9, 25, 40, -5 };           

    cout << "=== SPLAY TREE ===\n";
    Node* splayT1 = nullptr; for (int x : k1) splayT1 = insertSplay(splayT1, x);
    Node* splayT2 = nullptr; for (int x : k2) splayT2 = insertSplay(splayT2, x);

    cout << "[Task A] Negatives in T1: " << countNegatives(splayT1) << "\n\n";

    set<int> s1, s2; collectKeys(splayT1, s1); collectKeys(splayT2, s2);
    Node* splayNewT1 = nullptr;
    for (int x : s1) if (s2.count(x)) splayNewT1 = insertSplay(splayNewT1, x);
    splayT1 = splayNewT1;
    cout << "[Task B] Tree T1 after intersection:\n"; printTable(splayT1, false);

    Node* splayCopy = copyTree(splayT1);
    cout << "[Task C] Splay Copy T1:\n"; printTable(splayCopy, false);

    cout << "=== LLRB TREE ===\n";
    Node* llrbT1 = nullptr; for (int x : k1) { llrbT1 = insertLLRB(llrbT1, x); llrbT1->isRed = false; }

    cout << "[Task B & C combined for LLRB example]:\n";
    printTable(llrbT1, true);

    cout << "[Task D] Conversion LLRB to 2-3 Tree:\n";
    Node23* tree23 = convertTo23(llrbT1);
    print23Table(tree23);

    return 0;
}
