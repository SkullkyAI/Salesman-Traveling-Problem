#include "pch.h"
#include "Graph.h"
#include <set>
#include <stack>


// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================

void ShortestPathBacktracking(CGraph& graph, const CVertex* beginning, CVertex* end, stack<CTrack>& solutions, double& bestTrackLength, CTrack& bestTrack)
{
	for (auto iEdge : beginning->m_Edges) 
	{
		if (iEdge->m_Used || iEdge->m_pReverseEdge->m_Used) 
		{
			continue;
		}

		CVertex* vertex = iEdge->m_pDestination;
		if (beginning == iEdge->m_pDestination)
		{
			vertex = iEdge->m_pOrigin;
		}

		if (vertex->m_JaHePassat)
		{
			continue;
		}

		bestTrack.AddLast(iEdge);
		iEdge->m_Used = true;

		if (bestTrack.Length() < bestTrackLength)
		{
			vertex->m_JaHePassat = true;
			if (vertex == end)
			{
				if (!solutions.empty())
				{
					solutions.pop();
				}
				solutions.push(bestTrack);
				bestTrackLength = bestTrack.Length();
			}
			else
			{
				ShortestPathBacktracking(graph, vertex, end, solutions, bestTrackLength, bestTrack);
			}
			vertex->m_JaHePassat = false;
		}

		iEdge->m_Used = false;
		bestTrack.Delete(iEdge);
	}
}

void BacktrackingTool(CGraph& graph, CVisits& visits, stack<CTrack>& solution, const CVertex* currentVertex, CTrack& path, double& bestTrackLength, int count)
{
	for (auto iVertex : visits.m_Vertices) 
	{
		if (iVertex->m_VertexToVisit || !iVertex->m_Marca) 
		{
			continue;
		}

		if (iVertex != visits.m_Vertices.back() || count == visits.GetNVertices() - 1)
		{
			stack<CTrack> solutions;
			double maxLength = numeric_limits<double>::max();
			CTrack bestTrack = CTrack(&graph);

			ShortestPathBacktracking(graph, currentVertex, iVertex, solutions, maxLength, bestTrack);

			path.Append(solutions.top());
			if (path.Length() < bestTrackLength) 
			{
				iVertex->m_VertexToVisit = true;
				count++;
				if (count == visits.GetNVertices()) 
				{
					if (!solution.empty()) 
					{
						solution.pop();
					}
					solution.push(path);
					bestTrackLength = path.Length();
				}
				else
				{
					BacktrackingTool(graph, visits, solution, iVertex, path, bestTrackLength, count);
				}
				count--;
				iVertex->m_VertexToVisit = false;
			}
			while (path.m_Edges.back() != solutions.top().m_Edges.front()) {
				path.m_Edges.pop_back();
			}
			path.m_Edges.pop_back();
		}
	}
}

CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
	for (CVertex& iVertex : graph.m_Vertices) 
	{
		iVertex.m_Marca = false;
		iVertex.m_VertexToVisit = false; 
		iVertex.m_JaHePassat = false; 
	}

	for (CVertex* iVertex : visits.m_Vertices) 
	{
		iVertex->m_Marca = true;
	}

	for (CEdge& edge : graph.m_Edges) 
	{
		edge.m_Used = false;
	}

	visits.m_Vertices.front()->m_VertexToVisit = true;
	double bestTrackLength = numeric_limits<double>::max();

	stack<CTrack> solution;
	CTrack track(&graph);
	track.Clear();
	BacktrackingTool(graph, visits, solution, visits.m_Vertices.front(), track,bestTrackLength, 1);

	return solution.top();
}

// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================

CTrack FindShortestTrack(CGraph& graph, const CVertex* startVertex, const CVertex* endVertex);

void BacktrackingGreedyTool(CGraph& graph, CVisits& visits, stack<CTrack>& solution, CVertex* actual, CTrack& path, int count, double& best_length) {
	for (CVertex* iVertex : visits.m_Vertices) 
	{
		if (!iVertex->m_VertexToVisit && iVertex->m_Marca) 
		{
			if (iVertex != visits.m_Vertices.back() || count == visits.GetNVertices() - 1)
			{
				CTrack shortestTrack = CTrack(&graph);
				DijkstraQueue(graph, actual);
				shortestTrack = FindShortestTrack(graph, actual, iVertex);

				path.Append(shortestTrack);
				if (path.Length() < best_length) 
				{
					iVertex->m_VertexToVisit = true;
					count++;
					if (count == visits.GetNVertices()) 
					{
						if (!solution.empty()) 
						{
							solution.pop();
						}
						solution.push(path);
						best_length = path.Length();
					}
					else 
					{
						BacktrackingGreedyTool(graph, visits, solution, iVertex, path, count, best_length);
					}
					count--;
					iVertex->m_VertexToVisit = false;
				}

				while (path.m_Edges.back() != shortestTrack.m_Edges.front()) 
				{
					path.m_Edges.pop_back();
				}
				path.m_Edges.pop_back();
			}
		}
	}
}

CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	for (CVertex& vertex1 : graph.m_Vertices) {
		vertex1.m_Marca = false;
		vertex1.m_VertexToVisit = false;
	}

	for (CVertex* vertex2 : visits.m_Vertices) {
		vertex2->m_Marca = true;
	}

	visits.m_Vertices.front()->m_VertexToVisit = true;
	double maxLength = numeric_limits<double>::max();

	stack<CTrack> solution;
	CTrack track(&graph);
	BacktrackingGreedyTool(graph, visits, solution, visits.m_Vertices.front(), track, 1, maxLength);

	return solution.top();
}
