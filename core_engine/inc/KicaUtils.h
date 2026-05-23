
#pragma once

#include "KicaEngine.h"
#include <vector>
#include <random>
#include <cmath>
#include <stdexcept>
#include <unordered_set>
#include <numeric>
#include <algorithm>

#include <nlohmann/json.hpp>

namespace KiCA
{

    // 定义支持的初始势分布类型
    enum class PhiDistribution
    {
        Normal,
        Gamma,
        MaxwellBoltzmann,
        Exponential
    };

    // 定义初始边的连接模式
    enum class EdgeMode
    {
        Random,         // 完全随机连接
        PerfectMatching // 按照 Phi 排序后优先进行 1对1 完美匹配，剩余边随机
    };

    class KicaUtils
    {
    public:
        static KicaState generate_initial_state(
            int n, int epsilon,
            PhiDistribution dist_type,
            EdgeMode edge_mode,
            double param1, double param2 = 1.0,
            int seed = 42);
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(KiCA::PhiDistribution, {{KiCA::PhiDistribution::Normal, "Normal"},
                                                         {KiCA::PhiDistribution::Gamma, "Gamma"},
                                                         {KiCA::PhiDistribution::MaxwellBoltzmann, "MaxwellBoltzmann"},
                                                         {KiCA::PhiDistribution::Exponential, "Exponential"}})

    NLOHMANN_JSON_SERIALIZE_ENUM(KiCA::EdgeMode, {{KiCA::EdgeMode::Random, "Random"},
                                                  {KiCA::EdgeMode::PerfectMatching, "PerfectMatching"}})
}