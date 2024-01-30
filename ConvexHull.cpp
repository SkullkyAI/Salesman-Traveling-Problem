#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>



// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================

// QuickHull ===================================================================

double CrossProduct(CGPoint& a, CGPoint& b, CGPoint& c) {
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}

double TriangleArea(CGPoint& a, CGPoint& b, CGPoint c) {
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}

list<CVertex*> QuickHullRec(CVertex* pointLeft, CVertex* pointRight, list<CVertex*> points)
{
	if (points.empty())
	{
		return { pointLeft, pointRight };
	}

	double maxArea = 0;
	CVertex* maxAreaPoint = nullptr;
	for (const auto point : points)
	{
		const auto area = TriangleArea(pointLeft->m_Point, pointRight->m_Point, point->m_Point);
		if (area > maxArea)
		{
			maxArea = area;
			maxAreaPoint = point;
		}
	}

	std::list<CVertex*> leftPoints;
	std::list<CVertex*> rightPoints;

	for (auto vertex : points)
	{
		if (vertex == maxAreaPoint || vertex == pointLeft || vertex == pointRight)
		{
			continue;
		}

		if (CrossProduct(pointLeft->m_Point, maxAreaPoint->m_Point, vertex->m_Point) > 0)
		{
			leftPoints.push_back(vertex);
		}
		else if (CrossProduct(maxAreaPoint->m_Point, pointRight->m_Point, vertex->m_Point) > 0)
		{
			rightPoints.push_back(vertex);
		}
	}

	auto leftHull = QuickHullRec(pointLeft, maxAreaPoint, leftPoints);
	auto rightHull = QuickHullRec(maxAreaPoint, pointRight, rightPoints);

	leftHull.remove(maxAreaPoint);
	leftHull.splice(leftHull.end(), rightHull);

	return leftHull;
}

CConvexHull QuickHull(CGraph& graph)
{

	if(graph.m_Vertices.empty())
	{
		return {&graph};
	}

	CVertex* leftPoint = &graph.m_Vertices.front();
	CVertex* rightPoint = &graph.m_Vertices.back();

	for (CVertex& vertex : graph.m_Vertices)
	{
		if (leftPoint->m_Point.m_X > vertex.m_Point.m_X)
		{
			leftPoint = &vertex;
		}
		else if (rightPoint->m_Point.m_X < vertex.m_Point.m_X)
		{
			rightPoint = &vertex;
		}
	}

	if(leftPoint->m_Point.m_X == rightPoint->m_Point.m_X &&
		leftPoint->m_Point.m_Y == rightPoint->m_Point.m_Y)
	{
		auto resultHull = CConvexHull(&graph);
		resultHull.m_Vertices.push_back(&graph.m_Vertices.front());
		return resultHull;
	}

	std::list<CVertex*> upperPoints;
	std::list<CVertex*> lowerPoints;

	for(CVertex& vertex : graph.m_Vertices)
	{
		const auto crossProductResult = CrossProduct(leftPoint->m_Point, rightPoint->m_Point, vertex.m_Point);

		if (crossProductResult > 0)
		{
			upperPoints.push_back(&vertex);
		}
		else if (crossProductResult < 0)
		{
			lowerPoints.push_back(&vertex);
		}
	}

	auto upperHull = QuickHullRec(leftPoint, rightPoint, upperPoints);
	auto lowerHull = QuickHullRec(rightPoint, leftPoint, lowerPoints);

	upperHull.remove(rightPoint);
	upperHull.remove(leftPoint);
	upperHull.splice(upperHull.end(), lowerHull);
	auto resultHull = CConvexHull(&graph);
	resultHull.m_Vertices = upperHull;

	return resultHull;
}
