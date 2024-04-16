#include <cstring> // for memset()
#include <fstream>
#include <iostream>
#include <set>
#include <stack>
#include <vector>
using std::cin, std::cout, std::cerr, std::ifstream, std::ofstream, std::endl,
  std::vector, std::stack, std::set;
const int N = 1e6 + 1; // maximum of n

int discovered[N]; // -1 means not discovered, otherwise EqClassID

void
DFS1(int v, const vector<vector<int>>& E, stack<int>& order)
{
  discovered[v] = -2; // mark as discovered
  for (auto neighbor = E[v].begin(); neighbor != E[v].end(); neighbor++)
    if (discovered[*neighbor] == -1)
      DFS1(*neighbor, E, order);

  order.push(v);
}

void
DFS2(int v,
     const vector<vector<int>>& E,
     set<int>& EqNodes,
     set<int>& EqOuts,
     int EqClassID)
{
  discovered[v] = EqClassID;
  EqNodes.insert(v);
  for (auto neighbor = E[v].begin(); neighbor != E[v].end(); neighbor++)
    if (discovered[*neighbor] == -1)
      DFS2(*neighbor, E, EqNodes, EqOuts, EqClassID);
    else if (discovered[*neighbor] != EqClassID)
      EqOuts.insert(discovered[*neighbor]);
}

void
printEqClass(const set<int>& EqNodes, int EqClassID, ofstream& fout1)
{
  fout1 << "    subgraph cluster_" << EqClassID << " {\n";
  fout1 << "        label = \"EqClass_" << EqClassID << "\";\n";
  fout1 << "        ";
  for (auto node_it = EqNodes.begin(); node_it != EqNodes.end(); node_it++)
    fout1 << *node_it << "; ";
  fout1 << "\n";
  fout1 << "    }\n";
}

void
printEqClassDAG(const set<int>& EqOuts, int EqClassID, ofstream& fout2)
{
  fout2 << "    Eq_" << EqClassID << ";\n";
  for (auto out_it = EqOuts.begin(); out_it != EqOuts.end(); out_it++)
    fout2 << "Eq_" << EqClassID << " -> Eq_" << *out_it << ";\n";
}

void
printDiGraph(const vector<vector<int>>& E,
             stack<int>& order,
             ofstream& fout1,
             ofstream& fout2)
{
  fout1 << "digraph {\n    rankdir = LR;\n";
  fout2 << "digraph {\n    rankdir = LR;\n";

  memset(discovered, -1, sizeof(discovered));

  set<int> sinkClassNodes;
  set<int> EqOuts;
  int NumEqClass = 0;
  while (!order.empty()) {
    int sink = order.top();
    order.pop();
    if (discovered[sink] != -1)
      continue;
    sinkClassNodes.clear();
    EqOuts.clear();

    DFS2(sink, E, sinkClassNodes, EqOuts, NumEqClass);

    printEqClass(sinkClassNodes, NumEqClass, fout1);
    printEqClassDAG(EqOuts, NumEqClass, fout2);
    NumEqClass++;
  }

  for (int i = 0; i != E.size(); i++)
    for (auto node_out = E[i].begin(); node_out != E[i].end(); node_out++)
      fout1 << "        " << i << " -> " << *node_out << ";\n";

  fout1 << "}" << endl;
  fout2 << "}" << endl;
}

void
SCC(ifstream& fin, ofstream& fout1, ofstream& fout2)
{
  int n, m;      // n nodes, m edges
  fin >> n >> m; // node range: [0, n)
  vector<vector<int>> E1;
  vector<vector<int>> E2;

  E1.resize(n);
  E2.resize(n);

  int u, v;
  for (int i = 0; i < m; i++) {
    fin >> u >> v;
    E1[u].push_back(v);
    E2[v].push_back(u);
  }
  memset(discovered, -1, sizeof(discovered));

  stack<int> order;
  for (int i = 0; i < n; i++)
    if (discovered[i] == -1)
      DFS1(i, E2, order);

  printDiGraph(E1, order, fout1, fout2);
}

int
main(int argc, char* argv[])
{
  if (argc != 4) {
    cerr << "Usage: " << argv[0]
         << " <input_filename> <output1_filename> <output2_filename>" << endl;
    cerr << "Please provide exactly 3 arguments." << endl;
    return 1;
  }
  // Open input file
  ifstream input_file(argv[1]);
  if (!input_file) {
    cerr << "Error: Unable to open input file: " << argv[1] << endl;
    return 1;
  }

  // Open output files
  ofstream output_file1(argv[2]);
  ofstream output_file2(argv[3]);
  if (!output_file1 || !output_file2) {
    cerr << "Error: Unable to open output files." << endl;
    return 1;
  }

  SCC(input_file, output_file1, output_file2);

  // Close all files
  input_file.close();
  output_file1.close();
  output_file2.close();
  return 0;
}
