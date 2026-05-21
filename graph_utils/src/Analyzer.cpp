#include "Analyzer.h"

namespace KiCA::GraphUtils
{

    LCCInfo Analyzer::analyze_lcc() const
    {
        int lcc_root = 0;
        int max_size = 0;
        for (int i = 0; i < m_total_nodes; ++i)
        {
            if (m_dsu.find(i) == i && m_dsu.get_component_size(i) > max_size)
            {
                max_size = m_dsu.get_component_size(i);
                lcc_root = i;
            }
        }

        LCCInfo info = {lcc_root, max_size, 0, 0.0, 0, 0};

        for (int i = 0; i < m_state.num_u; ++i)
        {
            if (m_dsu.find(i) == lcc_root)
                info.u_count++;
        }
        for (int i = 0; i < m_state.num_v; ++i)
        {
            if (m_dsu.find(i + m_state.num_u) == lcc_root)
                info.v_count++;
        }

        for (const auto &edge : m_state.Edges)
        {
            if (m_dsu.find(edge.u) == lcc_root)
                info.edge_count++;
        }

        if (info.size > 0)
        {
            info.avg_degree = 2.0 * info.edge_count / info.size;
        }

        return info;
    }

    double Analyzer::get_susceptibility() const
    {
        int max_size = 0;
        int lcc_root = -1;

        for (int i = 0; i < m_total_nodes; ++i)
        {
            if (m_dsu.find(i) == i && m_dsu.get_component_size(i) > max_size)
            {
                max_size = m_dsu.get_component_size(i);
                lcc_root = i;
            }
        }

        double sum_s2 = 0.0;
        double sum_s = 0.0;

        for (int i = 0; i < m_total_nodes; ++i)
        {
            if (m_dsu.find(i) == i && i != lcc_root)
            {
                double s = m_dsu.get_component_size(i);
                sum_s2 += s * s;
                sum_s += s;
            }
        }

        return sum_s > 0 ? (sum_s2 / sum_s) : 0.0;
    }

    int Analyzer::get_k_step_volume(int start_node, int k)
    {
        int total_nodes = m_state.num_u + m_state.num_v;

        // 构建局部/全局的轻量级邻接表 O(N)
        std::vector<std::vector<int>> adj(total_nodes);
        for (const auto &edge : m_state.Edges)
        {
            int u = edge.u;
            int v = edge.v + m_state.num_u; // V 节点偏移
            adj[u].push_back(v);
            adj[v].push_back(u);
        }

        // BFS 初始化
        std::vector<bool> visited(total_nodes, false);
        std::queue<std::pair<int, int>> q; // {node, current_depth}

        q.push({start_node, 0});
        visited[start_node] = true;
        int volume = 0;

        // 执行 BFS
        while (!q.empty())
        {
            auto [curr, depth] = q.front();
            q.pop();
            volume++; // 记录访问到的节点数

            if (depth < k)
            {
                for (int neighbor : adj[curr])
                {
                    if (!visited[neighbor])
                    {
                        visited[neighbor] = true;
                        q.push({neighbor, depth + 1});
                    }
                }
            }
        }

        return volume;
    }
}