#pragma once

#include <vector>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <assert.h>
#include "KicaDS.h"

namespace KiCA::GraphUtils
{
    // Largest Connected Component(LCC)
    struct LCCInfo
    {
        int root;          // LCC 的 DSU 根节点
        int size;          // LCC 的节点总数
        int edge_count;    // LCC 内部的边数
        double avg_degree; // LCC 的平均度数
        int u_count;       // LCC 中 U 集合的节点数
        int v_count;       // LCC 中 V 集合的节点数
    };

    // 并查集 (用于高效计算连通分量);
    class DSU
    {
    public:
        // 构造函数：初始化 n 个元素的并查集
        explicit DSU(int n)
            : parent(n), component_size(n, 1), component_count(n)
        {
            // 使用 std::iota 快速填充 0, 1, 2, ..., n-1
            std::iota(parent.begin(), parent.end(), 0);
        }

        // 查找元素 i 所属的集合的根节点
        int find(int i) const;

        // 合并元素 i 和 j 所属的集合
        // 返回值：如果发生合并返回 true；如果它们已经在同一集合则返回 false
        bool unite(int i, int j);

        // 判断元素 i 和 j 是否在同一个集合中
        bool is_connected(int i, int j) { return find(i) == find(j); }

        // 返回元素 i 所在的 component 的 size
        int get_component_size(int i) const { return component_size[find(i)]; }
        int get_component_count() const { return component_count; }

    private:
        // 这里逻辑实质不改变图本身的性质; 所以谨慎引入 mutable, 允许在 const 成员函数中被修改
        mutable std::vector<int> parent;
        std::vector<int> component_size;
        int component_count;
    };

}