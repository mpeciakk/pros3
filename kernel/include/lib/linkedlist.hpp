#ifndef __LINKEDLIST_HPP
#define __LINKEDLIST_HPP

#include <cstddef>
#include <lib/memory.hpp>
#include <lib/string.hpp>

template <class T> struct Node {
    T data;
    Node* prev;
    Node* next;
};

template <class T> class LinkedList {
public:
    Node<T>* head;

    LinkedList() {
        head = nullptr;
        tail = nullptr;
    }

    ~LinkedList() {
        for (Node<T>* node = head; node != nullptr; node = node->next) {
            delete node;
        }
    }

    void pushFront(T value) {
        Node<T>* temp = new Node<T>();
        temp->data = value;

        if (head == nullptr) {
            head = temp;
            tail = temp;
            temp->prev = nullptr;
            temp->next = nullptr;
        } else {
            head->prev = temp;
            temp->next = head;

            temp->prev = nullptr;

            head = temp;
        }
    }

    void pushBack(T value) {
        Node<T>* temp = new Node<T>();
        temp->data = value;

        if (head == nullptr) {
            head = temp;
            tail = temp;
            temp->prev = nullptr;
            temp->next = nullptr;
        } else {
            tail->next = temp;
            temp->prev = tail;

            temp->next = nullptr;

            tail = temp;
        }
    }

    T popFront() {
        if (head == nullptr) {
            return NULL;
        }

        Node<T>* returnValue = head;
        Node<T>* newHead = head->next;

        if (returnValue->prev) {
            returnValue->prev->next = returnValue->next;
        }
        if (returnValue->next) {
            returnValue->next->prev = returnValue->prev;
        }

        if (!newHead) {
            head = nullptr;
            tail = nullptr;
        } else {
            head = newHead;
        }

        return returnValue->data;
    }

    T popBack() {
        if (head == nullptr) {
            return nullptr;
        }

        Node<T>* returnValue = tail;
        Node<T>* newTail = tail->prev;

        if (returnValue->prev) {
            returnValue->prev->next = returnValue->next;
        }
        if (returnValue->next) {
            returnValue->next->prev = returnValue->prev;
        }

        returnValue->prev = nullptr;
        returnValue->next = nullptr;

        if (!newTail) {
            head = nullptr;
            tail = nullptr;
        } else {
            tail = newTail;
        }

        return returnValue->data;
    }

    int size() {
        int size = 0;

        Node<T>* current = head;

        while (current != nullptr) {
            size++;

            current = head->next;
        }

        return size;
    }

private:
    Node<T>* tail;
};

#endif