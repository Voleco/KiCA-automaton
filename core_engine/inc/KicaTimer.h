#pragma once

#include <vector>

// 系统的局部时间记录器 (纯数据)
struct KicaTimer {
    int num_nodes;
    std::vector<int> ProperTime;   // 局部原时

    explicit KicaTimer(int n = 0) : num_nodes(n) {
        ProperTime.resize(n, 0);
    }
};