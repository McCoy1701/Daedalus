/* dDUFQuery.c - DUF query functions (AUF-style) */

#include "Daedalus.h"
#include <stdlib.h>
#include <string.h>

// =============================================================================
// Object Item Access (AUF-style)
// =============================================================================

dDUFValue_t* d_DUFGetObjectItem(dDUFValue_t* node, const char* key)
{
    if (node == NULL) {
        d_LogError("NULL node passed to d_DUFGetObjectItem.");
        return NULL;
    }

    // Traverse children looking for matching key
    dDUFValue_t* current = node->child;
    while (current != NULL) {
        if (current->string != NULL && strcmp(current->string, key) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}
