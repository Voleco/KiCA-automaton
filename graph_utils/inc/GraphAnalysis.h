#pragma once

#include <vector>

#include "KicaDS.h"

namespace KiCA::GraphUtils
{
    int get_largest_component_size(const KicaState &state);

    std::vector<int> get_degree_distribution(const KicaState &state);
}