#include <iostream>
#include <vector>
#include <memory>

// 引入核心引擎的头文件
#include "KicaDS.h"
#include "KicaEngine.h"
#include "RewireStrategies.h"
#include "KicaUtils.h"

using namespace ::KiCA;
// using KiCA::KicaState;
// using KiCA::KicaTimer;

// 辅助函数：打印当前网络状态
void print_state(int step, const KicaState &state, const KicaTimer &timer)
{
    std::cout << "=== Step " << step << " (tau = " << state.tau << ") ===\n";

    // // 打印节点状态 (Phi) 和 局部原时 (ProperTime)
    // std::cout << "Nodes (Phi | ProperTime): ";
    // for (int i = 0; i < state.num_nodes; ++i) {
    //     std::cout << "[" << i << "]: " << state.Phi[i] << "|" << timer.ProperTime[i] << "  ";
    // }
    // std::cout << "\n";

    // // 打印当前的边
    // std::cout << "Edges: ";
    // for (const auto& edge : state.Edges) {
    //     std::cout << "(" << edge.a << "-" << edge.b << ") ";
    // }
    // std::cout << "\n\n";
}

int main()
{
    // 1. 初始化系统参数

    KicaState state = KiCA::KicaUtils::generate_initial_state(
        10, 1, 11, PhiDistribution::Gamma, KiCA::EdgeMode::PerfectMatching, 10, 1, 43);
    const int NUM_NODES = 4;
    // KicaState state(NUM_NODES);
    // KicaTimer timer(NUM_NODES);

    // // 2. 设置初始状态 (Phi)
    // // 给节点赋予不同的初始值，以触发状态转移和潜在的拓扑断裂
    // state.Phi[0] = 10;
    // state.Phi[1] = 5;
    // state.Phi[2] = 10;
    // state.Phi[3] = 5;

    // // 3. 设置初始拓扑结构 (Edges)
    // // 构建一个简单的环: 0-1, 1-2, 2-3, 3-0
    // state.Edges = {
    //     {0, 1},
    //     {1, 2},
    //     {2, 3},
    //     {3, 0}
    // };

    // // 4. 实例化重组策略和引擎
    // auto random_strategy = std::make_shared<RandomRewireStrategy>();
    // KicaEngine engine(random_strategy);

    // std::cout << "--- Initial State ---\n";
    // print_state(0, state, timer);

    // // 5. 运行引擎演化 5 步
    // const int NUM_STEPS = 5;
    // for (int step = 1; step <= NUM_STEPS; ++step) {
    //     // 执行单步演化，传入 timer 以记录局部原时
    //     engine.step(state, &timer);

    //     // 打印演化后的状态
    //     print_state(step, state, timer);
    // }

    // std::cout << "Simulation finished successfully.\n";
    // return 0;
}