#include <cstring>
#include <iostream>
#include <set>
#include <stack>
#include <vector>
using std::cin, std::cout, std::endl, std::vector, std::stack, std::set;
const int N = 1e6 + 1;
bool discovered[N];

void
DFS1(int v, const vector<vector<int>>& E, stack<int>& order)
{
  discovered[v] = true;
  for (int i = 0; i != E[v].size(); i++) {
    if (!discovered[E[v][i]])
      DFS1(E[v][i], E, order);
  }
  order.push(v);
}

set<int>
DFS2(int v, const vector<vector<int>>& E, set<int>& EqNodes)
{
  discovered[v] = true;
  EqNodes.insert(v);
  for (int i = 0; i != E[v].size(); i++)
    if (!discovered[E[v][i]])
      DFS2(E[v][i], E, EqNodes);
  return EqNodes;
}

void
printDiGraph(const vector<vector<int>>& E, stack<int>& order);
void
printEqClass(const set<int>& EqNodes, int EqClassID);

int n, m; // n nodes m edges
int
main()
{
  cin >> n >> m; // node range from [0, n)
  vector<vector<int>> E1;
  vector<vector<int>> E2;

  E1.resize(n);
  E2.resize(n);

  int u, v;
  for (int i = 0; i < m; i++) {
    cin >> u >> v;
    E1[u].push_back(v);
    E2[v].push_back(u);
  }

  stack<int> order;
  for (int i = 0; i < n; i++)
    if (!discovered[i])
      DFS1(i, E2, order);

  printDiGraph(E1, order);
  return 0;
}

void
printDiGraph(const vector<vector<int>>& E, stack<int>& order)
{
  cout << "digraph {\n    rankdir = LR;\n";
  memset(discovered, 0, sizeof(discovered));

  set<int> sinkClassNodes;
  int NumEqClass = 0;
  for (int i = n - 1; i >= 0; i--) {
    int sink = order.top();
    order.pop();
    if (discovered[sink])
      continue;
    sinkClassNodes.clear();
    DFS2(sink, E, sinkClassNodes);
    printEqClass(sinkClassNodes, NumEqClass);
    NumEqClass++;
  }

  for (int i = 0; i < n; i++)
    for (auto node_out = E[i].begin(); node_out != E[i].end(); node_out++)
      cout << "        " << i << " -> " << *node_out << ";\n";

  cout << "}" << endl;
}

void
printEqClass(const set<int>& EqNodes, int EqClassID)
{
  cout << "    subgraph cluster_" << EqClassID << " {\n";
  cout << "        label = \"EqClass_" << EqClassID << "\";\n";
  cout << "        ";
  for (auto node_it = EqNodes.begin(); node_it != EqNodes.end(); node_it++)
    cout << *node_it << "; ";
  cout << "\n";
  cout << "    }\n";
}