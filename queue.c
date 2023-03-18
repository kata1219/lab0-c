#include <stdint.h>
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
    element_t *el, *safe;

    if (!l)
        return;
    list_for_each_entry_safe (el, safe, l, list)
        q_release_element(el);
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
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
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
    if (!head)
        return 0;
    int count = 0;
    struct list_head *node;

    list_for_each (node, head)
        count++;

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head **indir = &head->next;

    for (struct list_head *fast = head->next->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;
    element_t *el = list_entry(*indir, element_t, list);
    list_del(*indir);
    q_release_element(el);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *never = q_new();
    struct list_head *once = q_new();
    element_t *el = NULL;

    list_for_each_entry (el, head, list) {
        for (struct list_head *found = never->next;; found = found->next) {
            if (found == never) {
                q_insert_head(never, el->value);
                break;
            }
            if (strcmp(el->value, list_entry(found, element_t, list)->value))
                continue;
            for (struct list_head *dup = once->next;; dup = dup->next) {
                if (dup == once) {
                    q_insert_head(once, el->value);
                    break;
                }
                if (!strcmp(list_entry(found, element_t, list)->value,
                            list_entry(dup, element_t, list)->value))
                    break;
            }
            break;
        }
    }
    for (struct list_head *tmp = head->next, *front = tmp->next; tmp != head;
         tmp = front, front = front->next) {
        el = list_entry(tmp, element_t, list);
        for (struct list_head *dup = once->next; dup != once; dup = dup->next) {
            if (!strcmp(el->value, list_entry(dup, element_t, list)->value)) {
                list_del(tmp);
                q_release_element(el);
                break;
            }
        }
    }

    q_free(never);
    q_free(once);
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    for (struct list_head *curr = head->next, *next = curr->next; curr != head;
         curr = next, next = next->next) {
        list_move(curr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    struct list_head *curr, *next;
    int size = q_size(head), round = size / k, i,
        pos = k * (size / k - 1) + size % k;

    if (size % k) {
        for (i = 0, curr = head->prev, next = curr->prev; i < size % k;
             i++, curr = next, next = next->prev) {
            list_move(curr, head);
        }
    }
    while (round-- > 0) {
        // Reverse starting from the last group until the first group
        curr = head->next;
        for (i = 0; i < pos; i++) {
            curr = curr->next;
        }
        for (next = curr->next; curr != head; curr = next, next = next->next) {
            list_move(curr, head);
        }
    }
}

static struct list_head *mergeTwoLists(struct list_head *L1,
                                       struct list_head *L2)
{
    struct list_head *head = L1;
    struct list_head *rhead = L2;
    struct list_head **node = &head, *tmp = NULL;

    L1 = L1->next;
    L2 = L2->next;
    list_del_init(head);
    list_del_init(rhead);

    while ((*node) != tmp) {
        node = strcmp(list_entry(L1, element_t, list)->value,
                      list_entry(L2, element_t, list)->value) < 0
                   ? &L1
                   : &L2;
        tmp = *node;
        *node = (*node)->next;
        list_del_init(tmp);
        list_add_tail(tmp, head);
    }

    node = (*node == L1) ? &L2 : &L1;
    do {
        tmp = *node;
        *node = (*node)->next;
        list_del_init(tmp);
        list_add_tail(tmp, head);
    } while ((*node) != tmp);

    return head;
}

static struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return head;
    struct list_head *slow = head->next;

    for (struct list_head *fast = head->next->next;
         fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;

    slow->next = head;
    mid->prev = head->prev;
    head->prev->next = mid;
    head->prev = slow;

    LIST_HEAD(r_head);
    list_add(&r_head, mid);
    struct list_head *left = mergesort_list(head),
                     *right = mergesort_list(&r_head);

    return mergeTwoLists(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    mergesort_list(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *el, *safe;
    int count = q_size(head);

    list_for_each_entry_safe (el, safe, head, list) {
        for (struct list_head *tmp = (&el->list)->next; tmp != head;
             tmp = tmp->next) {
            if (strcmp(el->value, list_entry(tmp, element_t, list)->value) >
                0) {
                list_del(&el->list);
                q_release_element(el);
                count--;
                break;
            }
        }
    }

    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *el, *safe;
    int count = q_size(head);

    list_for_each_entry_safe (el, safe, head, list) {
        for (struct list_head *tmp = (&el->list)->next; tmp != head;
             tmp = tmp->next) {
            if (strcmp(el->value, list_entry(tmp, element_t, list)->value) <
                0) {
                list_del(&el->list);
                q_release_element(el);
                count--;
                break;
            }
        }
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    for (struct list_head *tmp = head->next->next; tmp != head;
         tmp = tmp->next) {
        queue_contex_t *chain = list_entry(tmp, queue_contex_t, chain);
        mergeTwoLists(first->q, chain->q);
        first->size += chain->size;
    }

    return first->size;
}
