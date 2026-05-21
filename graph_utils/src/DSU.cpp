#include "GraphTools.h"

namespace KiCA::GraphUtils
{

    int DSU::find(int i) const
    {
        // 边界检查
        assert(i >= 0 && i < parent.size());

        if (parent[i] == i)
            return i;

        // 路径压缩：将当前节点直接连接到根节点，加速后续查找
        return parent[i] = find(parent[i]);
    }

    bool DSU::unite(int i, int j)
    {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j)
        {
            // 按秩合并 (按大小合并)：将小树的根挂到大树的根上
            if (component_size[root_i] < component_size[root_j])
            {
                std::swap(root_i, root_j);
            }
            parent[root_j] = root_i;
            component_size[root_i] += component_size[root_j];

            // 每次成功合并两个不同的集合，总连通分量数量减 1
            component_count--;
            return true;
        }
        return false;
    }

}