#include "KicaEngine.h"
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

namespace KiCA
{

    

KicaEngine::KicaEngine(std::shared_ptr<IRewireStrategy> strategy) 
    : rewire_strategy(std::move(strategy)) {
    if (!rewire_strategy) {
        throw std::invalid_argument("Rewire strategy cannot be null");
    }
}

void KicaEngine::setRewireStrategy(std::shared_ptr<IRewireStrategy> strategy) {
    if (!strategy) {
        throw std::invalid_argument("Rewire strategy cannot be null");
    }
    rewire_strategy = std::move(strategy);
}

void KicaEngine::step(KicaState& state, KicaTimer* timer) {
    // ---------------------------------------------------------
    // (1) 准备阶段 & (2) 判定阶段 (Direction & Synchronous Evaluation)
    // ---------------------------------------------------------
    std::fill(state.Delta_U.begin(), state.Delta_U.end(), 0);
    std::fill(state.Delta_V.begin(), state.Delta_V.end(), 0);

    // 【性能优化】：将 tau 的判断提到循环外，避免百万次的分支预测开销
    if (state.tau == 0) {
        // 节拍 0：因果势从 U 流向 V
        for (const auto& edge : state.Edges) {
            if (state.Phi_U[edge.u] >= state.Phi_V[edge.v]) {
                state.Delta_U[edge.u] -= 1;
                state.Delta_V[edge.v] += 1;
            }
        }
    } else {
        // 节拍 1：因果势从 V 流向 U
        for (const auto& edge : state.Edges) {
            if (state.Phi_V[edge.v] >= state.Phi_U[edge.u]) {
                state.Delta_V[edge.v] -= 1;
                state.Delta_U[edge.u] += 1;
            }
        }
    }

    // ---------------------------------------------------------
    // (3) 处理阶段 (State Application)
    // ---------------------------------------------------------
    for (int i = 0; i < state.num_u; ++i) {
        state.Phi_tilde_U[i] = state.Phi_U[i] + state.Delta_U[i];
    }
    for (int i = 0; i < state.num_v; ++i) {
        state.Phi_tilde_V[i] = state.Phi_V[i] + state.Delta_V[i];
    }

    // ---------------------------------------------------------
    // (4) 检测阶段 (Frustration Detection)
    // ---------------------------------------------------------
    std::vector<Edge> surviving_edges;
    std::vector<Edge> broken_edges;
    
    // 同样为二分图拆分重连节点记录
    std::unordered_set<int> rewired_nodes_u;
    std::unordered_set<int> rewired_nodes_v;

    // 预分配内存，避免 vector 动态扩容开销
    surviving_edges.reserve(state.Edges.size());

    if (state.tau == 0) {
        for (const auto& edge : state.Edges) {
            bool transferred = (state.Phi_U[edge.u] >= state.Phi_V[edge.v]);
            bool reversed = (state.Phi_tilde_U[edge.u] < state.Phi_tilde_V[edge.v]);

            if (transferred && reversed) {
                broken_edges.push_back(edge);
                if (timer != nullptr) {
                    rewired_nodes_u.insert(edge.u);
                    rewired_nodes_v.insert(edge.v);
                }
            } else {
                surviving_edges.push_back(edge);
            }
        }
    } else {
        for (const auto& edge : state.Edges) {
            bool transferred = (state.Phi_V[edge.v] >= state.Phi_U[edge.u]);
            bool reversed = (state.Phi_tilde_V[edge.v] < state.Phi_tilde_U[edge.u]);

            if (transferred && reversed) {
                broken_edges.push_back(edge);
                if (timer != nullptr) {
                    rewired_nodes_u.insert(edge.u);
                    rewired_nodes_v.insert(edge.v);
                }
            } else {
                surviving_edges.push_back(edge);
            }
        }
    }
    
    // 使用 move 语义，避免拷贝
    state.Edges = std::move(surviving_edges);

    // ---------------------------------------------------------
    // (5) 重新匹配阶段 (Topological Rewiring)
    // ---------------------------------------------------------
    if (!broken_edges.empty()) {
        // 委托给注入的策略进行处理
        rewire_strategy->rewire(state, broken_edges);
    }

    // ---------------------------------------------------------
    // (6) 局部原时更新阶段 (Proper Time Evolution) - 可选
    // ---------------------------------------------------------
    if (timer != nullptr) {
        // 注意：这里假设你的 KicaTimer 也相应地拆分为了 ProperTime_U 和 ProperTime_V
        for (int i = 0; i < state.num_u; ++i) {
            bool state_event = (state.Delta_U[i] != 0);
            bool topo_event = (rewired_nodes_u.find(i) != rewired_nodes_u.end());
            
            if (state_event || topo_event) {
                timer->ProperTime_U[i] += 1;
            }
        }
        for (int i = 0; i < state.num_v; ++i) {
            bool state_event = (state.Delta_V[i] != 0);
            bool topo_event = (rewired_nodes_v.find(i) != rewired_nodes_v.end());
            
            if (state_event || topo_event) {
                timer->ProperTime_V[i] += 1;
            }
        }
    }

    // ---------------------------------------------------------
    // (7) 结束阶段 (Cycle Conclusion)
    // ---------------------------------------------------------
    state.Phi_U = state.Phi_tilde_U;
    state.Phi_V = state.Phi_tilde_V;
    state.tau = 1 - state.tau;
}

}