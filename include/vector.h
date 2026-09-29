#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

/// Growable array that owns copies of fixed size elements.
/// The struct itself is owned by the caller; the vector only manages `data`.
struct vector {
    size_t element_size;
    size_t num_elements;
    size_t max_elements;

    unsigned char* data;
};

/// @brief Initializes a vector with default initial capacity.
/// @param self
/// @param element_size sizeof(struct), must be non zero
/// @return 0 on success, -1 on bad arguments or memory allocation failure.
int vector_init(struct vector *restrict self, size_t element_size);

/// @brief Initializes a vector with the specified initial capacity.
/// @param self
/// @param element_size sizeof(struct), must be non zero
/// @param initSize initial capacity, a capacity of 0 is raised to 1
/// @return 0 on success, -1 on bad arguments or memory allocation failure.
int vector_init_size(struct vector *restrict self, size_t element_size, size_t initSize);

/// @brief Grow the capacity to hold at least min_capacity elements. Never shrinks
/// and never changes num_elements. Pointers into the vector are invalidated if it grows.
/// @param self
/// @param min_capacity
/// @return 0 on success, -1 on overflow or memory allocation failure.
int vector_reserve(struct vector *restrict self, size_t min_capacity);

/// @brief Free the element storage and reset the vector to empty.
/// Does not free `self`. Safe to call on a zeroed or already freed vector.
/// @param self
/// @return 0
int vector_free(struct vector *restrict self);

/// @brief Copy data into the element at index with bounds checking.
/// @param self
/// @param index
/// @param data element_size bytes to copy
/// @return 0 on success, -1 if index >= num_elements.
int vector_set_checked(struct vector *restrict self, size_t index, const void *data);

/// @brief Copy data into the element at index without bounds checking.
/// @param self
/// @param index must be < num_elements
/// @param data element_size bytes to copy
void vector_set_unchecked(struct vector *restrict self, size_t index, const void *data);

/// @brief Copy an element to the end of the vector, growing if necessary.
/// data may point at an element of this same vector.
/// @param self
/// @param data element_size bytes to copy
/// @return 0 on success, -1 on memory allocation failure.
int vector_push(struct vector *restrict self, const void *data);

/// @brief Get a pointer to the element at index with bounds checking.
/// The pointer is invalidated by any call that grows the vector.
/// @param self
/// @param index
/// @return pointer to the element, NULL if index >= num_elements.
void *vector_get_checked(struct vector *restrict self, size_t index);

/// @brief Get a pointer to the element at index without bounds checking.
/// The pointer is invalidated by any call that grows the vector.
/// @param self
/// @param index must be < num_elements
/// @return pointer to the element.
void *vector_get_unchecked(struct vector *restrict self, size_t index);

/// @brief Remove the last element from the vector.
/// The returned pointer is only valid until the next push, copy it out if you need to keep it.
/// @param self
/// @return pointer to the removed element, NULL if the vector is empty.
void *vector_pop(struct vector *restrict self);

/// @brief Peek at the last element in the vector without removing it.
/// @param self
/// @return pointer to the last element, NULL if the vector is empty.
void *vector_peek(struct vector *restrict self);

#endif
