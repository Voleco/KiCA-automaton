
#include "KicaUtils.h"

namespace KiCA
{

    KicaState KicaUtils::generate_initial_state(
        int n, int epsilon,
        PhiDistribution dist_type,
        EdgeMode edge_mode,
        double param1, double param2,
        int seed)
    {
        if (n <= 0 || n + epsilon <= 0)
        {
            throw std::invalid_argument("Node counts must be positive.");
        }

        int num_u = n + epsilon;
        int num_v = n;

        int num_edges = num_u;

        KicaState state(num_u, num_v);
        std::mt19937 rng(seed);

        // ---------------------------------------------------------
        // 1. 分配初始因果势 Phi
        // ---------------------------------------------------------
        auto assign_phi = [&](std::vector<int> &phi_vec)
        {
            switch (dist_type)
            {
            case PhiDistribution::Normal:
            {
                std::normal_distribution<double> dist(param1, param2);
                for (int &val : phi_vec)
                    val = static_cast<int>(std::round(dist(rng)));
                break;
            }
            case PhiDistribution::Gamma:
            {
                std::gamma_distribution<double> dist(param1, param2);
                for (int &val : phi_vec)
                    val = static_cast<int>(std::round(dist(rng)));
                break;
            }
            case PhiDistribution::MaxwellBoltzmann:
            {
                std::chi_squared_distribution<double> dist(3.0);
                double a = param1;
                for (int &val : phi_vec)
                {
                    val = static_cast<int>(std::round(a * std::sqrt(dist(rng))));
                }
                break;
            }
            }
        };

        assign_phi(state.Phi_U);
        assign_phi(state.Phi_V);

        // ---------------------------------------------------------
        // 2. 拓扑连接 (Edge Generation)
        // ---------------------------------------------------------
        std::unordered_set<Edge, EdgeHash> edge_set;

        if (edge_mode == EdgeMode::PerfectMatching)
        {
            // 提取索引并按照 Phi 的大小降序排序
            std::vector<int> idx_u(num_u);
            std::vector<int> idx_v(num_v);
            std::iota(idx_u.begin(), idx_u.end(), 0);
            std::iota(idx_v.begin(), idx_v.end(), 0);

            std::sort(idx_u.begin(), idx_u.end(), [&](int a, int b)
                      { return state.Phi_U[a] > state.Phi_U[b]; });
            std::sort(idx_v.begin(), idx_v.end(), [&](int a, int b)
                      { return state.Phi_V[a] > state.Phi_V[b]; });

            // 进行 1 对 1 的完美匹配 (直到较小的集合被耗尽)
            int match_count = std::min(num_u, num_v);
            for (int i = 0; i < match_count; ++i)
            {
                edge_set.insert({idx_u[i], idx_v[i]});
            }
        }

        // 填充剩余的边 (无论是 Random 模式，还是 PerfectMatching 剩下的边)
        std::uniform_int_distribution<int> dist_u(0, num_u - 1);
        std::uniform_int_distribution<int> dist_v(0, num_v - 1);

        while (edge_set.size() < static_cast<size_t>(num_edges))
        {
            Edge new_edge{dist_u(rng), dist_v(rng)};
            edge_set.insert(new_edge);
        }

        // 将生成的边写入状态
        state.Edges.assign(edge_set.begin(), edge_set.end());
        state.tau = 0;

        return state;
    }

}