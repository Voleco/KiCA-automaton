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

        std::fill(Delta_U.begin(), Delta_U.begin() + state.num_u, 0);
        std::fill(Delta_V.begin(), Delta_V.begin() + state.num_v, 0);
        std::fill(Degree_Snapshot_U.begin(), Degree_Snapshot_U.begin() + state.num_u, 0);
        std::fill(Degree_Snapshot_V.begin(), Degree_Snapshot_V.begin() + state.num_v, 0);

        std::vector<uint8_t> rewired_nodes_u;
        std::vector<uint8_t> rewired_nodes_v;
        if (timer != nullptr)
        {
            rewired_nodes_u.assign(state.num_u, 0);
            rewired_nodes_v.assign(state.num_v, 0);
        }

        // =========================================================
        // 攻防角色绑定 (Attacker & Defender Binding)
        // =========================================================
        // 根据 tau 的值，动态映射进攻方和防守方的数组引用
        auto &Phi_Atk = state.tau == 0 ? state.Phi_U : state.Phi_V;
        auto &Phi_Def = state.tau == 0 ? state.Phi_V : state.Phi_U;
        auto &Delta_Atk = state.tau == 0 ? Delta_U : Delta_V;
        auto &Delta_Def = state.tau == 0 ? Delta_V : Delta_U;
        auto &Deg_Atk = state.tau == 0 ? Degree_Snapshot_U : Degree_Snapshot_V;
        auto &Deg_Def = state.tau == 0 ? Degree_Snapshot_V : Degree_Snapshot_U;
        auto &Phi_tilde_Atk = state.tau == 0 ? Phi_tilde_U : Phi_tilde_V;
        auto &Phi_tilde_Def = state.tau == 0 ? Phi_tilde_V : Phi_tilde_U;
        auto &rewired_Atk = state.tau == 0 ? rewired_nodes_u : rewired_nodes_v;
        auto &rewired_Def = state.tau == 0 ? rewired_nodes_v : rewired_nodes_u;

        // 辅助 Lambda：从 Edge 中提取当前进攻方和防守方的节点索引
        auto get_roles = [tau = state.tau](const Edge &e) -> std::pair<int, int>
        {
            return tau == 0 ? std::make_pair(e.u, e.v) : std::make_pair(e.v, e.u);
        };

        // ---------------------------------------------------------
        // (1) 准备阶段 & (2) 判定阶段 (Direction & Synchronous Evaluation)
        // ---------------------------------------------------------
        for (const auto &edge : state.Edges)
        {
            auto [atk, def] = get_roles(edge);

            Deg_Atk[atk] += 1;
            Deg_Def[def] += 1;

            if (Phi_Atk[atk] >= Phi_Def[def])
            {
                Delta_Atk[atk] -= 1;
                Delta_Def[def] += 1;
            }
        }

        // ---------------------------------------------------------
        // (3) 处理阶段 (State Application)
        // ---------------------------------------------------------
        // 这一步无论 tau 是多少，U 和 V 都要更新，所以直接按原样写
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
        surviving_edges.reserve(state.Edges.size());

        for (const auto &edge : state.Edges)
        {
            auto [atk, def] = get_roles(edge);

            // 没有发生因果势转移 -- 无事发生
            if (Phi_Atk[atk] < Phi_Def[def])
            {
                surviving_edges.push_back(edge);
                continue;
            }

            int threshold = calculate_threshold(Deg_Atk[atk], Deg_Def[def]);

            // 发生受挫 (Frustration)：防守方的势能反超进攻方达到阈值
            bool frustrated = (Phi_tilde_Def[def] - Phi_tilde_Atk[atk] >= threshold);

            if (frustrated)
            {
                broken_edges.push_back(edge);
                if (timer != nullptr)
                {
                    rewired_Atk[atk] = 1;
                    rewired_Def[def] = 1;
                }
            }
            else
            {
                surviving_edges.push_back(edge);
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