#include "../include/vector.h"
#include "../include/assert.h"
#include <stddef.h>
#include "tests.h"

// element larger than a pointer so element stride bugs show up
typedef struct {
    int x;
    int y;
    int z;
} Point;

void vector_initTest() {
    struct vector vec;
    assertEquals(vector_init(&vec, sizeof(int)), 0, "vector_init returns success");
    assertEquals(vec.num_elements, 0, "vector_init starts empty");
    assertEquals(vec.element_size, sizeof(int), "vector_init stores element size");
    assertTrue(vec.max_elements > 0, "vector_init has a non zero capacity");
    assertNotNULL(vec.data, "vector_init allocates data");
    vector_free(&vec);
}

void vector_init_sizeTest() {
    struct vector vec;
    assertEquals(vector_init_size(&vec, sizeof(int), 16), 0, "vector_init_size returns success");
    assertEquals(vec.num_elements, 0, "vector_init_size starts empty");
    assertEquals(vec.max_elements, 16, "vector_init_size uses the given capacity");
    vector_free(&vec);
}

void vector_push_getTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 0;
    int j = 1;
    int f = 99;
    assertEquals(vector_push(&vec, &i), 0, "push returns success");
    vector_push(&vec, &j);
    vector_push(&vec, &f);
    assertEquals(vec.num_elements, 3, "push increments num_elements");
    assertEquals(*(int *)vector_get_checked(&vec, 0), i, "get element 0");
    assertEquals(*(int *)vector_get_checked(&vec, 1), j, "get element 1");
    assertEquals(*(int *)vector_get_checked(&vec, 2), f, "get element 2");
    vector_free(&vec);
}

void vector_push_copiesTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 5;
    vector_push(&vec, &i);
    i = 6;
    assertEquals(*(int *)vector_get_checked(&vec, 0), 5, "push copies the element instead of storing the pointer");
    vector_free(&vec);
}

void vector_push_growTest() {
    struct vector vec;
    vector_init_size(&vec, sizeof(int), 2);
    for (int i = 0; i < 100; i++) {
        if (vector_push(&vec, &i) != 0) break;
    }
    assertEquals(vec.num_elements, 100, "push grows past initial capacity");
    assertTrue(vec.max_elements >= 100, "capacity covers all elements after growth");

    int allMatch = 1;
    for (int i = 0; i < 100; i++) {
        int *value = vector_get_checked(&vec, i);
        if (value == NULL || *value != i) {
            allMatch = 0;
            break;
        }
    }
    assertTrue(allMatch, "elements survive growth");
    vector_free(&vec);
}

void vector_structTest() {
    struct vector vec;
    vector_init(&vec, sizeof(Point));
    Point a = {1, 2, 3};
    Point b = {4, 5, 6};
    vector_push(&vec, &a);
    vector_push(&vec, &b);
    Point *pa = vector_get_checked(&vec, 0);
    Point *pb = vector_get_checked(&vec, 1);
    assertTrue(pa->x == 1 && pa->y == 2 && pa->z == 3, "struct element 0 round trips");
    assertTrue(pb->x == 4 && pb->y == 5 && pb->z == 6, "struct element 1 round trips");
    assertEquals((char *)pb - (char *)pa, sizeof(Point), "elements are element_size apart");
    vector_free(&vec);
}

void vector_get_checkedTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    assertNULL(vector_get_checked(&vec, 0), "get on empty vector");
    int i = 7;
    vector_push(&vec, &i);
    assertNULL(vector_get_checked(&vec, 1), "get at num_elements");
    assertNULL(vector_get_checked(&vec, 2), "get inside capacity but past num_elements");
    assertNULL(vector_get_checked(&vec, 100), "get past capacity");
    assertNULL(vector_get_checked(&vec, -1), "negative index wraps and is rejected");
    vector_free(&vec);
}

void vector_get_uncheckedTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 3;
    int j = 4;
    vector_push(&vec, &i);
    vector_push(&vec, &j);
    assertEquals(*(int *)vector_get_unchecked(&vec, 0), i, "unchecked get element 0");
    assertEquals(*(int *)vector_get_unchecked(&vec, 1), j, "unchecked get element 1");
    vector_free(&vec);
}

void vector_set_checkedTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 1;
    int j = 2;
    vector_push(&vec, &i);
    assertEquals(vector_set_checked(&vec, 0, &j), 0, "set in bounds returns success");
    assertEquals(*(int *)vector_get_checked(&vec, 0), j, "set in bounds replaces the element");
    assertEquals(vector_set_checked(&vec, 1, &j), -1, "set at num_elements fails");
    assertEquals(vector_set_checked(&vec, 100, &j), -1, "set past capacity fails");
    assertEquals(vec.num_elements, 1, "failed set doesn't change num_elements");
    vector_free(&vec);
}

void vector_set_uncheckedTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 1;
    int j = 2;
    vector_push(&vec, &i);
    vector_set_unchecked(&vec, 0, &j);
    assertEquals(*(int *)vector_get_checked(&vec, 0), j, "unchecked set replaces the element");
    vector_free(&vec);
}

void vector_popTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    assertNULL(vector_pop(&vec), "pop on empty vector");
    int i = 1;
    int j = 2;
    vector_push(&vec, &i);
    vector_push(&vec, &j);

    int *value = vector_pop(&vec);
    assertTrue(value != NULL && *value == j, "pop returns the last element");
    assertEquals(vec.num_elements, 1, "pop decrements num_elements");
    value = vector_pop(&vec);
    assertTrue(value != NULL && *value == i, "second pop returns the first element");
    assertEquals(vec.num_elements, 0, "vector is empty after popping everything");
    assertNULL(vector_pop(&vec), "pop after emptying");
    vector_free(&vec);
}

void vector_peekTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    assertNULL(vector_peek(&vec), "peek on empty vector");
    int i = 1;
    int j = 2;
    vector_push(&vec, &i);
    vector_push(&vec, &j);
    int *value = vector_peek(&vec);
    assertTrue(value != NULL && *value == j, "peek returns the last element");
    assertEquals(vec.num_elements, 2, "peek doesn't remove the element");
    vector_free(&vec);
}

void vector_reserveTest() {
    struct vector vec;
    vector_init_size(&vec, sizeof(int), 4);
    int i = 42;
    vector_push(&vec, &i);

    assertEquals(vector_reserve(&vec, 2), 0, "reserve below capacity is a no-op success");
    assertEquals(vec.max_elements, 4, "reserve never shrinks");

    assertEquals(vector_reserve(&vec, 50), 0, "growing reserve returns success");
    assertTrue(vec.max_elements >= 50, "reserve reaches the requested capacity");
    assertEquals(vec.num_elements, 1, "reserve doesn't change num_elements");
    assertEquals(*(int *)vector_get_checked(&vec, 0), i, "reserve keeps existing elements");
    vector_free(&vec);
}

void vector_init_badArgsTest() {
    struct vector vec;
    assertEquals(vector_init(&vec, 0), -1, "zero element size is rejected");
    assertEquals(vector_init_size(&vec, sizeof(int), 0), 0, "zero capacity is accepted");
    int i = 1;
    assertEquals(vector_push(&vec, &i), 0, "push after zero capacity init grows");
    assertEquals(*(int *)vector_get_checked(&vec, 0), i, "element pushed after zero capacity init");
    vector_free(&vec);
}

void vector_push_selfTest() {
    struct vector vec;
    vector_init_size(&vec, sizeof(int), 1);
    int i = 11;
    vector_push(&vec, &i);
    // vector is full so this push reallocs while reading from the old buffer
    assertEquals(vector_push(&vec, vector_get_checked(&vec, 0)), 0, "push an element of the same vector");
    assertEquals(*(int *)vector_get_checked(&vec, 1), i, "self push copies the right value after growth");
    vector_free(&vec);
}

void vector_freeTest() {
    struct vector vec;
    vector_init(&vec, sizeof(int));
    int i = 1;
    vector_push(&vec, &i);
    assertEquals(vector_free(&vec), 0, "free returns success");
    assertNULL(vec.data, "free clears data");
    assertEquals(vec.num_elements, 0, "free empties the vector");
    assertEquals(vector_free(&vec), 0, "double free is safe");
}

void run_vector_tests() {
    printTestingSegment("Vector tests");
    vector_initTest();
    vector_init_sizeTest();
    vector_push_getTest();
    vector_push_copiesTest();
    vector_push_growTest();
    vector_structTest();
    vector_get_checkedTest();
    vector_get_uncheckedTest();
    vector_set_checkedTest();
    vector_set_uncheckedTest();
    vector_popTest();
    vector_peekTest();
    vector_reserveTest();
    vector_init_badArgsTest();
    vector_push_selfTest();
    vector_freeTest();
    allTestsPassing("Vector");
}
