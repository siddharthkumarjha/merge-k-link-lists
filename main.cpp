#include <functional>
#include <iostream>
#include <unordered_set>
#include <vector>

/*
 * go like defer
 * uses RAII syntax to defer operations to the end of scope
 * defers in LIFO
 *
 * eg: code,
 *
 * defer { cleanup1 };
 * defer { cleanup2 };
 *
 * executes,
 *
 * cleanup2
 * cleanup1
 */
class defer_impl
{
    std::function<void(void)> m_Func;

public:
    defer_impl() = delete;
    defer_impl(auto &&func) : m_Func(std::move(func)) {}
    defer_impl &operator=(auto &&func)
    {
        m_Func = std::move(func);
        return *this;
    }

    ~defer_impl()
    {
        if (m_Func)
            m_Func();
    }
};

#define CAT2(x, y) x##y
#define CAT(x, y) CAT2(x, y)
#define DEFER_OBJ_NAME CAT(defer_call_, __LINE__)
#define defer defer_impl DEFER_OBJ_NAME = [&]() -> void

/**
 * Definition for singly-linked list.
 */
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

void print_linked_list(ListNode *node)
{
    const char *sep = "[";
    if (node == nullptr)
        std::cout << sep;

    std::unordered_set<ListNode *> visited_nodes;
    for (; node != nullptr && visited_nodes.find(node) == visited_nodes.end();
         node = node->next)
    {
        std::cout << sep << node->val << "(" << node << ")";
        sep = " -> ";
        visited_nodes.insert(node);
    }

    if (node != nullptr)
    {
        std::cerr << "\nPANIC: Unexpected circular linked list "
                     "detected!\nAborting printing operation."
                  << std::endl;
    }
    std::cout << "]";
}

void gc(ListNode *&head)
{
    ListNode *cur = nullptr;
    while (head != nullptr)
    {
        cur  = head;
        head = head->next;
        delete cur;
        cur = nullptr;
    }
    head = nullptr;
}

ListNode *&add_node(ListNode *&head, int val)
{
    ListNode *node = new ListNode(val);
    node->next     = head;
    head           = node;
    return head;
}

ListNode *make_list(std::initializer_list<int> vals)
{
    ListNode *head              = nullptr;
    ListNode **head_indirection = &head;
    for (const auto &val : vals)
    {
        *head_indirection = new ListNode(val);
        head_indirection  = &((*head_indirection)->next);
    }
    return head;
}

int main()
{
    std::vector<ListNode *> lists;
    defer
    {
        for (auto &node : lists)
            gc(node);
        lists.clear();
    };

    lists.emplace_back(make_list({1, 4, 5}));
    lists.emplace_back(make_list({1, 3, 4}));
    lists.emplace_back(make_list({2, 6}));

    std::cout << "input to mergeKLists\n";
    for (const auto &head : lists)
    {
        print_linked_list(head);
        std::cout << std::endl;
    }

    std::cout << "\noutput\n";
    Solution s;
    print_linked_list(s.mergeKLists(lists));
    std::cout << std::endl;
    return 0;
}
