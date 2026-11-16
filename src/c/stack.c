/* stack.c — linked LIFO stack (undo of analyst actions). Owner: M2. */
#include "stack.h"
#include <stdlib.h>

Stack *stack_create(void) {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    if (!s) return NULL;
    s->top = NULL;
    s->size = 0;
    return s;
}

int stack_push(Stack *s, const ActionRecord *r) {
    if (!s || !r) return 0;
    StackNode *n = (StackNode *)malloc(sizeof(StackNode));
    if (!n) return 0;
    n->data = *r;
    n->next = s->top;
    s->top = n;
    s->size++;
    return 1;
}

int stack_pop(Stack *s, ActionRecord *out) {
    if (!s || !s->top) return 0;
    StackNode *n = s->top;
    if (out) *out = n->data;
    s->top = n->next;
    free(n);
    s->size--;
    return 1;
}

const ActionRecord *stack_peek(const Stack *s) { return (s && s->top) ? &s->top->data : NULL; }
int    stack_is_empty(const Stack *s)          { return !s || s->top == NULL; }
size_t stack_size(const Stack *s)              { return s ? s->size : 0; }

void stack_free(Stack *s) {
    if (!s) return;
    while (s->top) { StackNode *n = s->top; s->top = n->next; free(n); }
    free(s);
}
