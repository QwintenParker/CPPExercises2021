#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <set>


int debugPoint(int line) {
    if (line < 0)
        return 0;

    // You can put breakpoint at the following line to catch any rassert failure:
    return line;
}

#define rassert(condition, message) if (!(condition)) { std::stringstream ss; (ss << "Assertion \"" << message << "\" failed at line " << debugPoint(__LINE__) << "!"); throw std::runtime_error(ss.str()); }


struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w)
    {}
};

void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.

    int nvertices, medges;
    std::cin >> nvertices;
    std::cin >> medges;

    std::vector<std::vector<Edge>> edges_by_vertex(nvertices);
    for (int i = 0; i < medges; ++i) {
        int ai, bi;
        int w;
        std::cin >> ai >> bi >> w;
        rassert(ai >= 1 && ai <= nvertices, 23472894792020);
        rassert(bi >= 1 && bi <= nvertices, 23472894792021);

        ai -= 1;
        bi -= 1;
        rassert(ai >= 0 && ai < nvertices, 3472897424024);
        rassert(bi >= 0 && bi < nvertices, 3472897424025);

        Edge edgeAB(ai, bi, w);
        edges_by_vertex[ai].emplace_back(ai, bi, w);

        edges_by_vertex[bi].emplace_back(bi, ai, w); // а тут - обратное ребро, можно конструировать объект прямо в той же строчке где он и потребовался
    }

    const int start = 0;
    const int finish = nvertices - 1;

    const int INF = std::numeric_limits<int>::max();

    std::vector<int> distances(nvertices, INF);
    // TODO ...
    std::vector<int> previous(nvertices, -1);
    std::set<std::pair<int, int>> k;
    distances[start] = 0;
    previous[start] = start;
    k.emplace(0, start);

    while (!k.empty()) {
        auto g = *k.begin();
        k.erase(*k.begin());
        if (distances[g.second] != g.first)
            continue;
        for (auto n : edges_by_vertex[g.second]) {
            if (distances[n.v] > g.first + n.w) {
                distances[n.v] = g.first + n.w;
                k.emplace(distances[n.v], n.v);
                previous[n.v] = g.second;
            }
        }
    }

    if (distances[finish] != INF) {
        std::vector<int> path;
        for (int i = finish; previous[i] != i; i = previous[i]) {
            path.emplace_back(i + 1);
        }
        path.emplace_back(start + 1);
        for (auto j = path.rbegin(); j != path.rend(); j++) {
            std::cout << *j << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << -1 << std::endl;
    }
}

int main() {
    std::cin.tie();
    std::ios_base::sync_with_stdio(false);
    try {
        run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
    std::cout.flush();
}
