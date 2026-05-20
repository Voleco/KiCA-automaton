#pragma once

#include <vector>

namespace KiCA
{
    
// 定义一条二分图的羁绊边
struct Edge {
    int u; // 集合 U 中的顶点索引 (范围: 0 到 num_u - 1)
    int v; // 集合 V 中的顶点索引 (范围: 0 到 num_v - 1)
    
    bool operator==(const Edge& other) const {
        return u == other.u && v == other.v;
    }
};

// 系统的全局物理状态容器 (纯数据 - 严格二分图架构)
struct KicaState {
    int num_u;                     // 集合 U 的节点数量
    int num_v;                     // 集合 V 的节点数量
    int tau;                       // 全局节拍 0 或 1

    // ================= 集合 U 的状态 =================
    std::vector<int> Phi_U;        // 集合 U 的因果势快照
    std::vector<int> Delta_U;      // 集合 U 的变化量缓存
    std::vector<int> Phi_tilde_U;  // 集合 U 的中间态

    // ================= 集合 V 的状态 =================
    std::vector<int> Phi_V;        // 集合 V 的因果势快照
    std::vector<int> Delta_V;      // 集合 V 的变化量缓存
    std::vector<int> Phi_tilde_V;  // 集合 V 的中间态

    // ================= 拓扑结构 =================
    std::vector<Edge> Edges;       // 当前边集合

    // 构造函数：分别为两个集合预分配内存
    explicit KicaState(int n_u = 0, int n_v = 0) : num_u(n_u), num_v(n_v), tau(0) {
        // 初始化集合 U
        Phi_U.resize(num_u, 0);
        Delta_U.resize(num_u, 0);
        Phi_tilde_U.resize(num_u, 0);

        // 初始化集合 V
        Phi_V.resize(num_v, 0);
        Delta_V.resize(num_v, 0);
        Phi_tilde_V.resize(num_v, 0);

        Edges.resize(std::max(num_u,num_v));
    }
};



// 系统的局部原时记录器 
struct KicaTimer {
    int num_u;                     // 集合 U 的节点数量
    int num_v;                     // 集合 V 的节点数量

    std::vector<int> ProperTime_U; // 集合 U 的局部原时
    std::vector<int> ProperTime_V; // 集合 V 的局部原时

    // 构造函数：分别为两个集合预分配内存并初始化为 0
    explicit KicaTimer(int n_u = 0, int n_v = 0) : num_u(n_u), num_v(n_v) {
        ProperTime_U.resize(num_u, 0);
        ProperTime_V.resize(num_v, 0);
    }
};


}