#include "pch.h"
#include "Graph.h"
// SalesmanTrackGreedy =========================================================

CTrack FindShortestTrack(CGraph& graph, const CVertex* startVertex, const CVertex* endVertex)
{
	CTrack result(&graph);

	auto currentVertex = endVertex;
	while (true) {
		result.AddFirst(currentVertex->m_pDijkstraPrevious);

		if (currentVertex->m_pDijkstraPrevious->m_pOrigin == startVertex) {
			break;
		}

		currentVertex = currentVertex->m_pDijkstraPrevious->m_pOrigin;
	}

	return result;
}

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits &visits)
{
	CTrack result(&graph);

	// First element
	auto currentVertex = visits.m_Vertices.front();

	// Create candidates
	std::list<CVertex*> candidates(visits.m_Vertices);
	candidates.pop_back();
	candidates.pop_front();

	while(!candidates.empty()) {
		// Calculate the minimum cost tracks from a start point
		DijkstraQueue(graph, currentVertex);

		// Get min cost candidate
		CVertex* minCostVertex = candidates.front();
		for (const auto candidate : candidates) {
			if (minCostVertex->m_DijkstraDistance > candidate->m_DijkstraDistance) {
				minCostVertex = candidate;
			}
		}

		// Find shortest path from currentVertex to minCostVertex
		CTrack track = FindShortestTrack(graph, currentVertex, minCostVertex);
		result.Append(track);

		// Remove it as possible candidate
		candidates.remove(minCostVertex);

		// Update current vertex
		currentVertex = minCostVertex;
	}

	// Calculate the minimum cost tracks from a 
	DijkstraQueue(graph, currentVertex);

	// Find shortest track to the last vertex to visit
	CTrack finalTrack = FindShortestTrack(graph, currentVertex, visits.m_Vertices.back());
	result.Append(finalTrack);
	

	return result;
}
