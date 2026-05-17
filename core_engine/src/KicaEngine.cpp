#include "KicaEngine.h"
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

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
    std::fill(state.Delta.begin(), state.Delta.end(), 0);

    for (const auto& edge : state.Edges) {
        int source = (state.tau == 0) ? edge.a : edge.b;
        int target = (state.tau == 0) ? edge.b : edge.a;

        if (state.Phi[source] >= state.Phi[target]) {
            state.Delta[source] -= 1;
            state.Delta[target] += 1;
        }
    }

    // ---------------------------------------------------------
    // (3) 处理阶段 (State Application)
    // ---------------------------------------------------------
    for (int i = 0; i < state.num_nodes; ++i) {
        state.Phi_tilde[i] = state.Phi[i] + state.Delta[i];
    }

    // ---------------------------------------------------------
    // (4) 检测阶段 (Frustration Detection)
    // ---------------------------------------------------------
    std::vector<Edge> surviving_edges;
    std::vector<Edge> broken_edges;
    std::unordered_set<int> rewired_nodes;

    for (const auto& edge : state.Edges) {
        int source = (state.tau == 0) ? edge.a : edge.b;
        int target = (state.tau == 0) ? edge.b : edge.a;

        bool transferred = (state.Phi[source] >= state.Phi[target]);
        bool reversed = (state.Phi_tilde[source] < state.Phi_tilde[target]);

        if (transferred && reversed) {
            broken_edges.push_back(edge);
            // 只有当需要记录时间时，才需要收集拓扑事件的节点
            if (timer != nullptr) {
                rewired_nodes.insert(edge.a);
                rewired_nodes.insert(edge.b);
            }
        } else {
            surviving_edges.push_back(edge);
        }
    }
    state.Edges = surviving_edges;

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
        for (int i = 0; i < state.num_nodes; ++i) {
            bool state_event = (state.Delta[i] != 0);
            bool topo_event = (rewired_nodes.find(i) != rewired_nodes.end());
            
            if (state_event || topo_event) {
                timer->ProperTime[i] += 1;
            }
        }
    }

    // ---------------------------------------------------------
    // (7) 结束阶段 (Cycle Conclusion)
    // ---------------------------------------------------------
    state.Phi = state.Phi_tilde;
    state.tau = 1 - state.tau;
}