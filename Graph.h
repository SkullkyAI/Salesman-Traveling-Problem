// AssertsMaximFluxe.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#pragma once

#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <vector>
#include <string>
#include <list>
#include <vector>

#include "CVImage.h"
using namespace std;
#undef max
#undef min

// =============================================================================
// UTILITATS ===================================================================
// =============================================================================

/*
template<class T> T max(T a, T b) 
{
	if (a > b) return a;
	else return b;
}

template<class T> T min(T a, T b)
{
	if (a < b) return a;
	else return b;
}
*/

double Clock();
string StrPrint(const char* Format, ...);

// =============================================================================
// DEBUG =======================================================================
// =============================================================================

bool MyAssertFun(const char*strcond, const char*FileName, int line, const char*Msg = NULL);

#ifdef NDEBUG
#define MyAssert(cond)
#else
#define MyAssert(cond) ((void) ((cond) || MyAssertFun(#cond, __FILE__, __LINE__)))
#endif

// =============================================================================
// EXCEPCIONS ==================================================================
// =============================================================================

class MyException : public exception {
public:
private:
	// Campos
	char m_Message[256];
public:
	MyException() {}
	MyException(const char *Format, ...);
	MyException(const MyException& rhs);
	virtual const char *what() const throw() { return m_Message; }
};


// =============================================================================
// CGPoint =====================================================================
// =============================================================================

class CGPoint
{

public:
	double m_X, m_Y;

	CGPoint() : m_X(0.0), m_Y(0.0) {}
	CGPoint(const double x, const double y = 0) : m_X(x), m_Y(y) {}
	CGPoint(const CGPoint&p) : m_X(p.m_X), m_Y(p.m_Y) {}
	//CGPoint(const CPoint &p) : m_X(p.x), m_Y(p.y) {}
	//operator CPoint() { return CPoint(int(m_X), int(m_Y)); }
	CGPoint& operator=(const CGPoint &p) {
		m_X = p.m_X;
		m_Y = p.m_Y;
		return *this;
	}
	CGPoint operator+(const CGPoint &p2)  const {
		return CGPoint(m_X + p2.m_X, m_Y + p2.m_Y);
	}
	CGPoint operator+=(const CGPoint &p2) {
		m_X += p2.m_X;
		m_Y += p2.m_Y;
		return *this;
	}
	CGPoint operator-(const CGPoint &p2)  const {
		return CGPoint(m_X - p2.m_X, m_Y - p2.m_Y);
	}
	CGPoint operator-=(const CGPoint &p2) {
		m_X -= p2.m_X;
		m_Y -= p2.m_Y;
		return *this;
	}
	CGPoint operator*(const double n) {
		return CGPoint(m_X*n,m_Y*n);
	}
	CGPoint operator*=(const double n) {
		m_X *= n;
		m_Y *= n;
		return *this;
	}
	CGPoint operator/(const double n) {
		return CGPoint(m_X/n, m_Y/n);
	}
	CGPoint operator/=(const double n) {
		m_X /= n;
		m_Y /= n;
		return *this;
	}
	CGPoint operator-()  const {
		return CGPoint(-m_X, -m_Y);
	}
	double operator*(const CGPoint &p2)  const {
		return m_X * p2.m_X + m_Y * p2.m_Y;
	}
	bool operator==(const CGPoint& p)  const {
		return m_X == p.m_X && m_Y == p.m_Y;
	}
	bool operator!=(const CGPoint& p)  const {
		return m_X != p.m_X || m_Y != p.m_Y;
	}
	double Module() const { return sqrt(m_X*m_X + m_Y * m_Y); }
	double Distance(const CGPoint &p2) const { return sqrt((m_X - p2.m_X)*(m_X - p2.m_X) + (m_Y - p2.m_Y)*(m_Y - p2.m_Y)); }
	double Distance2(const CGPoint& p2) const { return (m_X - p2.m_X) * (m_X - p2.m_X) + (m_Y - p2.m_Y) * (m_Y - p2.m_Y); }
};

inline CGPoint operator*(double v, const CGPoint& p) {
	return CGPoint(v*p.m_X, v*p.m_Y);
}

inline CGPoint operator*(const CGPoint& p, double v) {
	return CGPoint(v*p.m_X, v*p.m_Y);
}

inline CGPoint operator/(const CGPoint& p, double v) {
	return CGPoint(p.m_X / v, p.m_Y / v);
}

inline ostream& operator<< (ostream& s, const CGPoint& p) {
	s << "(" << p.m_X << ", " << p.m_Y << ")";
	return s;
}

inline CGPoint Min(const CGPoint& p1, const CGPoint& p2) {
	return CGPoint(min(p1.m_X, p2.m_X), min(p1.m_Y, p2.m_Y));
}

inline CGPoint Max(const CGPoint& p1, const CGPoint& p2) {
	return CGPoint(max(p1.m_X, p2.m_X), max(p1.m_Y, p2.m_Y));
}

// =============================================================================
// CGRect ======================================================================
// =============================================================================

class CGRect {
public:
	CGPoint m_P0;
	CGPoint m_P1;
	explicit CGRect(double x0, double y0, double x1, double y1) : m_P0(x0, y0), m_P1(x1, y1) {}
	explicit CGRect(const CGPoint &p0, const CGPoint &p1) : m_P0(p0), m_P1(p1) {}
	CGRect() {}
	CGRect(const CGRect &r) {
		m_P0 = r.m_P0;
		m_P1 = r.m_P1;
	}
#ifdef _MFC_VER
	CGRect(const CRect &r) {
		m_P0.m_X = r.left;
		m_P0.m_Y = r.top;
		m_P1.m_X = r.right;
		m_P1.m_Y = r.bottom;
	}
	operator CRect() {
		return CRect(int(m_P0.m_X), int(m_P0.m_Y), int(m_P1.m_X), int(m_P1.m_Y));
	}
#endif
	CGRect& operator=(const CGRect &r) {
		m_P0 = r.m_P0;
		m_P1 = r.m_P1;
		return *this;
	}
	void SetRectEmpty() {
		m_P0.m_X = 1;
		m_P0.m_Y = 1;
		m_P1.m_X = 0;
		m_P1.m_Y = 0;
	}
	bool EmptyP() const {
		return m_P0.m_X > m_P1.m_X ||
			m_P0.m_Y > m_P1.m_Y;
	}
	CGRect operator|(const CGRect &r) const {
		if (EmptyP()) return r;
		else if (r.EmptyP()) return *this;
		else return CGRect(
			min(m_P0.m_X, r.m_P0.m_X),
			min(m_P0.m_Y, r.m_P0.m_Y),
			max(m_P1.m_X, r.m_P1.m_X),
			max(m_P1.m_Y, r.m_P1.m_Y));
	}
	CGRect &operator|=(const CGRect &r) {
		if (EmptyP()) *this = r;
		else if (r.EmptyP());
		else *this = CGRect(
			min(m_P0.m_X, r.m_P0.m_X),
			min(m_P0.m_Y, r.m_P0.m_Y),
			max(m_P1.m_X, r.m_P1.m_X),
			max(m_P1.m_Y, r.m_P1.m_Y));
		return *this;
	}
	CGRect operator&(const CGRect &r) const {
		return CGRect(
			max(m_P0.m_X, r.m_P0.m_X),
			max(m_P0.m_Y, r.m_P0.m_Y),
			min(m_P1.m_X, r.m_P1.m_X),
			min(m_P1.m_Y, r.m_P1.m_Y));
	}
	CGRect &operator&=(const CGRect &r) {
		*this = CGRect(
			max(m_P0.m_X, r.m_P0.m_X),
			max(m_P0.m_Y, r.m_P0.m_Y),
			min(m_P1.m_X, r.m_P1.m_X),
			min(m_P1.m_Y, r.m_P1.m_Y));
		return *this;
	}
	void NormalizeRect()
	{
		if (m_P0.m_X > m_P1.m_X) {
			double tmp = m_P0.m_X;
			m_P0.m_X = m_P1.m_X;
			m_P1.m_X = tmp;
		}
		if (m_P0.m_Y > m_P1.m_Y) {
			double tmp = m_P0.m_Y;
			m_P0.m_Y = m_P1.m_Y;
			m_P1.m_Y = tmp;
		}
	}
	bool PtInRect(CGPoint point) const
	{
		return point.m_X >= m_P0.m_X && point.m_X < m_P1.m_X
			&& point.m_Y >= m_P0.m_Y && point.m_Y < m_P1.m_Y;
	}

	// Las medidas de ancho, alto y area
	double Area() const { return (m_P1.m_X - m_P0.m_X)*(m_P1.m_Y - m_P0.m_Y); }
	double Width() const { return m_P1.m_X - m_P0.m_X; }
	double Height() const { return m_P1.m_Y - m_P0.m_Y; }
	bool operator==(const CGRect& r)  const {
		return m_P0 == r.m_P0 && m_P1 == r.m_P1;
	}
	bool operator!=(const CGRect& r)  const {
		return m_P0 != r.m_P0 || m_P1 != r.m_P1;
	}
};

inline ostream& operator<< (ostream& s, const CGRect &r) {
	s << "(" << r.m_P0 << "-" << r.m_P1 << ")";
	return s;
}

// =============================================================================
// GRAPH Classes ===============================================================
// =============================================================================
/*
Els grafs son dirigits. Per construir un graf no dirigit hi ha dos arestes que
conecten els vertexs en les dos direccions. Aquetes arestes estan relacionades
amb apuntadors.
*/

class CEdge;
class CGraph;
class CMaxFlow;

// CVertex =====================================================================

class CVertex {
public:
	// Atributs generals de CVertex
	string m_Name; // Nom del vertex
	CGPoint m_Point; 
	COLORREF m_Color;
	list<CEdge*> m_Edges;
	// Atributos de Dijkstra
	double m_DijkstraDistance;
	bool m_DijkstraVisit;
	CEdge* m_pDijkstraPrevious;
	// Atributs per Prim
	bool m_PrimInTree;
	// Atributs per Kruskal
	int m_KruskalLabel;
	CVertex* m_pKruskalFather;
	int m_KruskalDeep;
	// Backtracking
	bool m_VertexToVisit;
	// Marcar
	bool m_Marca;
	// TrobaCami
	bool m_JaHePassat;

	unsigned int m_indexMatrix;

	// Métodes
	CEdge* FindEdge(const char* name);
	bool MemberP(CEdge *pEdge);
	bool NeighbordP(CVertex* pVertex);
	void Unlink(CEdge *pEdge);
	void ResetColor() {	m_Color = RGB(0, 128, 128);	}
	void SetColor(COLORREF color) { m_Color = color; }
	CVertex(const char* name, double x, double y)
		: m_Name(name)
		, m_Color(RGB(0, 128, 128))
		, m_Point(x, y)
		, m_DijkstraDistance(-1.0)
	{}
};

inline ostream& operator<< (ostream& s, const CVertex& v) {
	s << "VERTEX(" << v.m_Name << "," << v.m_Point << ")";
	return s;
}


// CEdge =======================================================================

class CEdge {
public:
	string m_Name; // Nom del edge
	COLORREF m_Color;
	double m_Length; // Valor que se le asocia al edge: longitud, peso, coste, etc.
	CVertex* m_pOrigin;
	CVertex* m_pDestination;
	CEdge* m_pReverseEdge; // En caso de grafo no dirigido cada arista tiene su inverso.
	bool m_Processed;
	// Bacltracking basado en aristas
	bool m_Used; // Ya se ha usado la arista en el camino
public:
	CEdge(const char* name, double length, CVertex* pOrigin, CVertex* pDestination, CEdge* pReverseEdge)
		: m_Name(name)
		, m_Color(RGB(0, 0, 255))
		, m_Length(length)
		, m_pOrigin(pOrigin)
		, m_pDestination(pDestination)
		, m_pReverseEdge(pReverseEdge)
	{}
	void ResetColor() { m_Color = RGB(0, 0, 255); }
	void SetColor(COLORREF color) {
		m_Color = color;
		if (m_pReverseEdge) m_pReverseEdge->m_Color = color;
	}

};

inline ostream& operator<< (ostream& s, const CEdge& e) {
	s << "EDGE(" << e.m_Name << "," << e.m_pOrigin->m_Name << "-->" << e.m_pDestination->m_Name << ", " << e.m_Length << ")";
	return s;
}

// CGraph ======================================================================

class CGraph {
public:
	list<CVertex> m_Vertices;
	list<CEdge> m_Edges;
	string m_Filename;
	string m_BackgroundFilename;
	CVImage* m_pBackground;
	bool m_Directed; // Directed graph o undirected graph

public:
	CGraph(bool directed);
	~CGraph();
	void Clear();
	void ClearDistances();
	void ResetColor();

	// Vertices
	CVertex* NewVertex(const char *name, double x, double y);
	CVertex* NewVertex(const CGPoint &point);
	void DeleteVertex(CVertex *pVertex);
	CVertex* FindVertex(const char *name);
	CVertex* FindVertex(const CGPoint &point, double radius);
	CVertex* GetVertex(const char *name);
	CVertex* GetVertex(const int index);
	int GetVertexIndex(const CVertex* pVertex);

	bool MemberP(CVertex *pVertex);
	size_t GetNVertices() { return m_Vertices.size(); }

	// Edges
	CEdge* NewEdge(const char* name, double value, const char* originName, const char* destionatioName);
	CEdge* NewEdge(const char* name, double value, CVertex* pOrigin, CVertex* pDestination);
	CEdge* NewEdge(CVertex *pOrigin, CVertex* pDestination);
	void DeleteEdge(CEdge *pEdge);
	CEdge* FindEdge(CVertex* pVOrigin, CVertex* pVDestination);
	CEdge* FindEdge(const char* name);
	CEdge* FindEdge(const CGPoint &point, double radius);
	bool MemberP(CVertex* pVOrigin, CVertex* pVDestination) { return FindEdge(pVOrigin, pVDestination) != NULL; }
	bool MemberP(CEdge *pEdge);
	size_t GetNEdges() { return m_Edges.size(); }
	void SetDistancesToEdgeLength();

	// Background
	void SetBackground(const char*filename);
	CVImage* GetBackgroundImage() {
		return m_pBackground;
	}


	void RandomCreation(int nVertices, int nEdges);
	void CreateGrid(int nRows, int nColumns);
	bool GreaterLengthDifferenceP(double l, double minLength);
	void ToPlannar();

	void Load(const char* filename);
	void Save(const char* filename);
	void LoadDistances(const char* filename);
	void SaveDistances(const char* filename);

	CGRect RectHull();
	bool Invariant();

	//CMaxFlow ReadMaxFlow(const char* filename);
};

// =============================================================================
// CVisits =====================================================================
// =============================================================================

class CVisits {
public:
	list<CVertex*> m_Vertices;
	CGraph* m_pGraph;
	//CVisits() {m_pGraph = NULL; }
	CVisits(CGraph* pGraph) {
		m_pGraph = pGraph;
	}
	void SetGraph(CGraph* pGraph) {
		Clear();
		m_pGraph = pGraph;
	}

	bool MemberP(CVertex* pVertex);
	void Add(CVertex* pVertex) { m_Vertices.push_back(pVertex); }
	void Delete(CVertex* pVertex);
	size_t GetNVertices() { return m_Vertices.size(); }
	void Clear() { m_Vertices.clear(); }
	void Save(const char* filename);
	void Load(const char* filename);
	void RandomCreation(int nVisits, bool ciclo);
};

ostream& operator<< (ostream& s, const CVisits& visits);


// =============================================================================
// CTrack ======================================================================
// =============================================================================

class CTrack {
public:
	list<CEdge*> m_Edges;
	CGraph* m_pGraph;
	//CTrack() { m_pGraph = NULL; }
	CTrack(CGraph* pGraph) {
		m_pGraph = pGraph;
	}

	CTrack(CGraph* pGraph, list<CEdge*> Edges) {
		m_pGraph = pGraph;
		m_Edges = Edges;
	}

	void SetGraph(CGraph* pGraph) {
		Clear();
		m_pGraph = pGraph;

	}
	CTrack(const CTrack& t) : m_pGraph(t.m_pGraph), m_Edges(t.m_Edges) {}
	CTrack operator=(const CTrack& t) {
		m_pGraph = t.m_pGraph;
		m_Edges = t.m_Edges;
		return *this;
	}
	void AddFirst(CEdge* pEdge) {
		m_Edges.push_front(pEdge);
	}
	void AddLast(CEdge* pEdge) {
		m_Edges.push_back(pEdge);
	}
	void Delete(CVertex* pVertex);
	void Delete(CEdge* pEdge);
	void Clear() {
		m_Edges.clear();
	}
	void AppendBefore(CTrack& t);
	void Append(CTrack& t);
	size_t GetNEdges() { return m_Edges.size(); }


	// Files ----------------------------------------------------------------	
	void Save(const char* filename);
	void Load(const char* filename);

	// Length -------------------------------------------------------------------
	double Length();

	bool MemberP(CEdge* pE);
	bool MemberP(CVertex* pV);
};

ostream& operator<< (ostream& s, const CTrack& track);

// =============================================================================
// CSpanningTree ===============================================================
// =============================================================================

class CSpanningTree {
public:
	list<CEdge*> m_Edges;
	CGraph* m_pGraph;
	//CTrack() { m_pGraph = NULL; }
	CSpanningTree(CGraph* pGraph) {
		m_pGraph = pGraph;
	}
	void SetGraph(CGraph* pGraph) {
		Clear();
		m_pGraph = pGraph;

	}
	CSpanningTree(const CTrack& t) : m_pGraph(t.m_pGraph), m_Edges(t.m_Edges) {}
	CSpanningTree operator=(const CSpanningTree& t) {
		m_pGraph = t.m_pGraph;
		m_Edges = t.m_Edges;
		return *this;
	}
	void Add(CEdge* pEdge) {
		m_Edges.push_front(pEdge);
	}
	void Delete(CVertex* pVertex);
	void Delete(CEdge* pEdge);
	void Clear() {
		m_Edges.clear();
	}
	size_t GetNEdges() { return m_Edges.size(); }


	// Files ----------------------------------------------------------------	
	void Save(const char* filename);
	void Load(const char* filename);

	// Length -------------------------------------------------------------------
	double Length();

	bool MemberP(CEdge* pE);
	bool MemberP(CVertex* pV);
};

ostream& operator<< (ostream& s, const CSpanningTree& track);

// =============================================================================
// CConvexHull =================================================================
// =============================================================================

class CConvexHull {
public:
	list<CVertex*> m_Vertices;
	CGraph* m_pGraph;
	//CVisits() {m_pGraph = NULL; }
	CConvexHull(CGraph* pGraph) {
		m_pGraph = pGraph;
	}
	void SetGraph(CGraph* pGraph) {
		Clear();
		m_pGraph = pGraph;
	}

	bool MemberP(CVertex* pVertex);
	void Delete(CVertex* pVertex);
	size_t GetNVertices() { return m_Vertices.size(); }
	void Clear() { m_Vertices.clear(); }
	void Save(const char* filename);
	void Load(const char* filename);
};

ostream& operator<< (ostream& s, const CConvexHull& ch);

// =============================================================================
// ALGORITHMS ==================================================================
// =============================================================================

extern int SolutionNodesCreated;
extern int SolutionNodesBranched;

CSpanningTree SpanningTreePrim(CGraph& g);
CSpanningTree SpanningTreeKruskal(CGraph& g);

void Dijkstra(CGraph& g, CVertex* pStart);
void DijkstraQueue(CGraph& g, CVertex* pStart);

CTrack SalesmanTrackGreedy(CGraph& g, CVisits& visits);

CTrack SalesmanTrackBacktracking(CGraph& g, CVisits& visits);
CTrack SalesmanTrackBacktrackingGreedy(CGraph& g, CVisits& visits);

CTrack SalesmanTrackBranchAndBound1(CGraph& g, CVisits& visits);
CTrack SalesmanTrackBranchAndBound2(CGraph& g, CVisits& visits);
CTrack SalesmanTrackBranchAndBound3(CGraph& g, CVisits& visits);

CTrack SalesmanTrackDynamicProgramming(CGraph& graph, CVisits& visits);

CTrack SalesmanTrackProbabilistic(CGraph& g, CVisits& visits);

CTrack TrobaCami1(CGraph& g);
CTrack TrobaCami2(CGraph& g);
CTrack TrobaCami3(CGraph& g);
CTrack TrobaCamiBranchAndBound(CGraph& g);
CTrack TrobaCamiBranchAndBoundLink(CGraph& g);
CTrack TrobaCamiBranchAndBoundLink2(CGraph& g);
CTrack DijkstraQueueINICI_DESTI(CGraph& graph);
CTrack TrobaCamiProgramacioDinamicaBB(CGraph& g);


CConvexHull QuickHull(CGraph& g);
