// AssertsMaximFluxe.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include "pch.h"
#include "Graph.h"
#include <stdarg.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
using namespace std;

// =============================================================================
// UTILITATS ===================================================================
// =============================================================================

// Clock =======================================================================

double Clock()
{
	LARGE_INTEGER cnt;
	LARGE_INTEGER fre;

	if (QueryPerformanceFrequency(&fre) && QueryPerformanceCounter(&cnt)) {
		//cout << 1.0 / (double)fre.QuadPart << endl;
		return (double)cnt.QuadPart / (double)fre.QuadPart;
	}
	else {
		//return 0.0;
		return (double)clock() / (double)CLOCKS_PER_SEC;
	}
}

// StrPrint ====================================================================

string StrPrint(const char* Format, ...)
{
	va_list al;
	va_start(al, Format);
	char buf[1024];
	_vsnprintf_s(buf, 1024, Format, al);
	buf[1023] = 0;
	va_end(al);
	return buf;
}

// =============================================================================
// DEBUG =======================================================================
// =============================================================================

bool MyAssertFun(const char*strcond, const char*FileName, int line, const char*Msg)
{
	char buff[1024];

	sprintf_s(buff, "Assertion failed\r\nFile \"%s\"\r\nLine %d\r\nCond: %s\r\nMessage: %s",
		FileName, line, strcond, Msg ? Msg : "");
	throw MyException(buff);
	return true;
}

// =============================================================================
// EXCEPCIONS ==================================================================
// =============================================================================

// MyException =================================================================

MyException::MyException(const char *Format, ...)
{
	va_list al;
	va_start(al, Format);
	_vsnprintf_s(m_Message, 255, Format, al);
	m_Message[255] = 0;
	va_end(al);
}

// MyException =================================================================

MyException::MyException(const MyException& rhs) : exception(rhs)
{
	memcpy(m_Message, rhs.m_Message, sizeof m_Message);
}

// =============================================================================
// CGPoint =====================================================================
// =============================================================================

// =============================================================================
// CVertex =====================================================================
// =============================================================================

// MemberP =====================================================================

bool CVertex::MemberP(CEdge* pEdge)
{
	for (CEdge* pE : m_Edges) if (pE == pEdge) return true;
	return false;
}

// NeighbordP ==================================================================

bool CVertex::NeighbordP(CVertex* pVertex)
{
	for (CEdge* pE : m_Edges) if (pE->m_pDestination == pVertex) return true;
	return false;

}

// FindEdge =====================================================================

CEdge* CVertex::FindEdge(const char* name)
{
	for (CEdge* pEdge : m_Edges) {
		if (strcmp(pEdge->m_Name.c_str(), name)==0) return pEdge;
	}
	return NULL;
}

// Unlink ======================================================================

void CVertex::Unlink(CEdge *pEdge)
{
	for (list<CEdge*>::iterator it = m_Edges.begin(); it != m_Edges.end(); ++it) {
		if (pEdge == *it) {
			m_Edges.erase(it);
			break;
		}
	}
}

// =============================================================================
// CGraph ======================================================================
// =============================================================================

// CGraph ======================================================================

CGraph::CGraph(bool directed)
{
	m_pBackground = NULL;
	m_Directed=directed;
}

// ~CGraph =====================================================================

CGraph::~CGraph()
{
	if (m_pBackground) delete m_pBackground;
}

// Clear =======================================================================

void CGraph::Clear()
{
	m_Vertices.clear();
	m_Edges.clear();
	if (m_pBackground) delete m_pBackground;
	m_pBackground = NULL;
	m_BackgroundFilename.clear();
}

// ClearDistances ==============================================================

void CGraph::ClearDistances()
{
	for (CVertex& v : m_Vertices) v.m_DijkstraDistance = -1.0;
}


// ResetColor ==================================================================

void  CGraph::ResetColor()
{
	for (CVertex& v : m_Vertices) v.ResetColor();
	for (CEdge& e : m_Edges) e.ResetColor();
}

// NewVertex ==================================================================

CVertex* CGraph::NewVertex(const char* name, double x, double y)
{
	m_Vertices.push_back(CVertex(name, x, y));
	return &m_Vertices.back();
}

// NewVertex ==================================================================

CVertex* CGraph::NewVertex(const CGPoint &point)
{
	char name[10];
	size_t i = m_Vertices.size()+1;
	for (;;) {
		sprintf_s(name, "V%04llu", i);
		if (FindVertex(name) == NULL) break;
		++i;
	}
	m_Vertices.push_back(CVertex(name, point.m_X, point.m_Y));
	return &m_Vertices.back();
}

// DeleteVertex ==================================================================

void CGraph::DeleteVertex(CVertex *pVertex)
{
	for (list<CVertex>::iterator it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
		if (pVertex == &*it) {
			while (!pVertex->m_Edges.empty()) {
				DeleteEdge(pVertex->m_Edges.front());
			}
			m_Vertices.erase(it);
			break;
		}
	}
}

// FindVertex =================================================================

CVertex* CGraph::FindVertex(const char* name)
{
	for (CVertex &v : m_Vertices) {
		if (strcmp(v.m_Name.c_str(),name)==0) return &v;
	}
	return NULL;
}

// FindVertex =================================================================

CVertex* CGraph::FindVertex(const CGPoint &point,double radius)
{
	CVertex *pSel = NULL;
	double dMin = numeric_limits<double>::max();
	for (CVertex &v : m_Vertices) {
		double d = (v.m_Point - point).Module();
		if (d < dMin) {
			dMin = d;
			pSel = &v;
		}
	}
	if (dMin <= radius) return pSel;
	return NULL;
}

// GetVertex ===================================================================

CVertex* CGraph::GetVertex(const char* name)
{
	CVertex* pV = FindVertex(name);
	if (!pV) throw MyException("CGraph::GetVertex: no se ha encontrado un vértice del grafo con nombre: %s", name);
	return pV;
}

// GetVertex ===================================================================

CVertex* CGraph::GetVertex(const int index)
{
	int i = index;
	for (CVertex& v : m_Vertices) {
		if (i == 0) return &v;
		--i;
	}
	throw MyException("CGraph::GetVertex: no se ha encontrado un vértice del grafo con indice: %d", index);

}

// GetVertexIndex ==============================================================

int CGraph::GetVertexIndex(const CVertex* pVertex)
{
	int indice = 0;
	for (CVertex& v : m_Vertices) {
		if (&v == pVertex) return indice;
		++indice;
	}
	throw exception("CGraph::GetVertexIndex: no ha encontrado el vértice");
}

// MemberP =====================================================================

bool CGraph::MemberP(CVertex* pVertex)
{
	for (CVertex &v : m_Vertices) {
		if (&v==pVertex) return true;
	}
	return false;
}

// NewEdge =====================================================================

CEdge* CGraph::NewEdge(const char* name, double value, const char* originName, const char* destionatioName)
{
	CVertex* pVOrigin = FindVertex(originName);
	CVertex* pVDestination = FindVertex(destionatioName);
	MyAssert(pVOrigin);
	MyAssert(pVDestination);
	MyAssert(pVOrigin != pVDestination);
	//cout << pVOrigin->m_Name << "->" << pVDestination->m_Name << endl;
	//MyAssert(!MemberP(pVOrigin, pVDestination));
	if (m_Directed) {
		m_Edges.push_back(CEdge(name, value, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		return pEdge;
	}
	else {
		m_Edges.push_back(CEdge(name, value, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		string rname(name);
		rname += "$Reverse";
		m_Edges.push_back(CEdge(rname.c_str(), value, pVDestination, pVOrigin, NULL));
		CEdge* pRevEdge = &m_Edges.back();
		pVDestination->m_Edges.push_back(pRevEdge);
		pEdge->m_pReverseEdge = pRevEdge;
		pRevEdge->m_pReverseEdge = pEdge;
		return pEdge;
	}
}
// NewEdge =====================================================================

CEdge* CGraph::NewEdge(const char* name, double value, CVertex* pVOrigin, CVertex* pVDestination)
{
	MyAssert(pVOrigin);
	MyAssert(pVDestination);
	MyAssert(pVOrigin != pVDestination);
	//MyAssert(MemberP(pVOrigin));
	//MyAssert(MemberP(pVDestination));
	if (m_Directed) {
		m_Edges.push_back(CEdge(name, value, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		return pEdge;
	}
	else {
		m_Edges.push_back(CEdge(name, value, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		string rname(name);
		rname += "$Reverse";
		m_Edges.push_back(CEdge(rname.c_str(), value, pVDestination, pVOrigin, NULL));
		CEdge* pRevEdge = &m_Edges.back();
		pVDestination->m_Edges.push_back(pRevEdge);
		pEdge->m_pReverseEdge = pRevEdge;
		pRevEdge->m_pReverseEdge = pEdge;
		return pEdge;
	}

}

// NewEdge =====================================================================

CEdge* CGraph::NewEdge(CVertex *pVOrigin, CVertex* pVDestination)
{
	MyAssert(pVOrigin);
	MyAssert(pVDestination);
	MyAssert(pVOrigin != pVDestination);
	//MyAssert(MemberP(pVOrigin));
	//MyAssert(MemberP(pVDestination));
	if (m_Directed) {
		char name[10];
		size_t i = m_Edges.size() + 1;
		for (;;) {
			sprintf_s(name, "E%04llu", i);
			if (FindEdge(name) == NULL) break;
			++i;
		}
		m_Edges.push_back(CEdge(name, 0.0, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		return pEdge;
	}
	else {
		char name[10];
		size_t i = m_Edges.size() + 1;
		for (;;) {
			sprintf_s(name, "E%04llu", i);
			if (FindEdge(name) == NULL) break;
			++i;
		}
		m_Edges.push_back(CEdge(name, 0.0, pVOrigin, pVDestination, NULL));
		CEdge* pEdge = &m_Edges.back();
		pVOrigin->m_Edges.push_back(pEdge);
		string rname(name);
		rname += "$Reverse";
		m_Edges.push_back(CEdge(rname.c_str(), 0.0, pVDestination, pVOrigin, NULL));
		CEdge* pRevEdge = &m_Edges.back();
		pVDestination->m_Edges.push_back(pRevEdge);
		pEdge->m_pReverseEdge = pRevEdge;
		pRevEdge->m_pReverseEdge = pEdge;
		return pEdge;
	}
}

// DeleteEdge ==================================================================
// borra areste en graf dirigits y borra parelles d'arestes en graf no dirigits

void CGraph::DeleteEdge(CEdge *pEdge)
{
	for (list<CEdge>::iterator it = m_Edges.begin(); it != m_Edges.end(); ++it) {
		if (pEdge == &*it) {
			if (pEdge->m_pReverseEdge) {
				CEdge* pRevEdge = pEdge->m_pReverseEdge;
				pEdge->m_pOrigin->Unlink(pEdge);
				m_Edges.erase(it);
				for (list<CEdge>::iterator itr = m_Edges.begin(); itr != m_Edges.end(); ++itr) {
					if (pRevEdge == &*itr) {
						pRevEdge->m_pOrigin->Unlink(pRevEdge);
						m_Edges.erase(itr);
						break;
					}
				}
			}
			else {
				pEdge->m_pOrigin->Unlink(pEdge);
				m_Edges.erase(it);
			}
			break;
		}
	}
}

// FindEdge ====================================================================

CEdge* CGraph::FindEdge(CVertex* pVOrigin, CVertex* pVDestination)
{
	for (CEdge &e : m_Edges) {
		if (e.m_pOrigin == pVOrigin && e.m_pDestination == pVDestination) return &e;
	}
	return NULL;
}

// FindEdge ====================================================================

CEdge* CGraph::FindEdge(const char* name)
{
	for (CEdge &e : m_Edges) {
		if (strcmp(e.m_Name.c_str(), name) == 0) return &e;
	}
	return NULL;
}

// FindEdge ====================================================================

CEdge* CGraph::FindEdge(const CGPoint &point, double radius)
{
	CEdge *pEdge = NULL;
	double minD = numeric_limits<double>::max();
	for (CEdge &e : m_Edges) {
		CGPoint u = e.m_pDestination->m_Point - e.m_pOrigin->m_Point;
		u /= u.Module();
		CGPoint v2 = point - e.m_pOrigin->m_Point;
		double a = v2 * u;
		double d;
		if (a < 0) d = e.m_pOrigin->m_Point.Distance(point);
		else if (a> e.m_pOrigin->m_Point.Distance(e.m_pDestination->m_Point)) d = e.m_pDestination->m_Point.Distance(point);
		else d = (v2 - u * a).Module();
		if (d < minD) {
			minD = d;
			pEdge = &e;
		}
	}
	if (minD <= radius) return pEdge;
	return NULL;
}

// MemberP =====================================================================

bool CGraph::MemberP(CEdge *pEdge)
{
	for (CEdge &e : m_Edges) {
		if (&e == pEdge) return true;
	}
	return false;
}

// SetDistancesToEdgeLength ====================================================

void CGraph::SetDistancesToEdgeLength()
{
	for (CEdge& e : m_Edges) {
		e.m_Length = e.m_pOrigin->m_Point.Distance(e.m_pDestination->m_Point);
	}
}

// SetBackground ===============================================================

void CGraph::SetBackground(const char* filename)
{
	if (m_pBackground) delete m_pBackground;
	char path[_MAX_DRIVE+ _MAX_DIR+ _MAX_FNAME+ _MAX_EXT];
	char dir[_MAX_DIR];
	_splitpath_s(m_Filename.c_str(), path, _MAX_DRIVE, dir, _MAX_DIR, NULL,0, NULL,0);
	strcat_s(path, dir);
	strcat_s(path, filename);

	try {
		m_pBackground = CVReadImage(path);
		m_BackgroundFilename = filename;
	}
	catch (exception &ex) {
		AfxMessageBox(ex.what(), MB_ICONEXCLAMATION);
		m_pBackground = NULL;
		m_BackgroundFilename.clear();
	}
}

// Load ========================================================================

void CGraph::Load(const char* filename)
{
	ifstream file(filename);
	if (!file.good()) throw MyException("Error obrint graf %s", filename);
	// Llegir la capcelera
	char buff[255];
	file.getline(buff, 255);
	if (strcmp("GRAPH 1.0", buff) != 0) throw MyException("Falta la capcalera GRAPH 1.0 al fitxer %s", filename);
	// Netejar el graf
	Clear();
	m_Filename = filename;
	file.getline(buff, 255);
	if (strncmp("BACKGROUND ", buff, strlen("BACKGROUND ")) == 0) {
		cout << buff + strlen("BACKGROUND ") << endl;
		SetBackground(buff + strlen("BACKGROUND "));
		file.getline(buff, 255);
	}
	m_Directed = strcmp("DIRECTED", buff) == 0;
	file.getline(buff, 255);
	if (strcmp("VERTICES",buff)!=0) throw MyException("Falta VERTEXS al fitxer %s", filename);
	// LLegir els vertexs
	cout << "Leer vertices" << endl;
	for (;;) {
		file.getline(buff, 256);
		if (strcmp(buff, "EDGES") == 0) break;
		double x, y;
		char name[256];
		sscanf_s(buff, "%s %lf %lf",name,256,&x,&y);
		NewVertex(name, x, y);
	}
	// Llegir arestes
	cout << "Leer Aristas" << endl;
	for (;;) {
		file.getline(buff, 255);
		if (file.eof()) break;
		double value;
		char name[256], name1[256], name2[256];
		sscanf_s(buff, "%s %lf %s %s", name, 256, &value, name1,256, name2,256);
		NewEdge(name,value,name1, name2);
	}
	file.close();
	cout << "Fin lectura" << endl;
	/*
	if (!Invariant()) {
		cout << "NO CUMPLE LA INVARIANTE" << endl;
	}
	cout << "Fin verificar la invariante" << endl;
	*/
	// Completar el graph
	/*
	m_Edges.clear();
	for (list<CVertex>::iterator i = m_Vertices.begin();;) {
		CVertex* pV = &(*i);
		++i;
		if (i == m_Vertices.end()) break;
		for (list<CVertex>::iterator j = i; j != m_Vertices.end(); ++j) {
			NewEdge(pV, &(*j));
		}
	}
	*/
}

// Save ========================================================================

void CGraph::Save(const char* filename)
{
	ofstream file(filename);
	if (!file.good()) throw MyException("Error obrint fitxer %s", filename);
	file << "GRAPH 1.0" << endl;
	if (m_pBackground) file << "BACKGROUND " << m_BackgroundFilename << endl;
	if (m_Directed) file << "DIRECTED" << endl;
	else file << "UNDIRECTED" << endl;
	file << "VERTICES" << endl;
	for ( CVertex &v : m_Vertices) {
		file << v.m_Name << " " << setprecision(20) << v.m_Point.m_X << " " << setprecision(20) << v.m_Point.m_Y << endl;
	}
	file << "EDGES" << endl;
	for (CEdge& e : m_Edges) e.m_Processed = false;
	if (m_Directed) {
		for (CEdge& e : m_Edges) {
			file << e.m_Name << " " << setprecision(20) << e.m_Length << " " << e.m_pOrigin->m_Name << " " << e.m_pDestination->m_Name << endl;
		}
	}
	else {
		for (CEdge& e : m_Edges) {
			if (!e.m_Processed) {
				file << e.m_Name << " " << e.m_Length << " " << e.m_pOrigin->m_Name << " " << e.m_pDestination->m_Name << endl;
				e.m_pReverseEdge->m_Processed = true;
			}
		}
	}
	file.close();
}

// LoadDistances ========================================================================

void CGraph::LoadDistances(const char* filename)
{
	ifstream file(filename);
	if (!file.good()) throw MyException("Error obrint graf %s", filename);
	// Llegir la capcelera
	char buff[255];
	file.getline(buff, 255);
	if (strcmp("DISTANCES 1.0", buff) != 0) throw MyException("Falta la capcalera DISTANCES 1.0 al fitxer %s", filename);
	// LLegir les distancies
	for (;;) {
		file.getline(buff, 256);
		if (file.eof()) break;
		double d;
		char name[256];
		sscanf_s(buff, "%s %lf", name, 256, &d);
		GetVertex(name)->m_DijkstraDistance = d;
	}
	file.close();
}

// SaveDistances ========================================================================

void CGraph::SaveDistances(const char* filename)
{
	ofstream file(filename);
	if (!file.good()) throw MyException("Error obrint fitxer %s", filename);
	file << "DISTANCES 1.0" << endl;
	for (CVertex& v : m_Vertices) {
		file << v.m_Name << " " << setprecision(20) <<  v.m_DijkstraDistance << endl;
	}
	file.close();
}

// RectHull ====================================================================

CGRect CGraph::RectHull()
{
	CGPoint mins(numeric_limits<double>::max(), numeric_limits<double>::max());
	CGPoint maxs(numeric_limits<double>::lowest(), numeric_limits<double>::lowest());
	if (m_pBackground) {
		mins.m_X = m_pBackground->GetOrigenX();
		mins.m_Y = m_pBackground->GetOrigenY();
		maxs.m_X = m_pBackground->GetOrigenX()+m_pBackground->GetszX();
		maxs.m_Y = m_pBackground->GetOrigenY() + m_pBackground->GetszY();
	}
	for (CVertex &v : m_Vertices) {
		mins = Min(mins, v.m_Point);
		maxs = Max(maxs, v.m_Point);
	}
	if (mins.m_X >= maxs.m_X) return CGRect(0, 0, 1000, 1000);
	else return CGRect(mins, maxs);
}

// Invariant ===================================================================

bool CGraph::Invariant()
{
	SetDistancesToEdgeLength();
	cout << "Verficar pertenencia vertices" << endl;
	for (CVertex& v : m_Vertices) {
		for (CEdge* pE : v.m_Edges) {
			if (!MemberP(pE)) return false;
			if (pE->m_pOrigin != &v) return false;
		}
	}
	cout << "Verficar pertenencia edges" << endl;
	for (CEdge& e : m_Edges) {
		if (!MemberP(e.m_pOrigin)) return false;
		if (!MemberP(e.m_pDestination)) return false;
		if (m_Directed) {
			if (e.m_pReverseEdge != NULL) return false;
		}
		else {
			if (e.m_pReverseEdge == NULL) return false;
			if (!MemberP(e.m_pReverseEdge)) return false;
			if (e.m_pOrigin != e.m_pReverseEdge->m_pDestination) return false;
			if (e.m_pDestination != e.m_pReverseEdge->m_pOrigin) return false;
		}
		if (e.m_pOrigin == e.m_pDestination) return false;
	}
	cout << "Verificar no repeticion nombres vertices" << endl;
	for (list<CVertex>::iterator i = m_Vertices.begin(); i != m_Vertices.end();) {
		CVertex& v = *i;
		++i;
		for (list<CVertex>::iterator i2 = i; i2 != m_Vertices.end(); ++i2) {
			if (v.m_Name == i2->m_Name) return false;
		}
	}
	cout << "Verificar no repeticion nombres aristas" << endl;
	for (list<CEdge>::iterator i = m_Edges.begin(); i != m_Edges.end();) {
		CEdge& e = *i;
		++i;
		for (list<CEdge>::iterator i2 = i; i2 != m_Edges.end(); ++i2) {
			if (e.m_Name == i2->m_Name) return false;
		}
	}
	// Verificar que las distancias de los edges son suficentemente diferentes
	cout << "Verificar diferencias de longitudes de las aristas" << endl;
	for (list<CEdge>::iterator i = m_Edges.begin(); i != m_Edges.end();) {
		CEdge& e = *i;
		++i;
		for (list<CEdge>::iterator i2 = i; i2 != m_Edges.end(); ++i2) {
			if (abs(e.m_Length - i2->m_Length) < 0.00001 && e.m_pReverseEdge!=&*(i2)) {
				cout << "Dif " << abs(e.m_Length - i2->m_Length) << endl;
				//return false;
			}
		}
	}

	return true;
}

// RandomCreation ==============================================================

void CGraph::RandomCreation(int nVertices, int nEdges)
{
	Clear();
	srand((unsigned)time(NULL));
	vector<CVertex*> vertices(nVertices);
	for (int i = 0; i < nVertices; ++i) {
		double x, y;
		bool encontrado;
		double minDist = 16000.0 * 16000.0 * 0.2 * 0.2;
		do {
			x = rand()* (16000.0 / RAND_MAX);
			y = rand() * (16000.0 / RAND_MAX);
			encontrado = false;
			for (int j = 0; j < i; ++j) {
				double dx = vertices[j]->m_Point.m_X - x;
				double dy = vertices[j]->m_Point.m_Y - y;
				if (dx * dx + dy * dy < minDist) {
					encontrado = true;
					break;
				}
			}
			minDist *= 0.75;
			if (minDist < 1) cout << "Distancia minima " << minDist << endl;
		} while (encontrado);
		vertices[i] = NewVertex(CGPoint(x, y));
		if (i != 0) {
			for (;;) {
				int j = rand() % i;
				double len = vertices[i]->m_Point.Distance(vertices[j]->m_Point);
				if (GreaterLengthDifferenceP(len, 0.01)) {
					NewEdge(vertices[j], vertices[i])->m_Length =len;
					--nEdges;
					break;
				}
			}
		}
		if (i % 100 == 0) cout << i << " vertices created" << endl;
	}
	int rep = 0;
	while (nEdges > 0) {
		int i = rand() % nVertices;
		int j = rand() % nVertices;
		if (i != j && !vertices[i]->NeighbordP(vertices[j])) {
			double len = vertices[i]->m_Point.Distance(vertices[j]->m_Point);
			if (GreaterLengthDifferenceP(len, 0.01)) {
				NewEdge(vertices[i], vertices[j])->m_Length = len;
				--nEdges;
				rep = 0;
			}
			else {
				++rep;
				if (rep > 100) throw exception("CGraph::CrearAleatorio: No se puede encontrar un nuevo edge");
			}
		}
		else {
			++rep;
			if (rep > 100) throw exception("CGraph::CrearAleatorio: No se puede encontrar un nuevo edge");
		}
		if (nEdges % 100 == 0) cout << nEdges << " edges left to create" << endl;
	}
	/*
	if (!Invariant()) {
		cout << "NO CUMPLE LA INVARIANTE" << endl;
	}
	*/
}

// CreateGrid ==============================================================

void CGraph::CreateGrid(int nRows, int nColumns)
{
	Clear();
	vector<CVertex*> row0(nColumns,NULL);
	vector<CVertex*> row(nColumns, NULL);

	for (int y=0; y<nRows; ++y) {
		for (int x = 0; x < nColumns; ++x) {
			char buf[100];
			sprintf_s(buf, "%d_%d", x, y);
			row[x]=NewVertex(buf,x*100, y*100);
			if (x > 0) {
				sprintf_s(buf, "%s:%s", row[x-1]->m_Name.c_str(), row[x]->m_Name.c_str());
				NewEdge(buf,0.0,row[x - 1], row[x]);
			}
			if (y > 0) {
				sprintf_s(buf, "%s:%s", row0[x]->m_Name.c_str(), row[x]->m_Name.c_str());
				NewEdge(buf,0.0,row0[x], row[x]);
			}
		}
		row0 = row;
	}
}


// GreaterLengthP ==============================================================

bool CGraph::GreaterLengthDifferenceP(double l,double minLength)
{
	for (CEdge& e : m_Edges) {
		if (abs(e.m_Length - l) < minLength) return false;
	}
	return true;
}

// ToPlannar ===================================================================

void CGraph::ToPlannar()
{
	bool cross;
	do {
		//cout << "Buscar cruces" << endl;
		cross = false;
		for (list<CEdge>::iterator it1 = m_Edges.begin(); it1 != m_Edges.end();) {
			CEdge& e1 = *it1;
			++it1;
			for (list<CEdge>::iterator it2 = it1; it2 != m_Edges.end();) {
				CEdge& e2 = *it2;
				++it2;
				// Si comparten vertices no hay cruce
				if (e1.m_pOrigin == e2.m_pOrigin || e1.m_pOrigin == e2.m_pDestination) continue;
				if (e1.m_pDestination == e2.m_pOrigin || e1.m_pDestination == e2.m_pDestination) continue;
				/*
				* a*(d1-o1)+o1==b*(d2-o2)+o2
				* a*(d1-o1)==b*(d2-o2)+o2-o1
				* a*(d1.x-o1.x)==b*(d2.x-o2.x)+o2.x-o1.x
				* a*(d1.y-o1.y)==b*(d2.y-o2.y)+o2.y-o1.y
				* (b*(d2.x-o2.x)+o2.x-o1.x)/(d1.x-o1.x)==(b*(d2.y-o2.y)+o2.y-o1.y)/(d1.y-o1.y)
				* (b*v2.x+o2.x-o1.x)/v1.x==(b*v2.y+o2.y-o1.y)/v1.y
				* b*v2.x*v1.y+(o2.x-o1.x)*v1.y==b*v2.y*v1.x+(o2.y-o1.y)*v1.x
				* b*(v2.x*v1.y-v2.y*v1.x)==(o2.y-o1.y)*v1.x-(o2.x-o1.x)*v1.y
				* b==((o2.y-o1.y)*v1.x-(o2.x-o1.x)*v1.y)/(v2.x*v1.y-v2.y*v1.x)
				* 
				* a*(d1.x-o1.x)==b*(d2.x-o2.x)+o2.x-o1.x
				* a*v1.x==b*v2.x+o2.x-o1.x
				* a==(b*v2.x+o2.x-o1.x)/v1.x
				*/
				CGPoint o1 = e1.m_pOrigin->m_Point;
				CGPoint d1 = e1.m_pDestination->m_Point;
				CGPoint o2 = e2.m_pOrigin->m_Point;
				CGPoint d2 = e2.m_pDestination->m_Point;
				CGPoint v1 = d1 - o1;
				CGPoint v2 = d2 - o2;
				double numerador = ((o2.m_Y - o1.m_Y) * v1.m_X - (o2.m_X - o1.m_X) * v1.m_Y);
				double denominador = (v2.m_X * v1.m_Y - v2.m_Y * v1.m_X);
				if (denominador == 0.0) continue;
				double b = numerador / denominador;
				if (b < 0 || b>1) continue;
				double a;
				if (v1.m_X != 0) a = (b * v2.m_X + o2.m_X - o1.m_X) / v1.m_X;
				else a = (b * v2.m_Y + o2.m_Y - o1.m_Y) / v1.m_Y;
				if (a < 0 || a>1) continue;
				// Hay intersección
				CGPoint inter(b * v2 + o2);
				cross = true;
				// Añadir el nuevo vertice
				CVertex *pInter=NewVertex(inter);
				//cout << e1 << e2 << *pInter << endl;
				NewEdge(e1.m_pOrigin, pInter);
				NewEdge(pInter,e1.m_pDestination);
				NewEdge(e2.m_pOrigin, pInter);
				NewEdge(pInter, e2.m_pDestination);

				// Elimnar los edges
				DeleteEdge(&e1);
				DeleteEdge(&e2);
				goto repetir;
			}
		}
	repetir:;
	} while (cross);
}

// =============================================================================
// CVisits =====================================================================
// =============================================================================
// Lista de vértices

// Delete ======================================================================

void CVisits::Delete(CVertex* pVertex)
{
	for (list<CVertex*>::iterator iter = m_Vertices.begin(); iter != m_Vertices.end();) {
		if (*iter==pVertex) {
			list<CVertex*>::iterator tmp = iter;
			++iter;
			m_Vertices.erase(tmp);
		}
		else ++iter;
	}
}

// MemberP =====================================================================

bool CVisits::MemberP(CVertex* pVertex)
{
	for (CVertex* pV : m_Vertices) 	if (pV == pVertex) return true;
	return false;
}

// Save =======================================================================

void CVisits::Save(const char* filename) {
	ofstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening visits %s",filename);
	}
	f << "VISITS 1.0" << endl;
	for (CVertex* pV : m_Vertices) {
		f << pV->m_Name << endl;
	}
	f.close();
}

// Load ========================================================================

void CVisits::Load(const char* filename) {
	Clear();
	ifstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening visits %s",filename);
	}
	try {
		char buf[256];
		f.getline(buf, 256);
		if (strcmp(buf, "VISITS 1.0") != 0) {
			throw MyException("CVisits::Read: el fichero no tiene formato de fichero de visits: %s", filename);
		}
		// leer vértices
		while (!f.eof()) {
			f.getline(buf, 256);
			if (buf[0] == '\0') break;
			CVertex *pVertex=m_pGraph->FindVertex(buf);
			if (!pVertex) throw MyException("Vertex not found in graph: %s", buf);
			m_Vertices.push_back(pVertex);
		}
		f.close();
	}
	catch (exception) {
		f.close();
		Clear();
		throw;
	}
}

// RandomCreation ==============================================================

void CVisits::RandomCreation(int nVisits, bool ciclo)
{
	Clear();
	srand((unsigned)time(NULL));
	size_t n = m_pGraph->GetNVertices();
	if (n == 0) throw exception("CVisits::CrearAleatorio no puede crear visitas si no hay grafo");

	for (int i = 0; i < nVisits; ++i) {
		CVertex* pVertex;
		int j = 0;
		do {
			int index = rand() % n;
			pVertex = m_pGraph->GetVertex(index);
			++j;
			if (j > 1000) throw exception("CVisits::CrearAleatorio no encuentra un vértice diferente a los ya añadidos");
		} while (MemberP(pVertex));
		Add(pVertex);
	}
	if (ciclo) Add(m_Vertices.front());
}

// << ==========================================================================

ostream& operator<< (ostream& s, const CVisits& visits)
{
	s << "{";
	for (list<CVertex*>::const_iterator iter = visits.m_Vertices.cbegin(); iter != visits.m_Vertices.cend();) {
		const CVertex* pV = *iter;
		s << pV->m_Name;
		++iter;
		if (iter != visits.m_Vertices.cend()) {
			s << ",";
		}
	}
	s << "}";
	return s;
}

// =============================================================================
// CTrack ======================================================================
// =============================================================================
// Lista de aristas que forman un camino

// Delete ======================================================================

void CTrack::Delete(CVertex* pVertex)
{
	for (list<CEdge*>::iterator iter = m_Edges.begin(); iter != m_Edges.end();) {
		if ((*iter)->m_pOrigin == pVertex || (*iter)->m_pDestination == pVertex) {
			list<CEdge*>::iterator tmp = iter;
			++iter;
			m_Edges.erase(tmp);
		}
		else ++iter;
	}
}

// Delete ======================================================================

void CTrack::Delete(CEdge* pEdge)
{
	for (list<CEdge*>::iterator iter = m_Edges.begin(); iter != m_Edges.end();) {
		if (*iter == pEdge) {
			list<CEdge*>::iterator tmp = iter;
			++iter;
			m_Edges.erase(tmp);
		}
		else ++iter;
	}
}


// Append ======================================================================

void CTrack::Append(CTrack& t)
{
	for (list<CEdge*>::const_iterator iter = t.m_Edges.cbegin(); iter != t.m_Edges.cend(); ++iter) {
		m_Edges.push_back(*iter);
	}
}

// AppendBefore ================================================================

void CTrack::AppendBefore(CTrack& t)
{
	for (list<CEdge*>::const_reverse_iterator iter = t.m_Edges.crbegin(); iter != t.m_Edges.crend(); ++iter) {
		m_Edges.push_front(*iter);
	}
}

// Save =======================================================================

void CTrack::Save(const char* filename)
{
	ofstream  f(filename);
	if (!f.good()) {
		char msg[256];
		sprintf_s(msg, "Error opening track %s", filename);
		throw exception(msg);
	}
	f << "TRACK 1.0" << endl;
	for (list<CEdge*>::const_iterator iter = m_Edges.cbegin(); iter != m_Edges.cend(); ++iter) {
		const CEdge*pEdge = *iter;
		f << pEdge->m_Name << endl;
	}
	f.close();
}

// Load ========================================================================

void CTrack::Load(const char* filename)
{

	Clear();
	ifstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening track %s", filename);
	}
	try {
		char buf[256];
		f.getline(buf, 256);
		if (strcmp(buf, "TRACK 1.0") != 0) {
			throw MyException("CTrack::Read: el fichero no tiene formato de fichero de track: %s", filename);
		}
		// leer aristas
		while (!f.eof()) {
			f.getline(buf, 256);
			if (buf[0] == '\0') break;
			char edgeName[256];
			sscanf_s(buf, "%s", edgeName,256);
			CEdge* pEdge= m_pGraph->FindEdge(edgeName);
			AddLast(pEdge);
		}
		f.close();
	}
	catch (exception) {
		f.close();
		Clear();
		throw;
	}

}

// Length ======================================================================

double CTrack::Length() {
	double l = 0.0;
	for (CEdge* pE : m_Edges) {
		l += pE->m_Length;
	}
	return l;
}

// MemberP =====================================================================

bool CTrack::MemberP(CEdge* pE) {
	for (CEdge* pE2 : m_Edges) {
		if (pE2 == pE) return true;
	}
	return false;
}

// MemberP =====================================================================

bool CTrack::MemberP(CVertex* pV) {
	for (CEdge* pE : m_Edges) {
		if (pE->m_pOrigin == pV || pE->m_pDestination == pV) return true;
	}
	return false;
}


// << ==========================================================================

ostream& operator<< (ostream& s, const CTrack& track)
{
	s << "[";
	if (!track.m_Edges.empty()) {
		for (CEdge* pE : track.m_Edges) {
			s << pE->m_pOrigin->m_Name << " -(" << pE->m_Name << ")->";
		}
		s << track.m_Edges.back()->m_pDestination->m_Name;
	}
	s << "]";
	return s;
}

// =============================================================================
// CSpanningTree ===============================================================
// =============================================================================
// Lista de aristas que forman un camino

// Delete ======================================================================

void CSpanningTree::Delete(CVertex* pVertex)
{
	for (list<CEdge*>::iterator iter = m_Edges.begin(); iter != m_Edges.end();) {
		if ((*iter)->m_pOrigin==pVertex || (*iter)->m_pDestination == pVertex) {
			list<CEdge*>::iterator tmp = iter;
			++iter;
			m_Edges.erase(tmp);
		}
		else ++iter;
	}
}

// Delete ======================================================================

void CSpanningTree::Delete(CEdge* pEdge)
{
	for (list<CEdge*>::iterator iter = m_Edges.begin(); iter != m_Edges.end();) {
		if (*iter == pEdge) {
			list<CEdge*>::iterator tmp = iter;
			++iter;
			m_Edges.erase(tmp);
		}
		else ++iter;
	}
}


// Save =======================================================================

void CSpanningTree::Save(const char* filename)
{
	ofstream  f(filename);
	if (!f.good()) {
		char msg[256];
		sprintf_s(msg, "Error opening track %s", filename);
		throw exception(msg);
	}
	f << "SPANNNING TREE 1.0" << endl;
	for (list<CEdge*>::const_iterator iter = m_Edges.cbegin(); iter != m_Edges.cend(); ++iter) {
		const CEdge* pEdge = *iter;
		f << pEdge->m_Name << endl;
	}
	f.close();
}

// Load ========================================================================

void CSpanningTree::Load(const char* filename)
{

	Clear();
	ifstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening track %s", filename);
	}
	try {
		char buf[256];
		f.getline(buf, 256);
		if (strcmp(buf, "SPANNNING TREE 1.0") != 0) {
			throw MyException("CTrack::Read: el fichero no tiene formato de fichero de track: %s", filename);
		}
		// leer vértices
		while (!f.eof()) {
			f.getline(buf, 256);
			if (buf[0] == '\0') break;
			char edgeName[256];
			sscanf_s(buf, "%s", edgeName, 256);
			CEdge* pEdge = m_pGraph->FindEdge(edgeName);
			Add(pEdge);
		}
		f.close();
	}
	catch (exception) {
		f.close();
		Clear();
		throw;
	}

}

// Length ======================================================================

double CSpanningTree::Length() {
	double l = 0.0;
	for (CEdge* pE : m_Edges) {
		l += pE->m_Length;
	}
	return l;
}

// MemberP =====================================================================

bool CSpanningTree::MemberP(CEdge* pE) {
	for (CEdge* pE2 : m_Edges) {
		if (pE2 == pE) return true;
	}
	return false;
}

// MemberP =====================================================================

bool CSpanningTree::MemberP(CVertex* pV) {
	for (CEdge* pE : m_Edges) {
		if (pE->m_pOrigin == pV || pE->m_pDestination == pV) return true;
	}
	return false;
}

// << ==========================================================================

ostream& operator<< (ostream& s, const CSpanningTree& tree)
{
	s << "{";
	if (!tree.m_Edges.empty()) {
		bool first = true;
		for (CEdge* pE : tree.m_Edges) {
			if (first) first = false;
			else s << ", ";
			s << pE->m_pOrigin->m_Name << "--(" << pE->m_Name << ")-->" << pE->m_pDestination->m_Name;
		}
	}
	s << "}";
	return s;
}

// =============================================================================
// CConvexHull =================================================================
// =============================================================================
// Lista de vértices

// Delete ======================================================================

void CConvexHull::Delete(CVertex* pVertex)
{
	for (list<CVertex*>::iterator iter = m_Vertices.begin(); iter != m_Vertices.end();) {
		if (*iter == pVertex) {
			list<CVertex*>::iterator tmp = iter;
			++iter;
			m_Vertices.erase(tmp);
		}
		else ++iter;
	}
}

// MemberP =====================================================================

bool CConvexHull::MemberP(CVertex* pVertex)
{
	for (CVertex* pV : m_Vertices) 	if (pV == pVertex) return true;
	return false;
}

// Save =======================================================================

void CConvexHull::Save(const char* filename) {
	ofstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening convex hull %s", filename);
	}
	f << "CONVEX_HULL 1.0" << endl;
	for (CVertex* pV : m_Vertices) {
		f << pV->m_Name << endl;
	}
	f.close();
}

// Load ========================================================================

void CConvexHull::Load(const char* filename) {
	Clear();
	ifstream  f(filename);
	if (!f.good()) {
		throw MyException("Error opening convex hull %s", filename);
	}
	try {
		char buf[256];
		f.getline(buf, 256);
		if (strcmp(buf, "CONVEX_HULL 1.0") != 0) {
			throw MyException("CConvexHull::Read: el fichero no tiene formato de fichero de convex hull: %s", filename);
		}
		// leer vértices
		while (!f.eof()) {
			f.getline(buf, 256);
			if (buf[0] == '\0') break;
			CVertex* pVertex = m_pGraph->FindVertex(buf);
			if (!pVertex) throw MyException("Vertex not found in graph: %s", buf);
			m_Vertices.push_back(pVertex);
		}
		f.close();
	}
	catch (exception) {
		f.close();
		Clear();
		throw;
	}
}

// << ==========================================================================

ostream& operator<< (ostream& s, const CConvexHull& ch)
{
	s << "[";
	for (list<CVertex*>::const_iterator iter = ch.m_Vertices.cbegin(); iter != ch.m_Vertices.cend();) {
		const CVertex* pV = *iter;
		s << pV->m_Name;
		++iter;
		if (iter != ch.m_Vertices.cend()) {
			s << ",";
		}
	}
	s << "]";
	return s;
}
