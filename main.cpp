/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */

#include <vector>
struct ListNode
{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution
{
    std::pair<ListNode **, size_t> splice_list(ListNode **lastNode,
                                               ListNode *newHead)
    {
        size_t n    = 0;
        *(lastNode) = newHead;
        while (*lastNode != nullptr)
        {
            lastNode = &((*lastNode)->next);
            ++n;
        }

        return {lastNode, n};
    }

    template <typename Iter>
        requires requires(Iter it) {
            { *it } -> std::convertible_to<ListNode *>;
        }
    std::pair<ListNode *, size_t> flatten_list(Iter list_begin, Iter list_end)
    {
        while (list_begin != list_end && *list_begin == nullptr)
            ++list_begin;
        if (list_begin == list_end)
            return {nullptr, 0};

        ListNode head{};
        ListNode *dummy_ptr = &head;
        ListNode **lastNode = &(dummy_ptr->next);
        size_t n            = 0;

        for (; list_begin != list_end; ++list_begin)
        {
            auto [tail, len] = splice_list(lastNode, *list_begin);
            lastNode         = tail;
            n += len;
        }

        return {head.next, n};
    }

    ListNode *merge(ListNode *l1, ListNode *l2)
    {
        ListNode dummy;
        ListNode *tail = &dummy;

        while (l1 != nullptr && l2 != nullptr)
        {
            if (l1->val < l2->val)
            {
                tail->next = l1;
                l1         = l1->next;
            }
            else
            {
                tail->next = l2;
                l2         = l2->next;
            }
            tail = tail->next;
        }
        tail->next = (l1 != nullptr) ? l1 : l2;
        return dummy.next;
    }
    ListNode *merge_sort(ListNode *head, size_t n)
    {
        if (n <= 1)
            return head;
        size_t mid = n / 2;

        ListNode *left  = head;
        ListNode *right = head;
        ListNode *prev  = nullptr;

        for (size_t i = 0; i < mid; ++i)
        {
            prev  = right;
            right = right->next;
        }
        prev->next = nullptr;

        return merge(merge_sort(left, mid), merge_sort(right, n - mid));
    }

public:
    ListNode *mergeKLists(std::vector<ListNode *> &lists)
    {
        if (lists.size() <= 0)
            return nullptr;

        auto [flatListHead, n] = flatten_list(lists.begin(), lists.end());
        return merge_sort(flatListHead, n);
    }
};

int main() { return 0; }
