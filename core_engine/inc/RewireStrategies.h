#pragma once

#include "KicaState.h"
#include <vector>

#include <random>

// 拓扑重组策略的纯虚基类 (Interface)
class IRewireStrategy {
public:
    virtual ~IRewireStrategy() = default;

    // 核心接口：根据断裂的边，修改系统状态（通常是向 state.Edges 追加新边）
    virtual void rewire(KicaState& state, const std::vector<Edge>& broken_edges) = 0;
};


// 随机配对重组
class RandomRewireStrategy : public IRewireStrategy {
public:
    RandomRewireStrategy();

    void rewire(KicaState& state, const std::vector<Edge>& broken_edges) override;

private:
    std::mt19937 rng; // 策略内部维护自己的随机数状态
};