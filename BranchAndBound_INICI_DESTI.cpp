#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <queue>

// =============================================================================
// Branch&bound_INICI_DESTI ====================================================
// =============================================================================

int SolutionNodesCreated;
int SolutionNodesBranched;

// =============================================================================
// TrobaCamiBranchAndBound =====================================================
// =============================================================================

// CBBNode ===============================================================

class CBBNode {
public:
	CTrack m_Track;
	double m_Length;
	double m_WeigthMin; // Cota inferior. peso del nodo a más pesado más largo es su posible camino
public:
	CBBNode(CGraph* pGraph, CEdge *pEdge, CVertex *pDestination)
		: m_Track(pGraph)
		, m_Length(pEdge->m_Length)
		, m_WeigthMin(m_Length+pDestination->m_Point.Distance(pEdge->m_pDestination->m_Point))
	{
		m_Track.AddLast(pEdge);
	}
	CBBNode(const CBBNode& node)
		: m_Track(node.m_Track)
		, m_Length(node.m_Length)
		, m_WeigthMin(node.m_WeigthMin)
	{
	}
};

// comparator ==================================================================

struct comparator {
	bool operator()(const CBBNode* s1, const CBBNode* s2) {
		return s1->m_WeigthMin > s2->m_WeigthMin;
	}
};

// TrobaCamiBranchAndBound ======================================================

CTrack TrobaCamiBranchAndBound(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	CVertex* pDesti = g.GetVertex("DESTI");
	SolutionNodesCreated = 0;
	SolutionNodesBranched = 0;
	priority_queue<CBBNode*, std::vector<CBBNode*>, comparator> queue;
	pInici->SetColor(RGB(255, 100, 0));
	for (CEdge* pE : pInici->m_Edges) {
		pE->SetColor(RGB(255, 100, 0));
		queue.push(new CBBNode(&g, pE, pDesti));
		++SolutionNodesCreated;
	}
	double maxLength = numeric_limits<double>::max();
	CTrack optimum(&g);
	while (!queue.empty()) {
		CBBNode* pS = queue.top();
		//cout << pS->m_Track << " " << pS->m_WeigthMin << " " << pS->m_Length << endl;
		//cout << pS->m_WeigthMin << " " << pS->m_Length << endl;
		queue.pop();
		//cout << pS->m_Track << endl;
		CVertex* pV = pS->m_Track.m_Edges.back()->m_pDestination;
		pV->m_Color = RGB(255, 100, 0);
		if (pV == pDesti) {
			// Solucion encontrada
			optimum = pS->m_Track;
			delete pS;
			break;
		}
		for (CEdge* pE : pV->m_Edges) {
			double w = pS->m_Length + pE->m_Length+pDesti->m_Point.Distance(pE->m_pDestination->m_Point);
			if (w < maxLength) {
				for (CEdge* pE1 : pS->m_Track.m_Edges) {
					if (pE1->m_pOrigin == pE->m_pDestination) goto Bound;
				}
				CBBNode* pSNew = new CBBNode(*pS);
				++SolutionNodesCreated;
				pSNew->m_Length = pS->m_Length + pE->m_Length;
				pSNew->m_WeigthMin = w;
				pSNew->m_Track.AddLast(pE);
				pE->SetColor(RGB(255, 100, 0));
				queue.push(pSNew);
				if (pE->m_pDestination == pDesti) maxLength = w;
			}
		Bound:;
		}
		++SolutionNodesBranched;
		delete pS;
	}
	//cout << "QUEDA EN COLA" << endl;
	while (!queue.empty()) {
		CBBNode* pS = queue.top();
		queue.pop();
		//cout << pS->m_Track << endl;
		delete pS;
	}
	return optimum;
}


// =============================================================================
// TrobaCamiBranchAndBound =====================================================
// =============================================================================

// CBBLinkNode ===============================================================

class CBBLinkNode {
public:
	CBBLinkNode* m_pFather;
	CEdge* m_pEdge;
	double m_Length;
	double m_WeigthMin; // Cota inferior. peso del nodo a más pesado más largo es su posible camino
	unsigned m_CntRef;
public:
	CBBLinkNode(CEdge* pEdge, CVertex* pDestination)
		: m_pFather(NULL)
		, m_Length(pEdge->m_Length)
		, m_pEdge(pEdge)
		, m_WeigthMin(m_Length + pDestination->m_Point.Distance(pEdge->m_pDestination->m_Point))
		, m_CntRef(1)
	{
	}
	CBBLinkNode(CBBLinkNode *pFather, CEdge* pEdge, double weigthMin)
		: m_pFather(pFather)
		, m_pEdge(pEdge)
		, m_Length(pFather->m_Length+pEdge->m_Length)
		, m_WeigthMin(weigthMin)
		, m_CntRef(1)
	{
		++m_pFather->m_CntRef;
	}
	void Unlink() 
	{
		if (--m_CntRef == 0) {
			if (m_pFather) m_pFather->Unlink();
			delete this;
		}
	}
};

// comparator ==================================================================

struct LinkNodeComparator {
	bool operator()(const CBBLinkNode* s1, const CBBLinkNode* s2) {
		return s1->m_WeigthMin > s2->m_WeigthMin;
	}
};

// TrobaCamiBranchAndBoundLink ======================================================

CTrack TrobaCamiBranchAndBoundLink(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	CVertex* pDesti = g.GetVertex("DESTI");
	SolutionNodesCreated = 0;
	SolutionNodesBranched = 0;
	priority_queue<CBBLinkNode*, std::vector<CBBLinkNode*>, LinkNodeComparator> queue;
	pInici->SetColor(RGB(255, 100, 0));
	for (CEdge* pE : pInici->m_Edges) {
		pE->SetColor(RGB(255, 100, 0));
		queue.push(new CBBLinkNode(pE,pDesti));
		++SolutionNodesCreated;
	}
	double maxLength = numeric_limits<double>::max();
	CTrack optimum(&g);
	//cout << "BUCLE" << endl;
	while (!queue.empty()) {
		CBBLinkNode* pS = queue.top();
		queue.pop();
		//cout << pS->m_Track << endl;
		CVertex* pV = pS->m_pEdge->m_pDestination;
		pV->m_Color = RGB(255, 100, 0);
		if (pV == pDesti) {
			// Solucion encontrada
			for (CBBLinkNode* pN = pS; pN; pN = pN->m_pFather) optimum.AddFirst(pN->m_pEdge);
			pS->Unlink();
			break;
		}
		for (CEdge* pE : pV->m_Edges) {
			double w = pS->m_Length + pE->m_Length + pDesti->m_Point.Distance(pE->m_pDestination->m_Point);
			if (w < maxLength) {
				CBBLinkNode* pLN = pS;
				while (pLN) {
					if (pLN->m_pEdge->m_pOrigin == pE->m_pDestination) goto Bound;
					pLN = pLN->m_pFather;
				}
				CBBLinkNode* pSNew = new CBBLinkNode(pS,pE,w);
				++SolutionNodesCreated;
				pE->SetColor(RGB(255, 100, 0));
				queue.push(pSNew);
				if (pE->m_pDestination == pDesti) maxLength = w;
			}
		Bound:;
		}
		++SolutionNodesBranched;
		pS->Unlink();
	}
	//cout << "QUEDA EN COLA" << endl;
	while (!queue.empty()) {
		CBBLinkNode* pS = queue.top();
		queue.pop();
		//cout << pS->m_Track << endl;
		pS->Unlink();
	}
	return optimum;
}


// =============================================================================
// TrobaCamiBranchAndBound =====================================================
// =============================================================================

// CBBLinkNode2 ===============================================================

class CBBLinkNode2 {
public:
	CBBLinkNode2* m_pFather;
	CEdge* m_pEdge;
	double m_Length;
	double m_MinLength;
	double m_Weigth; // Peso para no ordenar que no es una cota inferior ni superior (no sirve para podar)
	unsigned m_CntRef;
public:
	CBBLinkNode2(CEdge* pEdge, CVertex* pDestination)
		: m_pFather(NULL)
		, m_Length(pEdge->m_Length)
		, m_MinLength(m_Length + pDestination->m_Point.Distance(pEdge->m_pDestination->m_Point))
		, m_pEdge(pEdge)
		, m_Weigth(m_Length + pDestination->m_Point.Distance2(pEdge->m_pDestination->m_Point))
		, m_CntRef(1)
	{
	}
	CBBLinkNode2(CBBLinkNode2* pFather, CEdge* pEdge, double minLength, double weigth)
		: m_pFather(pFather)
		, m_pEdge(pEdge)
		, m_Length(pFather->m_Length + pEdge->m_Length)
		, m_MinLength(minLength)
		, m_Weigth(weigth)
		, m_CntRef(1)
	{
		++m_pFather->m_CntRef;
	}
	void Unlink()
	{
		if (--m_CntRef == 0) {
			if (m_pFather) m_pFather->Unlink();
			delete this;
		}
	}
};

// comparator ==================================================================

struct comparatorLink2 {
	bool operator()(const CBBLinkNode2* s1, const CBBLinkNode2* s2) {
		return s1->m_Weigth > s2->m_Weigth;
	}
};

// TrobaCamiBranchAndBoundLink ======================================================

CTrack TrobaCamiBranchAndBoundLink2(CGraph& g)
{
	CVertex* pInici = g.GetVertex("INICI");
	CVertex* pDesti = g.GetVertex("DESTI");
	SolutionNodesCreated = 0;
	SolutionNodesBranched = 0;
	priority_queue<CBBLinkNode2*, std::vector<CBBLinkNode2*>, comparatorLink2> queue;
	pInici->SetColor(RGB(255, 100, 0));
	for (CEdge* pE : pInici->m_Edges) {
		pE->SetColor(RGB(255, 100, 0));
		queue.push(new CBBLinkNode2(pE, pDesti));
		++SolutionNodesCreated;
	}
	double maxLength = numeric_limits<double>::max();
	CTrack optimum(&g);
	//cout << "BUCLE" << endl;
	while (!queue.empty()) {
		CBBLinkNode2* pS = queue.top();
		queue.pop();
		//cout << pS->m_Track << endl;
		CVertex* pV = pS->m_pEdge->m_pDestination;
		pV->m_Color = RGB(255, 100, 0);
		for (CEdge* pE : pV->m_Edges) {
			double length = pS->m_Length + pE->m_Length;
			double minLength=length+ pDesti->m_Point.Distance(pE->m_pDestination->m_Point);
			double w = length + pDesti->m_Point.Distance2(pE->m_pDestination->m_Point);
			if (minLength < maxLength) {
				CBBLinkNode2* pLN = pS;
				while (pLN) {
					if (pLN->m_pEdge->m_pOrigin == pE->m_pDestination) goto Bound;
					pLN = pLN->m_pFather;
				}
				if (pE->m_pDestination == pDesti) {
					CBBLinkNode2* pLN = pS;
					maxLength = length;
					optimum.Clear();
					optimum.AddFirst(pE);
					for (CBBLinkNode2* pN = pS; pN; pN = pN->m_pFather) optimum.AddFirst(pN->m_pEdge);
				}
				else {
					CBBLinkNode2* pSNew = new CBBLinkNode2(pS, pE, minLength, w);
					++SolutionNodesCreated;
					pE->SetColor(RGB(255, 100, 0));
					queue.push(pSNew);
				}
			}
		Bound:;
		}
		++SolutionNodesBranched;
		pS->Unlink();
	}
	//cout << "QUEDA EN COLA" << endl;
	while (!queue.empty()) {
		CBBLinkNode2* pS = queue.top();
		queue.pop();
		//cout << pS->m_Track << endl;
		pS->Unlink();
	}
	return optimum;
}