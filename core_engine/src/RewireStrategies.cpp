#include "RewireStrategies.h"
#include <algorithm>
#include <random>

namespace KiCA
{

    RandomRewireStrategy::RandomRewireStrategy()
    {
        std::random_device rd;
        rng.seed(rd());
    }

    void RandomRewireStrategy::rewire(KicaState &state, const std::vector<Edge> &broken_edges)
    {
        if (broken_edges.empty())
            return;

        std::vector<Edge> pool = broken_edges;

        // 打乱断裂边的顺序 (假设 rng 是类成员变量)
        std::shuffle(pool.begin(), pool.end(), rng);

        size_t i = 0;
        while (i + 1 < pool.size())
        {
            const Edge &e1 = pool[i];
            const Edge &e2 = pool[i + 1];

            // 交叉重连：保证 u 始终来自集合 U，v 始终来自集合 V
            // e1.u 连 e2.v，e2.u 连 e1.v
            state.Edges.push_back({e1.u, e2.v});
            state.Edges.push_back({e2.u, e1.v});

            i += 2;
        }

        // 处理落单的边（如果有奇数条断裂边，最后一条原样接回）
        if (i < pool.size())
        {
            state.Edges.push_back(pool[i]);
        }
    }

    void OrderedRewireStrategy::rewire(KicaState &state, const std::vector<Edge> &broken_edges)
    {
        if (broken_edges.empty())
            return;

        std::vector<int> u_nodes;
        std::vector<int> v_nodes;
        u_nodes.reserve(broken_edges.size());
        v_nodes.reserve(broken_edges.size());

        // 1. 提取断裂节点
        for (const auto &edge : broken_edges)
        {
            u_nodes.push_back(edge.u);
            v_nodes.push_back(edge.v);
        }

        // 2. 对 U 集合的断裂节点，按照 Phi_U 从大到小降序排列
        std::sort(u_nodes.begin(), u_nodes.end(), [&state](int a, int b)
                  {
        if (state.Phi_U[a] == state.Phi_U[b]) {
            return a < b; // 保证稳定排序
        }
        return state.Phi_U[a] > state.Phi_U[b]; });

        // 3. 对 V 集合的断裂节点，按照 Phi_V 从大到小降序排列
        std::sort(v_nodes.begin(), v_nodes.end(), [&state](int a, int b)
                  {
        if (state.Phi_V[a] == state.Phi_V[b]) {
            return a < b; // 保证稳定排序
        }
        return state.Phi_V[a] > state.Phi_V[b]; });

        // 4. 根据当前模式进行连边
        size_t num_broken = broken_edges.size();
        for (size_t i = 0; i < num_broken; ++i)
        {
            if (current_mode == Mode::Ordered) // 顺序：最大的 U 连最大的 V
                state.Edges.push_back({u_nodes[i], v_nodes[i]});
            else // 逆序：最大的 U 连最小的 V
                state.Edges.push_back({u_nodes[i], v_nodes[num_broken - 1 - i]});
        }
    }
}