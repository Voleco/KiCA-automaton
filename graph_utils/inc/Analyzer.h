#pragma once

#include <vector>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <assert.h>
#include <queue>
#include "KicaDS.h"
#include "GraphTools.h"

namespace KiCA::GraphUtils
{

    class Analyzer
    {
    public:
        // 构造函数：接收状态，并一次性构建好 DSU
        explicit Analyzer(const KicaState &state)
            : m_state(state),
              m_total_nodes(state.num_u + state.num_v),
              m_dsu(m_total_nodes)
        {
            rebuild_dsu(); // 构造时调用
        }

        // 当外部的 m_state 发生演化后，调用此函数同步 DSU 状态
        void rebuild_dsu()
        {
            m_total_nodes = m_state.num_u + m_state.num_v;
            m_dsu = DSU(m_total_nodes); // 重新初始化 DSU
            for (const auto &edge : m_state.Edges)
            {
                m_dsu.unite(edge.u, edge.v + m_state.num_u);
            }
        }

        /**
         * @brief 提取最大连通分量(LCC)的综合信息
         */
        LCCInfo analyze_lcc() const;

        /**
         * @brief 计算渗流磁化率 (Susceptibility) - 寻找相变点的关键指标
         */
        double get_susceptibility() const;

        /**
         * @brief 在图上随机取点，计算 k 步 BFS 展开的节点数量 (体积)
         * @param start_node 起点 (0 到 num_u-1 为 U, num_u 到 num_u+num_v-1 为 V)
         * @param k 展开步数
         */
        int get_k_step_volume(int start_node, int k);

    private:
        const KicaState &m_state; // 保持对原状态的常量引用，避免拷贝
        int m_total_nodes;
        DSU m_dsu; // 分析器内部持有的 DSU 实例
    };
}