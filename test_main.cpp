#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

// 引入核心引擎的头文件
#include "KicaDS.h"
#include "KicaEngine.h"
#include "RewireStrategies.h"
#include "KicaUtils.h"

#include "Analyzer.h"

using namespace ::KiCA;
using json = nlohmann::json; // 简化命名

int main(int argc, char *argv[])
{
    // 1. 从命令行获取配置文件路径
    std::string config_path = "config.json"; // 默认路径
    if (argc > 1)
        config_path = argv[1];

    // 2. 读取并解析 JSON 配置
    std::ifstream f(config_path);
    if (!f.is_open())
    {
        std::cerr << "Error: Cannot open config file: " << config_path << "\n";
        return 1;
    }
    json config = json::parse(f);

    // 3. 从 JSON 中提取参数
    int num_v = config["system"]["num_v"];
    int delta = config["system"]["delta"];
    int num_steps = config["system"]["num_steps"];

    int param1 = config["initial_state"]["param1"];
    double param2 = config["initial_state"]["param2"];
    int seed = config["initial_state"]["seed"];

    json j_strategy = config["rewire_strategy"];
    std::string strategy_type = j_strategy["type"];

    KiCA::PhiDistribution dist = config["initial_state"]["distribution"];
    KiCA::EdgeMode mode = config["initial_state"]["edge_mode"];

    // 4. 初始化系统状态
    std::cout << "Initializing system with num_v=" << num_v << ", delta=" << delta << "...\n";
    KicaState state = KiCA::KicaUtils::generate_initial_state(
        num_v, delta, dist, mode, param1, param2, seed);

    KicaTimer timer(num_v + delta, num_v);

    // 5. 实例化重组策略和引擎
    std::shared_ptr<IRewireStrategy> strategy;
    if (strategy_type == "Random")
    {
        strategy = std::make_shared<RandomRewireStrategy>();
    }
    else if (strategy_type == "Ordered")
    {
        auto rewire_mode = j_strategy["mode"];
        auto rewire_metric = j_strategy["metric"];

        strategy = std::make_shared<OrderedRewireStrategy>(rewire_mode, rewire_metric);
    }
    else
    {
        std::cerr << "Unknown strategy type!\n";
        return 1;
    }

    KicaEngine engine(num_v + delta, num_v, strategy);
    KiCA::GraphUtils::Analyzer cur_analyzer(state);

    // 6. 运行引擎演化
    for (int step = 1; step <= num_steps; ++step)
    {
        engine.step(state, &timer);
        cur_analyzer.rebuild_dsu();
        auto lcc = cur_analyzer.analyze_lcc();
        std::cout << "Step " << step << " - lcc size:" << lcc.size
                  << ", lcc avg_degree: " << lcc.avg_degree << "\n";
    }

    return 0;
}