#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "memory_manager.h"
#include "linked_list.h"

// Mutex for thread safety
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t list_rwlock = PTHREAD_RWLOCK_INITIALIZER; // Read-Write lock for read-heavy functions.
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;    // Mutex lock for write-heavy functions.

// Initialization function
void list_init(Node** head, size_t size) {
    pthread_mutex_lock(&list_mutex);

    mem_init(size);
    *head = NULL; // Initialize head to NULL to represent an empty list

    pthread_mutex_unlock(&list_mutex);
}

// Insertion at the rear
void list_insert(Node** head, uint16_t data) {
    pthread_mutex_lock(&list_mutex);

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed during insertion.\n");
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    pthread_mutex_unlock(&list_mutex);
}

// Insertion after a specific node
void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL.\n");
        return;
    }

    pthread_mutex_lock(&list_mutex);

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed.\n");
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;

    pthread_mutex_unlock(&list_mutex);
}

// Insertion before a specific node
void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (*head == NULL || next_node == NULL) {
        printf("Error: Invalid node reference.\n");
        return;
    }

    pthread_mutex_lock(&list_mutex);

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed.\n");
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    new_node->data = data;

    // If next_node is the head
    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    // Find the node just before next_node
    Node* temp = *head;
    while (temp != NULL && temp->next != next_node) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Error: Node not found.\n");
        mem_free(new_node);
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    new_node->next = next_node;
    temp->next = new_node;

    pthread_mutex_unlock(&list_mutex);
}

// Deletion function
void list_delete(Node** head, uint16_t data) {
    pthread_mutex_lock(&list_mutex);

    if (*head == NULL) {
        printf("List is empty.\n");
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    // Check if head node contains the data
    if (temp != NULL && temp->data == data) {
        *head = temp->next; // Change head
        mem_free(temp); // Free memory
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    // Search for the node to be deleted
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Node with data %d not found.\n", data);
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    // Unlink the node from the list
    prev->next = temp->next;
    mem_free(temp); // Free memory

    pthread_mutex_unlock(&list_mutex);
}

// Search function
Node* list_search(Node** head, uint16_t data) {
    pthread_rwlock_rdlock(&list_rwlock);

    Node* temp = *head;
    while (temp != NULL) {
        if (temp->data == data) {
            pthread_mutex_unlock(&list_mutex);
            return temp; // Return the node if data matches
        }
        temp = temp->next;
    }

    pthread_rwlock_unrdlock(&list_rwlock);
    return NULL; // Return NULL if not found
}

// Display the list
void list_display(Node** head) {
    pthread_rwlock_rdlock(&list_rwlock);

    Node* current = *head;
    printf("[");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]\n");

    pthread_rwlock_unrdlock(&list_rwlock);
}


void list_display_range(Node** head, Node* start_node, Node* end_node) {
    pthread_rwlock_rdlock(&list_rwlock); // Acquire read lock

    Node* current = start_node ? start_node : *head;
    printf("[");
    while (current != NULL && (end_node == NULL || current != end_node->next)) {
        printf("%u", current->data);
        if (current->next != NULL && current != end_node) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");

    pthread_rwlock_unlock(&list_rwlock); // Release read lock
}



// Count nodes in the list
int list_count_nodes(Node** head) {
    pthread_rwlock_rdlock(&list_rwlock);

    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    pthread_rwlock_unrdlock(&list_rwlock);
    return count;
}

// Cleanup function
void list_cleanup(Node** head) {
    pthread_mutex_lock(&list_mutex);

    Node* temp = *head;
    while (temp != NULL) {
        Node* next = temp->next;
        mem_free(temp); // Free each node
        temp = next;
    }
    *head = NULL; // Set head to NULL after cleanup

    pthread_mutex_unlock(&list_mutex);
    pthread_mutex_destroy(&list_mutex); // Destroy mutex after use
}
