#pragma once

#include "KicaDS.h"
#include <vector>

#include <random>

namespace KiCA
{

    // 拓扑重组策略的纯虚基类 (Interface)
    class IRewireStrategy
    {
    public:
        virtual ~IRewireStrategy() = default;

        // 核心接口：根据断裂的边，修改系统状态（通常是向 state.Edges 追加新边）
        virtual void rewire(KicaState &state, const std::vector<Edge> &broken_edges) = 0;
    };

    // 随机配对重组
    class RandomRewireStrategy : public IRewireStrategy
    {
    public:
        RandomRewireStrategy();

        void rewire(KicaState &state, const std::vector<Edge> &broken_edges) override;

    private:
        std::mt19937 rng; // 策略内部维护自己的随机数状态
    };

    // 排序重连策略 (支持顺序与逆序)
    class OrderedRewireStrategy : public IRewireStrategy
    {
    public:
        // 定义匹配模式
        enum class Mode
        {
            Ordered, // 顺序：大配大，小配小 (倾向于最小作用量/稳定)
            Reversed // 逆序：大配小，小配大 (倾向于最大势差/剧烈交换)
        };

        // 构造函数，默认使用顺序匹配
        explicit OrderedRewireStrategy(Mode mode = Mode::Ordered) : current_mode(mode) {};

        // 允许在运行时动态切换模式，方便实验探索
        void setMode(Mode mode) { current_mode = mode; };
        Mode getMode() const { return current_mode; };

        void rewire(KicaState &state, const std::vector<Edge> &broken_edges) override;

    private:
        Mode current_mode; // 当前的匹配模式
    };

}