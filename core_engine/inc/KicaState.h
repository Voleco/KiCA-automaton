#pragma once

#include <vector>

// 定义一条羁绊边
struct Edge {
    int a;
    int b;
    
    bool operator==(const Edge& other) const {
        return a == other.a && b == other.b;
    }
};

// 系统的全局物理状态容器 (纯数据)
struct KicaState {
    int num_nodes;
    int tau;                       // 全局节拍 0 或 1

    std::vector<int> Phi;          // 因果势快照
    std::vector<int> Delta;        // 变化量缓存
    std::vector<int> Phi_tilde;    // 中间态
    std::vector<Edge> Edges;       // 当前边集合

    // 构造函数：预分配内存
    explicit KicaState(int n = 0) : num_nodes(n), tau(0) {
        Phi.resize(n, 0);
        Delta.resize(n, 0);
        Phi_tilde.resize(n, 0);
    }
};