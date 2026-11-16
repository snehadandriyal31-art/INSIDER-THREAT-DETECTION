/* stack.h — LIFO stack of ActionRecord (analyst undo). Owner: M1/M2. */
#ifndef STACK_H
#define STACK_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StackNode {
    ActionRecord      data;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
    size_t     size;
} Stack;

Stack              *stack_create(void);
int                 stack_push(Stack *s, const ActionRecord *r);   /* 1 ok */
int                 stack_pop(Stack *s, ActionRecord *out);        /* 1 ok, 0 if empty */
const ActionRecord *stack_peek(const Stack *s);                    /* NULL if empty */
int                 stack_is_empty(const Stack *s);
size_t              stack_size(const Stack *s);
void                stack_free(Stack *s);

#ifdef __cplusplus
}
#endif
#endif
