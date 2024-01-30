#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <random>
#include <chrono>

// SalesmanTrackProbabilistic ==================================================

struct Cell
{
	double mDijkstraDistance;
	list<CEdge*> mSection;
};

using Matrix = vector<vector<Cell>>;

CTrack SalesmanTrackProbabilistic(CGraph& graph, CVisits& visits)
{
	if (visits.m_Vertices.empty())
	{
		return { &graph };
	}

	int accumulator = 0;
	Matrix matrix(visits.m_Vertices.size() - 1);
	for (auto iOrigin = visits.m_Vertices.begin(); iOrigin != next(visits.m_Vertices.begin(), visits.m_Vertices.size() - 1); ++iOrigin)
	{
		DijkstraQueue(graph, *iOrigin);
		(*iOrigin)->m_indexMatrix = accumulator;

		for (const CVertex* iDestination : visits.m_Vertices)
		{

			Cell cell;
			cell.mDijkstraDistance = iDestination->m_DijkstraDistance;

			if (*iOrigin != iDestination)
			{
				while (iDestination != *iOrigin)
				{
					cell.mSection.push_front(iDestination->m_pDijkstraPrevious);
					iDestination = iDestination->m_pDijkstraPrevious->m_pOrigin;
				}
			}

			matrix[accumulator].push_back(cell);

		}
		accumulator++;
	}
	visits.m_Vertices.back()->m_indexMatrix = accumulator;

	deque<int> range;
	for (const CVertex* iVertex : visits.m_Vertices)
	{
		range.push_back(iVertex->m_indexMatrix);
	}

	range.pop_front();
	range.pop_back();

	deque rangeCopy(range);
	double firstLength = 0;
	vector<int> firstSolution;
	firstSolution.push_back(0);
	while (!rangeCopy.empty())
	{
		accumulator = rand() % rangeCopy.size();
		int newVertex = rangeCopy[accumulator];

		rangeCopy.erase(next(rangeCopy.begin(), accumulator));

		firstLength += matrix[firstSolution.back()][newVertex].mDijkstraDistance;
		firstSolution.push_back(newVertex);
	}

	firstLength += matrix[firstSolution.back()][visits.m_Vertices.back()->m_indexMatrix].mDijkstraDistance;
	firstSolution.push_back(visits.m_Vertices.back()->m_indexMatrix);

	vector<int> gradientSolution;
	double gradientLength = std::numeric_limits<double>::max();
	static constexpr int gradientIteration = 10000;
	for (int i = 0; i < gradientIteration * visits.m_Vertices.size(); i++)
	{
		rangeCopy = range;
		bool stopGradient = false;
		int newVertex;
		double currentLength = 0;

		vector<int> currentSolution;
		currentSolution.push_back(0);

		while (!rangeCopy.empty() && currentLength < gradientLength && !stopGradient)
		{
			if (currentLength > gradientLength)
			{
				stopGradient = true;
				break;
			}

			const int gradientIndex = rand() % rangeCopy.size();
			newVertex = rangeCopy[gradientIndex];

			rangeCopy.erase(next(rangeCopy.begin(), gradientIndex));

			currentLength += matrix[currentSolution.back()][newVertex].mDijkstraDistance;
			currentSolution.push_back(newVertex);
		}

		if (stopGradient)
		{
			continue;
		}

		currentLength += matrix[currentSolution.back()][visits.m_Vertices.back()->m_indexMatrix].mDijkstraDistance;
		currentSolution.push_back(visits.m_Vertices.back()->m_indexMatrix);

		if (currentLength < gradientLength)
		{
			gradientLength = currentLength;
			gradientSolution = currentSolution;
		}
	}

	if (gradientLength < firstLength)
	{
		auto solution = CTrack(&graph, matrix[gradientSolution[0]][gradientSolution[1]].mSection);

		for (int element = 1; element < gradientSolution.size() - 1; element++)
		{
			for (CEdge* iEdge : matrix[gradientSolution[element]][gradientSolution[element + 1]].mSection)
			{
				solution.m_Edges.push_back(iEdge);
			}
		}

		return solution;
	}

	auto solution = CTrack(&graph, matrix[firstSolution[0]][firstSolution[1]].mSection);
	for (int element = 1; element < firstSolution.size() - 1; element++) {

		for (CEdge* iEdge : matrix[firstSolution[element]][firstSolution[element + 1]].mSection)
		{
			solution.m_Edges.push_back(iEdge);
		}
	}

	return solution;
}
