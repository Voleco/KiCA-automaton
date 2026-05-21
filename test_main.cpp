#include <iostream>
#include <vector>
#include <memory>

// 引入核心引擎的头文件
#include "KicaDS.h"
#include "KicaEngine.h"
#include "RewireStrategies.h"
#include "KicaUtils.h"

#include "Analyzer.h"

using namespace ::KiCA;

int main()
{
    // 1. 初始化系统参数

    int num_v = 1000000;
    int delta = 100000;
    KicaState state = KiCA::KicaUtils::generate_initial_state(
        num_v, delta, PhiDistribution::Gamma, KiCA::EdgeMode::Random, 1000, 1, 43);

    KicaTimer timer(num_v + delta, num_v);
    // // 4. 实例化重组策略和引擎
    auto random_strategy = std::make_shared<RandomRewireStrategy>();
    KicaEngine engine(random_strategy);

    // std::cout << "--- Initial State ---\n";
    // print_state(0, state, timer);

    KiCA::GraphUtils::Analyzer cur_analyzer(state);
    // // 5. 运行引擎演化 5 步
    const int NUM_STEPS = 50;
    for (int step = 1; step <= NUM_STEPS; ++step)
    {
        // 执行单步演化，传入 timer 以记录局部原时
        engine.step(state, &timer);

        cur_analyzer.rebuild_dsu();
        auto lcc = cur_analyzer.analyze_lcc();
        std::cout << "lcc size:" << lcc.size << ", lcc avg_degree: " << lcc.avg_degree << "\n";
    }

    // std::cout << "Simulation finished successfully.\n";
    return 0;
}