#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
#include <stack>
using namespace std;


// =============================================================================
// SpanningTreeKruskal =========================================================
// =============================================================================

CSpanningTree SpanningTreeKruskal(CGraph& graph)
{
	struct comparator {
		bool operator()(CEdge* pE1, CEdge* pE2) {
			return pE1->m_Length > pE2->m_Length;
		}
	};
	priority_queue<CEdge*, std::vector<CEdge*>, comparator> queue;

	CSpanningTree tree(&graph);

	for (CEdge &e : graph.m_Edges) queue.push(&e);
	
	int label = 1;
	for (CVertex& v : graph.m_Vertices) v.m_KruskalLabel = label++;

	while (!queue.empty()) {
		CEdge* pE = queue.top();
		if (pE->m_pOrigin->m_KruskalLabel != pE->m_pDestination->m_KruskalLabel) {
			tree.m_Edges.push_back(pE);
			int l2 = pE->m_pDestination->m_KruskalLabel;
			for (CVertex& v : graph.m_Vertices) {
				if (v.m_KruskalLabel == l2) v.m_KruskalLabel = pE->m_pOrigin->m_KruskalLabel;
			}
		}
		queue.pop();
	}
	return tree;
}

void Marcar(CGraph& g, CVertex* pVertex)
{
	stack<CVertex*> pila;
	for (CVertex& v : g.m_Vertices) v.m_Marca = false;
	pVertex->m_Marca = true;
	pila.push(pVertex);
	while (!pila.empty()) {
		CVertex* pV = pila.top();
		pila.pop();
		for (CEdge* pE : pV->m_Edges) {
			if (!pE->m_pDestination->m_Marca) {
				pE->m_pDestination->m_Marca = true;
				pila.push(pE->m_pDestination);
			}
		}
	}
}