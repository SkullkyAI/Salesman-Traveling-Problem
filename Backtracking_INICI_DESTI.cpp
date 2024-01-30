#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>

#define PAS_A_PAS 1

// =============================================================================
//Backtracking_INICI_DESTI =====================================================
// =============================================================================

// ==============================================================================
// TrobaCami1 ===================================================================
// ==============================================================================

CTrack CamiMesCurt(NULL);
double LongitudCamiMesCurt;

// TrobaCami1 ===================================================================
// Funcio recursiva

void TrobaCami1(CTrack cami, CVertex* pActual, CVertex* pDesti)
{
#if PAS_A_PAS
	if (!cami.m_Edges.empty()) {
		pGraphApplicationDlg->m_Track = cami;
		pGraphApplicationDlg->m_GraphCtrl.Invalidate();
		WaitLoop();
	}
#endif

	if (pActual == pDesti) {
		//cout << "Cami " << cami.Length() << endl;
		if (cami.Length() < LongitudCamiMesCurt) {
			CamiMesCurt = cami;
			LongitudCamiMesCurt = cami.Length();

#if PAS_A_PAS
			pGraphApplicationDlg->m_Track2 = cami;
			pGraphApplicationDlg->m_GraphCtrl.Invalidate();
			WaitLoop();
#endif

		}
	}
	else if (cami.Length()< LongitudCamiMesCurt) {
		pActual->m_JaHePassat = true;
		for (CEdge* pE : pActual->m_Edges) {
			if (!pE->m_pDestination->m_JaHePassat) {
				cami.m_Edges.push_back(pE);
				TrobaCami1(cami, pE->m_pDestination, pDesti);
				cami.m_Edges.pop_back();
#if PAS_A_PAS
				pGraphApplicationDlg->m_Track = cami;
				pGraphApplicationDlg->m_GraphCtrl.Invalidate();
				WaitLoop();
#endif
			}
		}
		pActual->m_JaHePassat = false;
	}
}

// TrobaCami1 ===================================================================
// Funcio principal

CTrack TrobaCami1(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	CVertex* pDesti = g.GetVertex("DESTI");
	CamiMesCurt.Clear();
	LongitudCamiMesCurt = numeric_limits<double>::max();
	for (CVertex& v : g.m_Vertices) v.m_JaHePassat = false;
	CTrack cami(&g);
	TrobaCami1(cami, pInici, pDesti);
	pGraphApplicationDlg->m_Track2.Clear();
	return CamiMesCurt;
}

// ==============================================================================
// TrobaCami2 ===================================================================
// ==============================================================================

CVertex* pDesti;
CTrack CamiActual(NULL);
double LongitudCamiActual;

// TrobaCami2 ===================================================================
// Funcio recursiva


void TrobaCami2(CVertex* pActual)
{
	if (pActual == pDesti) {
		if (LongitudCamiActual < LongitudCamiMesCurt) {
			CamiMesCurt = CamiActual;
			LongitudCamiMesCurt = LongitudCamiActual;
		}
	}
	else if (LongitudCamiActual < LongitudCamiMesCurt) {
		pActual->m_JaHePassat = true;
		for (CEdge* pE : pActual->m_Edges) {
			if (!pE->m_pDestination->m_JaHePassat) {
				CamiActual.m_Edges.push_back(pE);
				LongitudCamiActual += pE->m_Length;
				TrobaCami2(pE->m_pDestination);
				CamiActual.m_Edges.pop_back();
				LongitudCamiActual -= pE->m_Length;
			}
		}
		pActual->m_JaHePassat = false;
	}
}


// TrobaCami2 ===================================================================
// Funcio principal

CTrack TrobaCami2(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	pDesti = g.GetVertex("DESTI");
	CamiMesCurt.Clear();
	CamiActual.Clear();
	CamiActual.SetGraph(&g);
	LongitudCamiMesCurt = numeric_limits<double>::max();
	LongitudCamiActual = 0.0;
	for (CVertex& v : g.m_Vertices) v.m_JaHePassat = false;
	TrobaCami2(pInici);
	return CamiMesCurt;
}

// ==============================================================================
// TrobaCami3 ===================================================================
// ==============================================================================

struct NodeCami {
	CEdge* m_pEdge;
	NodeCami* m_pAnterior;
};

// TrobaCami3 ===================================================================
// Funcio recursiva

void TrobaCami3(NodeCami *pAnterior, CVertex* pActual)
{
	if (pActual == pDesti) {
		if (LongitudCamiActual < LongitudCamiMesCurt) {
			CamiMesCurt.Clear();
			while (pAnterior) {
				CamiMesCurt.m_Edges.push_front(pAnterior->m_pEdge);
				pAnterior = pAnterior->m_pAnterior;
			}
			LongitudCamiMesCurt = LongitudCamiActual;
		}
	}
	else if (LongitudCamiActual < LongitudCamiMesCurt) {
		pActual->m_JaHePassat = true;
		NodeCami node;
		node.m_pAnterior = pAnterior;
		for (CEdge* pE : pActual->m_Edges) {
			if (!pE->m_pDestination->m_JaHePassat) {
				node.m_pEdge = pE;
				LongitudCamiActual += pE->m_Length;
				TrobaCami3(&node,pE->m_pDestination);
				LongitudCamiActual -= pE->m_Length;
			}
		}
		pActual->m_JaHePassat = false;
	}
}

// TrobaCami3 ===================================================================
// Funcio principal

CTrack TrobaCami3(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	pDesti = g.GetVertex("DESTI");
	CamiMesCurt.Clear();
	LongitudCamiMesCurt = numeric_limits<double>::max();
	LongitudCamiActual = 0.0;
	for (CVertex& v : g.m_Vertices) v.m_JaHePassat = false;
	TrobaCami3(NULL,pInici);
	return CamiMesCurt;
}