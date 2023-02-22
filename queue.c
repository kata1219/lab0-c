#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head) {
        free(head);
        return NULL;
    }

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    struct list_head *head = l->next;

    while (head != l) {
        struct list_head *tmp = head->next;
        element_t *el = list_entry(head, element_t, list);
        q_release_element(el);
        head = tmp;
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *el = malloc(sizeof(element_t));
    if (!el || !head)
        return false;

    el->value = malloc(strlen(s) + 1);
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, strlen(s) + 1);
    list_add(&el->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *el = malloc(sizeof(element_t));
    if (!el || !head)
        return false;

    el->value = malloc(strlen(s) + 1);
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, strlen(s) + 1);
    list_add_tail(&el->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;

    element_t *el = list_first_entry(head, element_t, list);
    list_del(&el->list);

    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return el;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;

    element_t *el = list_last_entry(head, element_t, list);
    list_del(&el->list);

    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return el;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;

    struct list_head *tmp = head->next;
    int count = 1;
    while (tmp->next != head) {
        tmp = tmp->next;
        count++;
    }

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || head->next == head)
        return false;

    int mid = q_size(head) / 2;
    struct list_head *tmp = head->next;
    while (mid-- > 0)
        tmp = tmp->next;

    element_t *el = list_entry(tmp, element_t, list);
    list_del(tmp);
    q_release_element(el);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
