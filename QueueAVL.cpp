#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>

#endif


template < typename T>
struct Node {
    T value;
    Node<T>* parent;
    Node<T>* left;
    Node<T>* right;
    int depth;
    int size; // Размер поддерева

    Node(const T& val) : value(val), parent(nullptr), left(nullptr), right(nullptr), depth(0), size(1) {}

    static void updateDepthAndSize(Node<T>* node) {
        if (node) {
            int left_depth = node->left ? node->left->depth : -1;
            int right_depth = node->right ? node->right->depth : -1;
            node->depth = std::max(left_depth, right_depth) + 1;

            int left_size = node->left ? node->left->size : 0;
            int right_size = node->right ? node->right->size : 0;
            node->size = 1 + left_size + right_size;
        }
    }


    static Node<T>* findByIndex(Node<T>* node, size_t index) {
        while (node) {
            int leftSize = node->left ? node->left->size : 0;
            if (index < (size_t)leftSize) {
                node = node->left;
            } else if (index == (size_t)leftSize) {
                return node;
            } else {
                index -= leftSize + 1;
                node = node->right;
            }
        }
        return nullptr;
    }


    static Node<T>* insertByIndex(Node<T>*& node, size_t index, T value, Node<T>* parent = nullptr) {
        if (!node) {
            node = new Node<T>(value);
            node->parent = parent;
            return node;
        }

        int leftSize = node->left ? node->left->size : 0;

        Node<T>* insertedNode = nullptr;

        if (index <= (size_t)leftSize) {
            insertedNode = insertByIndex(node->left, index, value, node);
        } else {
            insertedNode = insertByIndex(node->right, index - leftSize - 1, value, node);
        }

        updateDepthAndSize(node);

        node = balance(node);

        return insertedNode; // Возвращаем указатель на вставленный узел
    }







    static Node<T>* eraseByIndex(Node<T>*& node, size_t index) {
        if (!node) return nullptr;

        int leftSize = node->left ? node->left->size : 0;

        if (index < (size_t)leftSize) {
            node->left = eraseByIndex(node->left, index);
            if (node->left) node->left->parent = node;
        } else if (index == (size_t)leftSize) {

            if (!node->left || !node->right) {
                Node<T>* temp = node->left ? node->left : node->right;
                if (temp) temp->parent = node->parent;
                delete node;
                node = temp;
            } else {

                Node<T>* successor = minValueNode(node->right);
                node->value = successor->value;
                node->right = eraseByIndex(node->right, 0);
                if (node->right) node->right->parent = node;
            }
        } else {
            node->right = eraseByIndex(node->right, index - leftSize - 1);
            if (node->right) node->right->parent = node;
        }

        if (node) {
            updateDepthAndSize(node);


            node = balance(node);
        }

        return node;
    }


    static Node<T>* eraseByIndexAndReturnNode(Node<T>*& node, size_t index, Node<T>*& removedNode) {
        if (!node) return nullptr;

        int leftSize = node->left ? node->left->size : 0;

        if (index < (size_t)leftSize) {
            node->left = eraseByIndexAndReturnNode(node->left, index, removedNode);
            if (node->left) node->left->parent = node;
        } else if (index == (size_t)leftSize) {

            removedNode = node;

            if (!node->left || !node->right) {
                Node<T>* temp = node->left ? node->left : node->right;
                if (temp) temp->parent = node->parent;
                node = temp;
            } else {

                Node<T>* successor = minValueNode(node->right);

                std::swap(node->value, successor->value);
                node->right = eraseByIndexAndReturnNode(node->right, 0, removedNode);
                if (node->right) node->right->parent = node;
            }
        } else {
            node->right = eraseByIndexAndReturnNode(node->right, index - leftSize - 1, removedNode);
            if (node->right) node->right->parent = node;
        }

        if (node) {
            updateDepthAndSize(node);
            node = balance(node);
        }

        return node;
    }


    static Node<T>* minValueNode(Node<T>* node) {
        Node<T>* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    static int getBalance(Node<T>* node) {
        if (!node) return 0;
        int left_depth = node->left ? node->left->depth : -1;
        int right_depth = node->right ? node->right->depth : -1;
        return right_depth - left_depth;
    }

    static Node<T>* balance(Node<T>* node) {
        int balanceFactor = getBalance(node);

        if (balanceFactor > 1) {
            if (getBalance(node->right) >= 0) {
                node = rotateLeft(node);
            } else {
                node->right = rotateRight(node->right);
                node = rotateLeft(node);
            }
        } else if (balanceFactor < -1) {
            if (getBalance(node->left) <= 0) {
                node = rotateRight(node);
            } else {
                node->left = rotateLeft(node->left);
                node = rotateRight(node);
            }
        }

        return node;
    }

    static Node<T>* rotateLeft(Node<T>* x) {
        Node<T>* y = x->right;
        Node<T>* T2 = y->left;


        y->left = x;
        x->right = T2;


        y->parent = x->parent;
        x->parent = y;
        if (T2) T2->parent = x;


        updateDepthAndSize(x);
        updateDepthAndSize(y);

        return y;
    }

    static Node<T>* rotateRight(Node<T>* y) {
        Node<T>* x = y->left;
        Node<T>* T2 = x->right;


        x->right = y;
        y->left = T2;


        x->parent = y->parent;
        y->parent = x;
        if (T2) T2->parent = y;


        updateDepthAndSize(y);
        updateDepthAndSize(x);

        return x;
    }

    static void destroySubtree(Node<T>* node) {
        if (!node) return;
        destroySubtree(node->left);
        destroySubtree(node->right);
        delete node;
    }


    static Node<T>* cutByIndex(Node<T>*& node, size_t index, Node<T>*& removedNode) {
        if (!node) return nullptr;

        int leftSize = node->left ? node->left->size : 0;

        if (index < (size_t)leftSize) {
            node->left = cutByIndex(node->left, index, removedNode);
            if (node->left) node->left->parent = node;
        } else if (index == (size_t)leftSize) {
            removedNode = node;
            Node<T>* temp = merge(node->left, node->right);
            if (temp) temp->parent = node->parent;
            node = temp;
        } else {
            node->right = cutByIndex(node->right, index - leftSize - 1, removedNode);
            if (node->right) node->right->parent = node;
        }

        if (node) {
            updateDepthAndSize(node);
            node = balance(node);
        }

        return node;
    }

    static Node<T>* merge(Node<T>* left, Node<T>* right) {
        if (!left) return right;
        if (!right) return left;


        Node<T>* maxNode = left;
        while (maxNode->right) {
            maxNode = maxNode->right;
        }

        Node<T>* dummy = nullptr;
        left = cutByIndex(left, left->size - 1, dummy);


        maxNode->left = left;
        if (left) left->parent = maxNode;
        maxNode->right = right;
        if (right) right->parent = maxNode;

        updateDepthAndSize(maxNode);
        return balance(maxNode);
    }
    static Node<T>* insertNodeByIndex(Node<T>*& node, size_t index, Node<T>* nodeToInsert, Node<T>* parent = nullptr) {
        if (!node) {
            node = nodeToInsert;
            node->parent = parent;
            return node;
        }

        int leftSize = node->left ? node->left->size : 0;

        if (index <= (size_t)leftSize) {
            node->left = insertNodeByIndex(node->left, index, nodeToInsert, node);
        } else {
            node->right = insertNodeByIndex(node->right, index - leftSize - 1, nodeToInsert, node);
        }

        updateDepthAndSize(node);
        return balance(node);
    }
    static void eraseByIndexAndReturnValue(Node<T>*& node, size_t index, T& returnValue) {
        if (!node) {
            throw std::out_of_range("Index out of bounds");
        }

        int leftSize = node->left ? node->left->size : 0;

        if (index < (size_t)leftSize) {
            eraseByIndexAndReturnValue(node->left, index, returnValue);
            if (node->left) node->left->parent = node;
        } else if (index == (size_t)leftSize) {
            // Узел, который нужно удалить, найден
            returnValue = node->value;

            if (!node->left || !node->right) {
                Node<T>* temp = node->left ? node->left : node->right;
                if (temp) temp->parent = node->parent;
                delete node;
                node = temp;
            } else {
                // Узел с двумя детьми
                Node<T>* successor = minValueNode(node->right);
                // Копируем значение преемника в текущий узел
                node->value = successor->value;
                // Рекурсивно удаляем преемника
                eraseByIndexAndReturnValue(node->right, 0, returnValue);
                if (node->right) node->right->parent = node;
            }
        } else {
            eraseByIndexAndReturnValue(node->right, index - leftSize - 1, returnValue);
            if (node->right) node->right->parent = node;
        }

        if (node) {
            updateDepthAndSize(node);
            node = balance(node);
        }
    }


};


template < typename T >
struct Queue {
    bool empty() const {
        return m_size ==0;
    }
    size_t size() const {
        return m_size;
    }

    struct Ref{
        Node<T> * node;
    };


    Ref push_last(T x) {
        Node<T>* insertedNode = Node<T>::insertByIndex(root.node, m_size, x);
        m_size++;
        return Ref{ insertedNode };
    }

    T pop_first() {
        if (m_size == 0) {
            throw std::out_of_range("");
        }

        T firstValue;
        Node<T>::eraseByIndexAndReturnValue(root.node, 0, firstValue);
        m_size--;
        return firstValue;
    }


// throw std::out_of_range if empty

    size_t position(const Ref& it) const {
        if (it.node == nullptr){return -1;}
        Node<T>* node = it.node;
        size_t index = node->left ? node->left->size : 0;

        while (node->parent) {
            if (node == node->parent->right) {
                index += 1;
                if (node->parent->left)
                    index += node->parent->left->size;
            }
            node = node->parent;
        }

        return index;
    }


    void jump_ahead(const Ref& ref, size_t positions) {

        if(ref.node == nullptr){return;}

        size_t current_position = position(ref);
        size_t newPos = (positions >= current_position) ? 0 : (current_position - positions );
        Node<T>* removedNode = nullptr;
        root.node = Node<T>::cutByIndex(root.node, current_position, removedNode);


        removedNode->parent = nullptr;
        removedNode->left = nullptr;
        removedNode->right = nullptr;
        removedNode->size = 1;
        removedNode->depth = 0;

        root.node = Node<T>::insertNodeByIndex(root.node, newPos, removedNode);



    }

    Queue() {
        root.node = nullptr;
        m_size =0;
    }
    ~Queue() {
        root.node->destroySubtree(root.node);
        root.node = nullptr;
        m_size =0;
    }
private :
    Ref root;
    size_t m_size;
};


#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
    std::string ret = "\"";
    for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)

////////////////// End of dark magic ////////////////////////


void test1(int& ok, int& fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++) {
        Q.push_last(i % RUN);
        CHECK(Q.empty(), false);
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

void test2(int& ok, int& fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++) {
        refs.push_back(Q.push_last(i % RUN));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[0], 15);
    Q.jump_ahead(refs[3], 0);

    CHECK(Q.size(), TOT);
    for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[8], 100);
    Q.jump_ahead(refs[9], 100);
    Q.jump_ahead(refs[7], 1);

    static_assert(RUN == 10 && TOT >= 30);
    for (int i : { 9, 8, 0, 1, 2, 3, 4, 5, 7, 6 })
        CHECK(Q.pop_first(), i);

    for (int i = 0; i < TOT*2 / 3; i++) {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 11 - i);
    }

    CHECK(Q.empty(), false);
}

template < int TOT >
void test_speed(int& ok, int& fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    for (int i = 0; i < TOT; i++) {
        refs.push_back(Q.push_last(i));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.position(refs[i]), i);
        Q.jump_ahead(refs[i], i);
    }

    for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), TOT - 1 - i);

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.pop_first(), TOT - 1 - i);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

int main() {
    int ok = 0, fail = 0;
   //----- if (!fail) test1(ok, fail);
    if (!fail) test2(ok, fail);
    if (!fail) test_speed<1'000>(ok, fail);

    if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif