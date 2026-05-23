#pragma once

#include <vector>
#include <random>

#include <nlohmann/json.hpp>

#include "KicaDS.h"
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
        // 排序指标：绝对势能 或 势能密度
        enum class Metric
        {
            AbsolutePhi, // 按 Phi 排序
            PhiPerDegree // 按 Phi / (k + 1) 排序
        };

        // 构造函数，默认使用顺序匹配
        explicit OrderedRewireStrategy(Mode mode = Mode::Ordered) : current_mode(mode) {};

        explicit OrderedRewireStrategy(Mode mode = Mode::Ordered, Metric metric = Metric::AbsolutePhi)
            : current_mode(mode), current_metric(metric) {}

        // 允许在运行时动态切换模式，方便实验探索
        void setMode(Mode mode) { current_mode = mode; };
        void setMetric(Metric metric) { current_metric = metric; };
        Mode getMode() const { return current_mode; };
        Metric getMetric() const { return current_metric; };

        void rewire(KicaState &state, const std::vector<Edge> &broken_edges) override;

    private:
        Mode current_mode;
        Metric current_metric;
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(OrderedRewireStrategy::Mode, {{OrderedRewireStrategy::Mode::Ordered, "Ordered"},
                                                               {OrderedRewireStrategy::Mode::Reversed, "Reversed"}})

    NLOHMANN_JSON_SERIALIZE_ENUM(OrderedRewireStrategy::Metric, {{OrderedRewireStrategy::Metric::AbsolutePhi, "AbsolutePhi"},
                                                                 {OrderedRewireStrategy::Metric::PhiPerDegree, "PhiPerDegree"}})
}