#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <queue>

// =============================================================================
// ProgramacioDinamica_INICI_DESTI =============================================
// =============================================================================

CTrack DijkstraQueueINICI_DESTI(CGraph& graph)
{
	struct CPair {
		CVertex* m_pVertex;
		double m_Distance;
		CPair(CVertex* pV, double d) : m_pVertex(pV), m_Distance(d) {}
	};

	struct comparator {
		bool operator()(CPair& pair1, CPair& pair2) {
			return pair1.m_Distance > pair2.m_Distance;
		}
	};

	CVertex* pInici = graph.GetVertex("INICI");
	CVertex* pDesti = graph.GetVertex("DESTI");

	priority_queue<CPair, std::vector<CPair>, comparator> queue;
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraVisit = false;
		v.m_DijkstraDistance = numeric_limits<double>::max();
	}
	pInici->m_DijkstraDistance = 0;
	pInici->m_pDijkstraPrevious = NULL;
	queue.push(CPair(pInici, 0.0));
	while (!queue.empty()) {
		CVertex* pVMin = queue.top().m_pVertex;
		double pairDistance = queue.top().m_Distance;
		queue.pop();
		if (pairDistance == pVMin->m_DijkstraDistance) {
			pVMin->m_DijkstraVisit = true;
			for (CEdge* pE : pVMin->m_Edges) {
				double d = pVMin->m_DijkstraDistance + pE->m_Length;
				if (pE->m_pDestination->m_DijkstraDistance > d) {
					pE->m_pDestination->m_DijkstraDistance = d;
					pE->m_pDestination->m_pDijkstraPrevious = pE;
					queue.push(CPair(pE->m_pDestination, d));
				}
			}
		}
	}
	CTrack cami(&graph);
	while (pDesti != pInici) {
		cami.m_Edges.push_front(pDesti->m_pDijkstraPrevious);
		pDesti = pDesti->m_pDijkstraPrevious->m_pOrigin;
	}
	return cami;
}

// =============================================================================
// TrobaCamiProgramacioDinamicaBB ==============================================
// =============================================================================

CTrack TrobaCamiProgramacioDinamicaBB(CGraph& graph)
{
	struct CNode {
		CVertex* m_pVertex;
		double m_Distance;
		double m_Estimacio;
		CNode(CVertex* pV, double d, double estimacio) : m_pVertex(pV), m_Distance(d), m_Estimacio(estimacio) {}
	};

	struct comparator {
		bool operator()(CNode& node1, CNode& node2) {
			return node1.m_Estimacio > node2.m_Estimacio;
		}
	};

	CVertex* pInici = graph.GetVertex("INICI");
	CVertex* pDesti = graph.GetVertex("DESTI");
	SolutionNodesCreated = 0;
	SolutionNodesBranched = 0;
	priority_queue<CNode, std::vector<CNode>, comparator> queue;
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraVisit = false;
		v.m_DijkstraDistance = numeric_limits<double>::max();
	}
	pInici->m_DijkstraDistance = 0;
	pInici->m_pDijkstraPrevious = NULL;
	queue.push(CNode(pInici, 0.0, 0.0));
	++SolutionNodesCreated;
	while (!queue.empty()) {
		CVertex* pVMin = queue.top().m_pVertex;
		double pairDistance = queue.top().m_Distance;
		queue.pop();
		if (pairDistance == pVMin->m_DijkstraDistance) {
			pVMin->m_Color = RGB(255, 100, 0);
			if (pVMin == pDesti) break;
			for (CEdge* pE : pVMin->m_Edges) {
				pE->SetColor(RGB(255, 100, 0));
				double d = pVMin->m_DijkstraDistance + pE->m_Length;
				if (pE->m_pDestination->m_DijkstraDistance > d) {
					pE->m_pDestination->m_DijkstraDistance = d;
					pE->m_pDestination->m_pDijkstraPrevious = pE;
					queue.push(CNode(pE->m_pDestination, d, d+pDesti->m_Point.Distance(pE->m_pDestination->m_Point)));
					++SolutionNodesCreated;
				}
			}
			++SolutionNodesBranched;
		}
	}
	CTrack cami(&graph);
	while (pDesti!=pInici) {
		cami.m_Edges.push_front(pDesti->m_pDijkstraPrevious);
		pDesti = pDesti->m_pDijkstraPrevious->m_pOrigin;
	}
	return cami;
}
