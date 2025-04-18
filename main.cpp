#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>
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
namespace list
{
template <typename T> struct node
{
    T val;
    node *next;
    node() : val(0), next(nullptr) {}
    node(T x) : val(x), next(nullptr) {}
    node(T x, node *next) : val(x), next(next) {}
};

template <typename> struct is_node : std::false_type
{
};
template <typename T> struct is_node<node<T>> : std::true_type
{
};
template <typename T> inline constexpr bool is_node_v = is_node<T>::value;

namespace algorithm
{
template <typename T>
auto splice_list(node<T> **lastNode, node<T> *newHead)
    -> std::pair<node<T> **, size_t>
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

template <typename T>
concept nodeItr = is_node_v<std::remove_pointer_t<std::iter_value_t<T>>>;

template <nodeItr Iter> using node_ptr = std::iter_value_t<Iter>;

template <nodeItr Iter>
auto flatten_list(Iter list_begin, Iter list_end)
    -> std::pair<node_ptr<Iter>, size_t>
{
    using node_t = std::remove_pointer_t<node_ptr<Iter>>;

    while (list_begin != list_end && *list_begin == nullptr)
        ++list_begin;
    if (list_begin == list_end)
        return {nullptr, 0};

    node_t head{};
    node_ptr<Iter> dummy_ptr = &head;
    node_ptr<Iter> *lastNode = &(dummy_ptr->next);
    size_t n                 = 0;

    for (; list_begin != list_end; ++list_begin)
    {
        auto [tail, len] = splice_list(lastNode, *list_begin);
        lastNode         = tail;
        n += len;
    }

    return {head.next, n};
}

template <typename T> auto merge(node<T> *l1, node<T> *l2) -> node<T> *
{
    node<T> dummy;
    node<T> *tail = &dummy;

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

template <typename T> auto merge_sort(node<T> *head, size_t n) -> node<T> *
{
    if (n <= 1)
        return head;
    size_t mid = n / 2;

    node<T> *left  = head;
    node<T> *right = head;
    node<T> *prev  = nullptr;

    for (size_t i = 0; i < mid; ++i)
    {
        prev  = right;
        right = right->next;
    }
    prev->next = nullptr;

    return merge(merge_sort(left, mid), merge_sort(right, n - mid));
}

template <typename T>
auto merge_k_lists(std::vector<node<T> *> &lists) -> node<T> *
{
    if (lists.size() <= 0)
        return nullptr;

    auto [flatListHead, n] = flatten_list(lists.begin(), lists.end());
    return merge_sort(flatListHead, n);
}
}; // namespace algorithm

namespace detail
{
template <typename T> auto print_linked_list(node<T> *head) -> void
{
    const char *sep = "[";
    if (head == nullptr)
        std::cout << sep;

    std::unordered_set<list::node<T> *> visited_nodes;
    for (; head != nullptr && visited_nodes.find(head) == visited_nodes.end();
         head = head->next)
    {
        std::cout << sep << head->val << "(" << head << ")";
        sep = " -> ";
        visited_nodes.insert(head);
    }

    if (head != nullptr)
    {
        std::cerr << "\nPANIC: Unexpected circular linked list "
                     "detected!\nAborting printing operation."
                  << std::endl;
    }
    std::cout << "]";
}

template <typename T> auto gc(node<T> *&head) -> void
{
    node<T> *cur = nullptr;
    while (head != nullptr)
    {
        cur  = head;
        head = head->next;
        delete cur;
        cur = nullptr;
    }
    head = nullptr;
}

template <typename T> auto add_node(node<T> *&head, int val) -> node<T> *&
{
    node<T> *new_node = new list::node<T>(val);
    new_node->next    = head;
    head              = new_node;
    return head;
}

template <typename T> auto make_list(std::initializer_list<T> vals) -> node<T> *
{
    node<T> *head              = nullptr;
    node<T> **head_indirection = &head;
    for (const auto &val : vals)
    {
        *head_indirection = new node<T>(val);
        head_indirection  = &((*head_indirection)->next);
    }
    return head;
}
}; // namespace detail
} // namespace list

auto main() -> int
{
    std::vector<list::node<int> *> lists;
    defer
    {
        for (auto &node : lists)
            list::detail::gc(node);
        lists.clear();
    };

    lists.emplace_back(list::detail::make_list({1, 4, 5}));
    lists.emplace_back(list::detail::make_list({1, 3, 4}));
    lists.emplace_back(list::detail::make_list({2, 6}));

    std::cout << "input to mergeKLists\n";
    for (const auto &head : lists)
    {
        list::detail::print_linked_list(head);
        std::cout << std::endl;
    }

    std::cout << "\noutput\n";
    list::detail::print_linked_list(list::algorithm::merge_k_lists(lists));
    std::cout << std::endl;
    return 0;
}
