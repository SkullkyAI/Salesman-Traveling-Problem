#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
using namespace std;

// =============================================================================
// SpanningTreePrim ============================================================
// =============================================================================


CSpanningTree SpanningTreePrim(CGraph & graph)
{
	// Check if the graph is empty
	CSpanningTree tree(&graph);
	if (graph.m_Edges.empty() || graph.m_Vertices.empty()) {
		return tree;
	}

	struct comparator {
		bool operator()(const CEdge* pE1, const CEdge* pE2) const
		{
			return pE1->m_Length > pE2->m_Length;
		}
	};	
	priority_queue<CEdge*, std::vector<CEdge*>, comparator> queue;

	// Add initial candidates to the queue
	CVertex& first_vertex = graph.m_Vertices.front();
	for(CEdge* edge : first_vertex.m_Edges) {
		queue.push(edge);
	}

	// Setting all nodes as outside the tree
	for (CVertex& vertex : graph.m_Vertices) {
		vertex.m_PrimInTree = false;
	}
	first_vertex.m_PrimInTree = true;

	// Loop till the tree is created or all nodes are visited
	while (!queue.empty() && tree.m_Edges.size() != graph.m_Vertices.size() - 1) {

		// Add best candidate
		auto minimum_cost_edge = queue.top();
		minimum_cost_edge->m_pDestination->m_PrimInTree = true;
		tree.m_Edges.push_back(minimum_cost_edge);

		// Remove used candidate
		queue.pop();

		// Add new candidates
		for (const auto edge : minimum_cost_edge->m_pDestination->m_Edges) {
			// Check if the candidate is valid
			if (edge->m_pDestination->m_PrimInTree == true)
			{
				continue;
			}

			queue.push(edge);
		}

		// Clean the queue
		for (int i = 0; i < queue.size(); i++)
		{
			if (queue.top()->m_pDestination->m_PrimInTree == true) {
				queue.pop();
			}
			else
			{
				break;
			}
		}
	}
	return tree;
}

