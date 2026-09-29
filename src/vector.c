#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/vector.h"

#define VECTOR_INITIAL_CAPACITY 4
#define VECTOR_GROWTH_FACTOR 2

static unsigned char *vector_element(struct vector *restrict self, size_t index) {
    return self->data + index * self->element_size;
}

int vector_init(struct vector *restrict self, size_t element_size) {
    return vector_init_size(self, element_size, VECTOR_INITIAL_CAPACITY);
}

int vector_init_size(struct vector *restrict self, size_t element_size, size_t initSize) {
    self->element_size = element_size;
    self->num_elements = 0;
    self->max_elements = 0;
    self->data = NULL;

    if (element_size == 0) {
        return -1; // Can't store zero sized elements
    }
    if (initSize == 0) {
        initSize = 1; // Growth multiplies the capacity so it can't start at 0
    }
    if (initSize > SIZE_MAX / element_size) {
        return -1; // Allocation size would overflow
    }

    self->data = malloc(initSize * element_size);
    if (self->data == NULL) {
        return -1; // Memory allocation failed
    }
    self->max_elements = initSize;
    return 0; // Success
}

int vector_reserve(struct vector *restrict self, size_t min_capacity) {
    if (min_capacity <= self->max_elements) {
        return 0; // No need to resize
    }

    size_t new_capacity = self->max_elements ? self->max_elements : 1;
    while (new_capacity < min_capacity) {
        if (new_capacity > SIZE_MAX / VECTOR_GROWTH_FACTOR) {
            new_capacity = min_capacity;
            break;
        }
        new_capacity *= VECTOR_GROWTH_FACTOR;
    }
    if (new_capacity > SIZE_MAX / self->element_size) {
        return -1; // Allocation size would overflow
    }

    unsigned char *new_data = realloc(self->data, new_capacity * self->element_size);
    if (new_data == NULL) {
        return -1; // Memory allocation failed
    }

    self->data = new_data;
    self->max_elements = new_capacity;
    return 0; // Success
}

int vector_free(struct vector *restrict self) {
    free(self->data);
    self->data = NULL;
    self->num_elements = 0;
    self->max_elements = 0;
    return 0;
}

int vector_set_checked(struct vector *restrict self, size_t index, const void *data) {
    if (index >= self->num_elements) {
        return -1; // Index out of bounds
    }

    vector_set_unchecked(self, index, data);
    return 0; // Success
}

void vector_set_unchecked(struct vector *restrict self, size_t index, const void *data) {
    // memmove so setting an element to itself is fine
    memmove(vector_element(self, index), data, self->element_size);
}

int vector_push(struct vector *restrict self, const void *data) {
    if (self->num_elements >= self->max_elements) {
        uintptr_t src = (uintptr_t)data;
        uintptr_t start = (uintptr_t)self->data;
        int inside = self->data != NULL
            && src >= start
            && src < start + self->num_elements * self->element_size;
        size_t offset = inside ? (size_t)(src - start) : 0;

        if (vector_reserve(self, self->num_elements + 1) != 0) {
            return -1; // Memory allocation failed
        }
        if (inside) {
            data = self->data + offset;
        }
    }

    memcpy(vector_element(self, self->num_elements), data, self->element_size);
    self->num_elements++;
    return 0; // Success
}

void *vector_get_checked(struct vector *restrict self, size_t index) {
    if (index >= self->num_elements) {
        return NULL; // Index out of bounds
    }

    return vector_element(self, index);
}

void *vector_get_unchecked(struct vector *restrict self, size_t index) {
    return vector_element(self, index);
}

void *vector_pop(struct vector *restrict self) {
    if (self->num_elements == 0) {
        return NULL; // Vector is empty
    }

    self->num_elements--;
    return vector_element(self, self->num_elements);
}

void *vector_peek(struct vector *restrict self) {
    if (self->num_elements == 0) {
        return NULL; // Vector is empty
    }

    return vector_element(self, self->num_elements - 1);
}
