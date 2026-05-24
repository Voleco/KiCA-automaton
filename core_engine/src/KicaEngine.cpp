#include "KicaEngine.h"
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

#include <iostream>

namespace KiCA
{

    KicaEngine::KicaEngine(int num_u, int num_v, std::shared_ptr<IRewireStrategy> strategy)
        : rewire_strategy(std::move(strategy))
    {
        if (!rewire_strategy)
        {
            throw std::invalid_argument("Rewire strategy cannot be null");
        }
        // 初始化引擎的工作内存
        Delta_U.resize(num_u, 0);
        Delta_V.resize(num_v, 0);
        Phi_tilde_U.resize(num_u, 0);
        Phi_tilde_V.resize(num_v, 0);

        Degree_Snapshot_U.resize(num_u, 0);
        Degree_Snapshot_V.resize(num_v, 0);
    }

    void KicaEngine::setRewireStrategy(std::shared_ptr<IRewireStrategy> strategy)
    {
        if (!strategy)
        {
            throw std::invalid_argument("Rewire strategy cannot be null");
        }
        rewire_strategy = std::move(strategy);
    }

    inline int calculate_threshold(int n1, int n2)
    {
        return n1 + n2;
    }

    void KicaEngine::step(KicaState &state, KicaTimer *timer)
    {
        // 此处不做安全检查。如果传入的 state 尺寸与 Engine 预分配的不符，那就爆炸吧
        // ---------------------------------------------------------
        // (1) 准备阶段 & (2) 判定阶段 (Direction & Synchronous Evaluation)
        // ---------------------------------------------------------
        std::fill(Delta_U.begin(), Delta_U.begin() + state.num_u, 0);
        std::fill(Delta_V.begin(), Delta_V.begin() + state.num_v, 0);

        std::fill(Degree_Snapshot_U.begin(), Degree_Snapshot_U.begin() + state.num_u, 0);
        std::fill(Degree_Snapshot_V.begin(), Degree_Snapshot_V.begin() + state.num_v, 0);

        if (state.tau == 0) // 节拍 0：因果势从 U 流向 V
        {
            for (const auto &edge : state.Edges)
            {
                Degree_Snapshot_U[edge.u] += 1;
                Degree_Snapshot_V[edge.v] += 1;
                if (state.Phi_U[edge.u] >= state.Phi_V[edge.v])
                {
                    Delta_U[edge.u] -= 1;
                    Delta_V[edge.v] += 1;
                }
            }
        }
        else // 节拍 1：因果势从 V 流向 U
        {
            for (const auto &edge : state.Edges)
            {
                Degree_Snapshot_V[edge.v] += 1;
                Degree_Snapshot_U[edge.u] += 1;
                if (state.Phi_V[edge.v] >= state.Phi_U[edge.u])
                {
                    Delta_V[edge.v] -= 1;
                    Delta_U[edge.u] += 1;
                }
            }
        }

        // ---------------------------------------------------------
        // (3) 处理阶段 (State Application)
        // ---------------------------------------------------------
        for (int i = 0; i < state.num_u; ++i)
        {
            Phi_tilde_U[i] = state.Phi_U[i] + Delta_U[i];
        }
        for (int i = 0; i < state.num_v; ++i)
        {
            Phi_tilde_V[i] = state.Phi_V[i] + Delta_V[i];
        }
        // ---------------------------------------------------------
        // (4) 检测阶段 (Frustration Detection)
        // ---------------------------------------------------------
        std::vector<Edge> surviving_edges;
        std::vector<Edge> broken_edges;

        // 当 timer 存在时，才分配并清零标记数组
        std::vector<uint8_t> rewired_nodes_u;
        std::vector<uint8_t> rewired_nodes_v;
        if (timer != nullptr)
        {
            rewired_nodes_u.assign(state.num_u, 0);
            rewired_nodes_v.assign(state.num_v, 0);
        }

        // 预分配内存，避免 vector 动态扩容开销
        surviving_edges.reserve(state.Edges.size());

        if (state.tau == 0)
        {
            for (const auto &edge : state.Edges)
            {
                // 没有转移 -- 无事发生
                if (state.Phi_U[edge.u] < state.Phi_V[edge.v])
                {
                    surviving_edges.push_back(edge);
                    continue;
                }

                int threshold = calculate_threshold(Degree_Snapshot_U[edge.u], Degree_Snapshot_V[edge.v]);

                // 使用引擎内部的 Phi_tilde
                bool frustrated = (Phi_tilde_V[edge.v] - Phi_tilde_U[edge.u] >= threshold);

                if (frustrated)
                {
                    broken_edges.push_back(edge);
                    if (timer != nullptr)
                    {
                        rewired_nodes_u[edge.u] = 1;
                        rewired_nodes_v[edge.v] = 1;
                    }
                }
                else
                {
                    surviving_edges.push_back(edge);
                }
            }
        }
        else
        {
            for (const auto &edge : state.Edges)
            {
                if (state.Phi_V[edge.v] < state.Phi_U[edge.u])
                {
                    surviving_edges.push_back(edge);
                    continue;
                }
                int threshold = calculate_threshold(Degree_Snapshot_V[edge.v], Degree_Snapshot_U[edge.u]);

                // 使用引擎内部的 Phi_tilde
                bool frustrated = (Phi_tilde_U[edge.u] - Phi_tilde_V[edge.v] >= threshold);

                if (frustrated)
                {
                    broken_edges.push_back(edge);
                    if (timer != nullptr)
                    {
                        rewired_nodes_u[edge.u] = 1;
                        rewired_nodes_v[edge.v] = 1;
                    }
                }
                else
                {
                    surviving_edges.push_back(edge);
                }
            }
        }

        // 使用 move 语义，避免拷贝
        state.Edges = std::move(surviving_edges);

        // ---------------------------------------------------------
        // (5) 重新匹配阶段 (Topological Rewiring)
        // ---------------------------------------------------------
        std::cout << "broken_edges count: " << broken_edges.size() << "\n";
        if (!broken_edges.empty())
        {
            // 委托给注入的策略进行处理
            rewire_strategy->rewire(state, broken_edges);
        }

        // ---------------------------------------------------------
        // (6) 局部原时更新阶段 (Proper Time Evolution) - 可选
        // ---------------------------------------------------------
        if (timer != nullptr)
        {
            auto update_proper_time = [](int num_nodes, const std::vector<int> &delta,
                                         const std::vector<uint8_t> &rewired_nodes,
                                         std::vector<int> &proper_time)
            {
                for (int i = 0; i < num_nodes; ++i)
                {
                    if (delta[i] != 0 || rewired_nodes[i])
                        proper_time[i] += 1;
                }
            };

            update_proper_time(state.num_u, Delta_U, rewired_nodes_u, timer->ProperTime_U);
            update_proper_time(state.num_v, Delta_V, rewired_nodes_v, timer->ProperTime_V);
        }

        // ---------------------------------------------------------
        // (7) 结束阶段 (Cycle Conclusion)
        // ---------------------------------------------------------
        // 这里使用 std::copy 以避免 vector 的重新分配
        std::copy(Phi_tilde_U.begin(), Phi_tilde_U.begin() + state.num_u, state.Phi_U.begin());
        std::copy(Phi_tilde_V.begin(), Phi_tilde_V.begin() + state.num_v, state.Phi_V.begin());

        state.tau = 1 - state.tau;
    }

}