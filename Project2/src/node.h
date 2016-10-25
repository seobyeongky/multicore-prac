#ifndef __PROJECT2_SLIST_H__
#define __PROJECT2_SLIST_H__

template <typename T>
struct Node {
public:
    Node * next;       
    T data;

    Node() {}

    Node(Node * next, const T & data)
        : next(next), data(data) {}
};

template <typename T>
Node<T> * NodePushFront(Node<T> ** head, const T & data) {
    Node<T> * new_node = new Node<T>(*head, data);
    *head = new_node;
    return new_node;
}

template <typename T>
Node<T> * NodeErase(Node<T> ** head, Node<T> * node) {
    Node<T> * prev_node = nullptr;
    for (Node<T> * it = *head; it != nullptr; it = it->next) {
        if (it == node) {
            break;
        }
        prev_node = it;
    }

    if (prev_node == nullptr) {
        // That means node is head
        *head = node->next;
    }
    
    return NodeErase(prev_node, node);
}

template <typename T>
Node<T> * NodeErase(Node<T> * prev_node, Node<T> * node) {
    if (prev_node != nullptr) {
        prev_node->next = node->next;
    }

    //delete node;

    return prev_node ? prev_node->next : nullptr;
}

template <typename T>
void DestroyNodes(Node<T> ** head) {
    for (Node<T> * it = *head; it != nullptr;) {
        Node<T> *next_it = it->next;
        delete it;
        it = next_it;
    }
    *head = nullptr;
}


#endif /* __PROJECT2_SLIST_H__ */
