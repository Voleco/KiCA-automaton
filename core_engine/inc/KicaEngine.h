#pragma once

#include "KicaDS.h"
#include "RewireStrategies.h"
#include <vector>
#include <memory>

namespace KiCA
{

    class KicaEngine
    {
    public:
        // 构造函数：必须注入一个重组策略
        explicit KicaEngine(int num_u, int num_v, std::shared_ptr<IRewireStrategy> strategy);

        // 允许在运行时动态更换策略
        void setRewireStrategy(std::shared_ptr<IRewireStrategy> strategy);

        // 核心：执行单步演化。
        // timer 为可选参数，传入 nullptr 则不进行时间统计。
        void step(KicaState &state, KicaTimer *timer = nullptr);

    private:
        // 引擎持有的重组策略
        std::shared_ptr<IRewireStrategy> rewire_strategy;

        // 工作内存 (Scratchpad Memory) - 属于 Engine，不属于 State
        std::vector<int> Delta_U;
        std::vector<int> Delta_V;
        std::vector<int> Phi_tilde_U;
        std::vector<int> Phi_tilde_V;

        std::vector<int> Degree_Snapshot_U;
        std::vector<int> Degree_Snapshot_V;
    };

}