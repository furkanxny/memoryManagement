/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.c to edit this template
 */


#include "fscMalloc.h"
#include <assert.h>
#include <sys/mman.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for the init of rand */
#include <stddef.h> /* for size_t */

void* fscMemorySetup(memoryStructure* m, fscAllocationMethod am, size_t sizeInBytes) {
    m->counter = 1;
    if (FIRST_FIT_RETURN_FIRST != am) {
        fprintf(stderr, "This code only supports the FIRST_FIT_RETURN_FIRST allocation method\n");
        return 0;
    }

    fsc_free_node_t* freeBlock = (void*) malloc(sizeof (fsc_free_node_t) + sizeInBytes);

    if (!freeBlock) {
        printf("Malloc failed!");
        return 0;
    }

    freeBlock->size = sizeInBytes;
    freeBlock->next = NULL;
    m->head = freeBlock;
    m->magicNumber = 123;

    return m->head;
}

void* fscMalloc(memoryStructure* m, size_t requestedSizeInBytes) {
    memoryStructure *current = m;
    fsc_free_node_t *temp = current->head;

    while (temp != NULL) {
        int size = temp->size;

        if (size > (requestedSizeInBytes + sizeof (fsc_alloc_header_t))) {
            fsc_alloc_header_t* header = (fsc_alloc_header_t *) ((char*) temp + sizeof(fsc_free_node_t));
            header->size = requestedSizeInBytes;
            header->magic = 123;

            size_t totalSize = requestedSizeInBytes + sizeof (fsc_free_node_t);
            size_t totalSize1 = requestedSizeInBytes - sizeof (fsc_alloc_header_t);
            //fsc_free_node_t *pTemp = (fsc_free_node_t *) ((char *) temp + counter * (sizeof (fsc_alloc_header_t)));
            fsc_free_node_t* new = (fsc_free_node_t *) ((char *) temp + totalSize);
            //new->next = NULL;
            new->size = temp->size - totalSize1;
            m->head = new;


            printf("Memory allocation is successful\n    Taken: %zu\n    Left: %zu\n\n", totalSize1, new->size);
            return header;
        } else {
            printf("\nMemory Allocation  failed.\n    Reqeusted: %zu\n    Only Have: %d\n\n", requestedSizeInBytes + sizeof (fsc_alloc_header_t), size);
        }
        temp = temp->next;
    }
}


void fscFree(memoryStructure* m, void* returnedMemory) {
    fsc_alloc_header_t* header = (fsc_alloc_header_t*) returnedMemory;
    size_t totalSize = header->size + sizeof(fsc_alloc_header_t);
    fsc_free_node_t* freedBlock = (fsc_free_node_t*) header;
    freedBlock->size = totalSize;
    freedBlock->next = NULL;

    
    if (m->head == NULL) {
        m->head = freedBlock;
        return;
    }

    
    fsc_free_node_t* prev = NULL;
    fsc_free_node_t* current = m->head;
    while (current != NULL && (char*) current < (char*) freedBlock) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        freedBlock->next = m->head;
        m->head = freedBlock;
    } else {
        freedBlock->next = prev->next;
        prev->next = freedBlock;
    }

    
    if (prev != NULL && (char*) prev + prev->size == (char*) freedBlock) {
        prev->size += freedBlock->size;
        prev->next = freedBlock->next;
        freedBlock = prev;
    }

    
    if (freedBlock->next != NULL && (char*) freedBlock + freedBlock->size == (char*) freedBlock->next) {
        freedBlock->size += freedBlock->next->size;
        freedBlock->next = freedBlock->next->next;
    }
}



/* Given a node, prints the list for you. */
void printFreeList(FILE * out, fsc_free_node_t* head) {
    /* given a node, prints the list. */
    fsc_free_node_t* current = head;
    fprintf(out, "                                           About to dump the free list:\n\n");
    while (0 != current) {
        fprintf(out,
                "Node address: %p\t Node size (stored): %zu\t Node size (actual): %zu\t Node next: %p\n",
                current,
                current->size,
                current->size + sizeof (fsc_free_node_t),
                current->next);
        current = current->next;
    }
}


