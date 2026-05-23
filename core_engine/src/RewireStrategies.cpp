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

        // 预计算度数
        std::vector<int> deg_U;
        std::vector<int> deg_V;

        if (current_metric == Metric::PhiPerDegree)
        {
            deg_U.assign(state.num_u, 0);
            deg_V.assign(state.num_v, 0);

            // 统计存活边的度数
            for (const auto &edge : state.Edges)
            {
                deg_U[edge.u]++;
                deg_V[edge.v]++;
            }
            // // 统计断裂边的度数 (重连前后总度数守恒)
            // for (const auto &edge : broken_edges)
            // {
            //     deg_U[edge.u]++;
            //     deg_V[edge.v]++;
            // }
        }

        // 对 U 集合排序
        std::sort(u_nodes.begin(), u_nodes.end(), [&state, &deg_U, this](int a, int b)
                  {
        if (current_metric == Metric::AbsolutePhi) {
            if (state.Phi_U[a] == state.Phi_U[b]) return a < b;
            return state.Phi_U[a] > state.Phi_U[b];
        } else {
            // 比较 Phi_a / (k_a + 1) > Phi_b / (k_b + 1)
            // 转为乘法：Phi_a * (k_b + 1) > Phi_b * (k_a + 1)
            // 使用 long long 防止乘法溢出
            long long val_a = static_cast<long long>(state.Phi_U[a]) * (deg_U[b] + 1);
            long long val_b = static_cast<long long>(state.Phi_U[b]) * (deg_U[a] + 1);
            
            if (val_a == val_b) return a < b;
            return val_a > val_b;
        } });

        // 对 V 集合排序
        std::sort(v_nodes.begin(), v_nodes.end(), [&state, &deg_V, this](int a, int b)
                  {
        if (current_metric == Metric::AbsolutePhi) {
            if (state.Phi_V[a] == state.Phi_V[b]) return a < b;
            return state.Phi_V[a] > state.Phi_V[b];
        } else {
            long long val_a = static_cast<long long>(state.Phi_V[a]) * (deg_V[b] + 1);
            long long val_b = static_cast<long long>(state.Phi_V[b]) * (deg_V[a] + 1);
            
            if (val_a == val_b) return a < b;
            return val_a > val_b;
        } });

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