#include "pch.h"

#include <queue>

#include "Graph.h"

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================
void Dijkstra(CGraph& graph, CVertex* pStart) {
	DijkstraQueue(graph, pStart);
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================
void DijkstraQueue(CGraph& graph, CVertex *pStart) {
	// Assign infinite distance to all vertices, except the start vertex
	for (auto& vertex : graph.m_Vertices) {
		vertex.m_DijkstraDistance = std::numeric_limits<double>::max();
		vertex.m_DijkstraVisit = false;
	}
	pStart->m_DijkstraDistance = 0.0;

	if (graph.m_Edges.empty()) {
		return;
	}

	// Initialize a priority queue
	struct comparator {
		bool operator()(pair<CVertex*, double> pairLeft, pair<CVertex*, double> pairRight) const {
			return pairLeft.second > pairRight.second;
		}
	};
	priority_queue<pair<CVertex*, double>, std::vector<pair<CVertex*, double>>, comparator> queue;
	queue.emplace(pStart, 0.0);


	while(!queue.empty()) {
		const auto currentVertex = queue.top().first;
		queue.pop();

		if (!currentVertex->m_DijkstraVisit) {
			for (auto const edge : currentVertex->m_Edges) {
				auto const adjacentVertex = edge->m_pDestination;
				// Calculate the dijkstra distance of the adjacent vertices
				const auto tentativeDistance = currentVertex->m_DijkstraDistance + edge->m_Length;
				if (tentativeDistance < adjacentVertex->m_DijkstraDistance) {
					adjacentVertex->m_DijkstraDistance = tentativeDistance;
					adjacentVertex->m_pDijkstraPrevious = edge;
					queue.emplace(adjacentVertex, tentativeDistance);
				}
			}
			currentVertex->m_DijkstraVisit = true;
		}
	}
}