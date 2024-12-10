#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "memory_manager.h"
#include "linked_list.h"
#include <string.h>



// Initialization function
void list_init(Node** head, size_t size) {
    mem_init(size);
    *head = NULL; // Initialize head to NULL to represent an empty list

}

// Insertion at the rear
void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));//
    if (!new_node) {
        printf("Memory allocation failed during insertion.\n");
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
        return;
    }
}








// Insertion after a specific node
void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    
}

// Insertion before a specific node
void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (*head == NULL || next_node == NULL) {
        printf("Error: Invalid node reference.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    new_node->data = data;

    // If next_node is the head
    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
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
        return;
    }

    new_node->next = next_node;
    temp->next = new_node;
}

// Deletion function
void list_delete(Node** head, uint16_t data) {
    if (*head == NULL) {
        printf("List is empty.\n");
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    // Check if head node contains the data
    if (temp != NULL && temp->data == data) {
        *head = temp->next; // Change head
        mem_free(temp); // Free memory
        return;
    }

    // Search for the node to be deleted
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Node with data %d not found.\n", data);
        return;
    }

    // Unlink the node from the list
    prev->next = temp->next;
    mem_free(temp); // Free memory
}

// Search function
Node* list_search(Node** head, uint16_t data) {
    Node* temp = *head;
    while (temp != NULL) {
        if (temp->data == data) {
            return temp; // Return the node if data matches
        }
        temp = temp->next;
    }
    return NULL; // Return NULL if not found
}




void list_display(Node** head) {
    if (head == NULL) {
        printf("[]\n");
        return;
    }

    Node* current = *head;
    printf("[");

    // Traverse & print each node data
    while (current != NULL) {
        printf("%d", current->data);

        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]\n");
}


void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* temp = *head;

    // If start_node is NULL, start from the beginning of the list
    if (start_node == NULL) {
        start_node = *head;
    }

    // Traverse to the start_node
    while (temp != NULL && temp != start_node) {
        temp = temp->next;
    }

    // Start printing the list in the correct format
    printf("[");
    bool first = true;  // To handle commas correctly

    // Print elements from start_node to end_node
    while (temp != NULL) {
        if (!first) {
            printf(", ");  // Print a comma before subsequent elements
        }
        printf("%d", temp->data);
        first = false;  // After the first element, commas are needed

        if (temp == end_node) {
            break;
        }
        temp = temp->next;
    }
    printf("]");  // Do not add newline here
}



// Count nodes in the list
int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

// Cleanup function
void list_cleanup(Node** head) {
    Node* temp = *head;
    while (temp != NULL) {
        Node* next = temp->next;
        mem_free(temp); // Free each node
        temp = next;
    }
    *head = NULL; // Set head to NULL after cleanup
    mem_deinit();
}
