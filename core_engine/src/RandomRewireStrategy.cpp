#include "RewireStrategies.h"
#include <algorithm>
#include <random>

namespace KiCA
{
    
RandomRewireStrategy::RandomRewireStrategy() {
    std::random_device rd;
    rng.seed(rd());
}

void RandomRewireStrategy::rewire(KicaState& state, const std::vector<Edge>& broken_edges) {
    if (broken_edges.empty()) return;

    std::vector<Edge> pool = broken_edges;
    
    // 打乱断裂边的顺序 (假设 rng 是类成员变量)
    std::shuffle(pool.begin(), pool.end(), rng);

    size_t i = 0;
    while (i + 1 < pool.size()) {
        const Edge& e1 = pool[i];
        const Edge& e2 = pool[i+1];

        // 交叉重连：保证 u 始终来自集合 U，v 始终来自集合 V
        // e1.u 连 e2.v，e2.u 连 e1.v
        state.Edges.push_back({e1.u, e2.v});
        state.Edges.push_back({e2.u, e1.v});
        
        i += 2;
    }

    // 处理落单的边（如果有奇数条断裂边，最后一条原样接回）
    if (i < pool.size()) {
        state.Edges.push_back(pool[i]);
    }
}

}