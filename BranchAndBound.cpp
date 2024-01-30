#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <numeric>
#include <tuple>

class Node {
public:
	vector<int> mIndexes;
	vector<bool> mVisited;
	double mLength;
	double mInferiorHeight;
	double mSuperiorHeight;

	Node(double mmDijkstraDistance, vector<int> indexes, int nVertex)
		: mIndexes(indexes)
		, mLength(mmDijkstraDistance)
	    , mInferiorHeight(0)
		, mSuperiorHeight(0)
	{
		mVisited.resize(nVertex, false);
		mVisited[0] = true;
	}

	Node(double mmDijkstraDistance, vector<int> indexes, int nVertex, double inferiorHeight, double cotaSuperior)
		: mIndexes(indexes)
	    , mLength(mmDijkstraDistance)
	    , mInferiorHeight(inferiorHeight)
		, mSuperiorHeight(cotaSuperior)
	{
		mVisited.resize(nVertex, false);
		mVisited[0] = true;
	}

	Node(const Node& node)
		: mLength(0)
		, mInferiorHeight(0)
		, mSuperiorHeight(0)
	{
		mIndexes = node.mIndexes;
		mVisited = node.mVisited;
	}
};


struct NodeLenghtComparator {
	bool operator()(const Node* s1, const Node* s2) {
		return s1->mLength > s2->mLength;
	}
};

struct NodeHeightComparator {
	bool operator()(const Node* s1, const Node* s2) {
		return s1->mInferiorHeight > s2->mInferiorHeight;
	}
};

class Cell {
public:
	double mDijkstraDistance;
	list<CEdge*> mPath;
};

using Matrix = vector<vector<Cell>>;
using PriorityQueueByLength = priority_queue<Node*, std::vector<Node*>, NodeLenghtComparator>;
using PriorityQueueByHeight = priority_queue<Node*, std::vector<Node*>, NodeHeightComparator>;

Matrix BuildDistanceMatrix(CGraph& graph, CVisits& visits)
{
	Matrix matrix = Matrix(visits.m_Vertices.size() - 1);

	int index = 0;
	for (auto iOriginVisits = visits.m_Vertices.begin(); iOriginVisits != next(visits.m_Vertices.begin(), visits.m_Vertices.size() - 1); ++iOriginVisits)
	{
		DijkstraQueue(graph, *iOriginVisits);

		(*iOriginVisits)->m_indexMatrix = index;

		for (const CVertex* jDestinationVisits : visits.m_Vertices) {

			Cell matrixCell;
			matrixCell.mDijkstraDistance = jDestinationVisits->m_DijkstraDistance;

			if (*iOriginVisits != jDestinationVisits)
			{
				while (jDestinationVisits != *iOriginVisits)
				{
					matrixCell.mPath.push_front(jDestinationVisits->m_pDijkstraPrevious);
					jDestinationVisits = jDestinationVisits->m_pDijkstraPrevious->m_pOrigin;
				}
			}

			matrix[index].push_back(matrixCell);

		}
		index++;
	}

	visits.m_Vertices.back()->m_indexMatrix = index;
	return matrix;
}

void LengthBound(Node* topNode, const CVisits& visits, const Matrix& matrix, PriorityQueueByLength& priorityQueue)
{
	if (topNode->mIndexes.back() == visits.m_Vertices.back()->m_indexMatrix)
	{
		return;
	}

	for (int col = 0; col < visits.m_Vertices.size(); col++)
	{
		if (col != topNode->mIndexes.back() && !topNode->mVisited[col])
		{
			Node* newNode = new Node(*topNode);
			newNode->mIndexes.push_back(col);
			newNode->mLength = topNode->mLength + matrix[topNode->mIndexes.back()][col].mDijkstraDistance;
			priorityQueue.push(newNode);
		}
	}
}

CTrack BuildSolution(CGraph& graph, const Node* topNode, const Matrix& matrix)
{
	CTrack solution(&graph, matrix[topNode->mIndexes[0]][topNode->mIndexes[1]].mPath);

	for (int element = 1; element < topNode->mIndexes.size() - 1; element++)
	{
		for (CEdge* iEdge : matrix[topNode->mIndexes[element]][topNode->mIndexes[element + 1]].mPath)
		{
			solution.m_Edges.push_back(iEdge);
		}
	}

	return solution;
}

CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits)
{
	const auto matrix = BuildDistanceMatrix(graph, visits);

	PriorityQueueByLength priorityQueue;
	for (int col = 1; col < visits.m_Vertices.size(); col++) {

		vector<int> pathIndexes;
		pathIndexes.push_back(0);
		pathIndexes.push_back(col);
		priorityQueue.push(new Node(matrix[0][col].mDijkstraDistance, pathIndexes, visits.m_Vertices.size()));
	}

	while (!priorityQueue.empty())
	{
		Node* topNode = priorityQueue.top();
		priorityQueue.pop();

		topNode->mVisited[topNode->mIndexes.back()] = true;

		if (topNode->mIndexes.back() != visits.m_Vertices.back()->m_indexMatrix)
		{
			LengthBound(topNode, visits, matrix, priorityQueue);
			continue;
		}

		bool bound = false;
		for (const bool visitVertex : topNode->mVisited)
		{
			if (!visitVertex)
			{
				bound = true;
				break;
			}
		}

		if (bound)
		{
			LengthBound(topNode, visits, matrix, priorityQueue);
			continue;
		}

		auto solution = BuildSolution(graph, topNode, matrix);
		delete topNode;
		return solution;
	}

	return {&graph};

}

void BuildHeights(CVisits& visits, Matrix matrix, vector<double>& minimum, vector<double>& maximum)
{
	minimum.resize(visits.m_Vertices.size(), numeric_limits<double>::max());
	maximum.resize(visits.m_Vertices.size(), 0.0);

	for (int row = 0; row < visits.m_Vertices.size() - 1; row++)
	{
		for (int col = 0; col < visits.m_Vertices.size(); col++)
		{
			if (row == col)
			{
				continue;
			}

			if (matrix[row][col].mDijkstraDistance < minimum[col])
			{
				minimum[col] = matrix[row][col].mDijkstraDistance;
			}

			if (matrix[row][col].mDijkstraDistance > maximum[col])
			{
				maximum[col] = matrix[row][col].mDijkstraDistance;
			}
		}
	}
}

void HeightBound(Node* topNode, const CVisits& visits, const Matrix& matrix,
				 PriorityQueueByHeight& priorityQueue, double& globalSuperiorHeight,
				 double& superiorHeight, double& inferiorHeight,
				 const vector<double>& minimum, const vector<double>& maximum)
{
	if (topNode->mIndexes.back() == visits.m_Vertices.back()->m_indexMatrix)
	{
		return;
	}

	for (int col = 0; col < visits.m_Vertices.size(); col++)
	{
		if (col == topNode->mIndexes.back() || topNode->mVisited[col] || col == visits.m_Vertices.size() - 1 && topNode->mIndexes.size() == 1)
		{
			continue;
		}

		inferiorHeight = topNode->mInferiorHeight - minimum[col] + matrix[topNode->mIndexes.back()][col].mDijkstraDistance;

		if (globalSuperiorHeight <= inferiorHeight)
		{
			continue;
		}

		superiorHeight = topNode->mSuperiorHeight - maximum[col] + matrix[topNode->mIndexes.back()][col].mDijkstraDistance + pow(10, -6);

		if (globalSuperiorHeight > superiorHeight)
		{
			globalSuperiorHeight = superiorHeight;
		}

		Node* newNode = new Node(*topNode);
		newNode->mInferiorHeight = inferiorHeight;
		newNode->mSuperiorHeight = superiorHeight;

		newNode->mIndexes.push_back(col);


		newNode->mLength = topNode->mLength + matrix[topNode->mIndexes.back()][col].mDijkstraDistance;

		priorityQueue.push(newNode);
	}
}

CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits& visits)
{
	const auto matrix = BuildDistanceMatrix(graph, visits);

	if (visits.m_Vertices.size() == 2)
	{
		return { &graph, matrix[0][1].mPath };
	}

	vector <double> minimum, maximum;
	BuildHeights(visits, matrix, minimum, maximum);

	double inferiorHeight = std::accumulate(minimum.begin(), minimum.end(), 0.0);
	double superiorHeight = std::accumulate(maximum.begin(), maximum.end(), 0.0) + pow(10, -6);
	double globalSuperiorHeight = superiorHeight;

	PriorityQueueByHeight priorityQueue;
	priorityQueue.push(new Node(0, { 0 }, visits.m_Vertices.size(), inferiorHeight, superiorHeight));

	while (!priorityQueue.empty())
	{
		Node* topNode = priorityQueue.top();
		priorityQueue.pop();

		topNode->mVisited[topNode->mIndexes.back()] = true;

		if (topNode->mIndexes.back() != visits.m_Vertices.back()->m_indexMatrix)
		{
			HeightBound(topNode, visits, matrix, priorityQueue, globalSuperiorHeight, superiorHeight, inferiorHeight, minimum, maximum);
			continue;
		}

		bool bound = false;
		for (const bool visitVertex : topNode->mVisited)
		{
			if (!visitVertex)
			{
				HeightBound(topNode, visits, matrix, priorityQueue, globalSuperiorHeight, superiorHeight, inferiorHeight, minimum, maximum);
				bound = true;
				break;
			}
		}

		if(bound)
		{
			continue;
		}

		auto solution = BuildSolution(graph, topNode, matrix);
		delete topNode;
		return solution;
	}

	return { &graph };
}



// SalesmanTrackBranchAndBound3 ===================================================


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits& visits)
{
	return SalesmanTrackBranchAndBound2(graph, visits);

	const auto matrix = BuildDistanceMatrix(graph, visits);

	if (visits.m_Vertices.size() == 2)
	{
		return { &graph, matrix[0][1].mPath };
	}

	vector <double> minimum, maximum;
	BuildHeights(visits, matrix, minimum, maximum);

	double inferiorHeight = std::accumulate(minimum.begin(), minimum.end(), 0.0);
	double superiorHeight = std::accumulate(maximum.begin(), maximum.end(), 0.0) + pow(10, -6);
	double globalSuperiorHeight = superiorHeight;

	PriorityQueueByHeight priorityQueue;
	priorityQueue.push(new Node(0, { 0 }, visits.m_Vertices.size(), inferiorHeight, superiorHeight));


	while (!priorityQueue.empty())
	{
		Node* topNode = priorityQueue.top();
		priorityQueue.pop();

		topNode->mVisited[topNode->mIndexes.back()] = true;

		if (topNode->mIndexes.back() == visits.m_Vertices.back()->m_indexMatrix)
		{
			const auto ip = std::unique(topNode->mIndexes.begin(), topNode->mIndexes.begin() + topNode->mIndexes.size());

			topNode->mIndexes.resize(distance(topNode->mIndexes.begin(), ip));

			if (topNode->mIndexes.size() != visits.m_Vertices.size())
			{
				continue;
			}

			auto solution = BuildSolution(graph, topNode, matrix);
			delete topNode;
			return solution;
		}

		for (int row = 0; row < visits.m_Vertices.size() - 1; row++)
		{
			if (topNode->mVisited[row]) 
			{
				continue;
			}

			for (int col = 0; col < visits.m_Vertices.size(); col++)
			{
				if (row == col || topNode->mVisited[col]) 
				{
					continue;
				}

				if (matrix[row][col].mDijkstraDistance < minimum[col])
				{
					minimum[col] = matrix[row][col].mDijkstraDistance;
				}

				if (matrix[row][col].mDijkstraDistance > maximum[col])
				{
					maximum[col] = matrix[row][col].mDijkstraDistance;
				}
			}
		}

		for (int col = 0; col < visits.m_Vertices.size() - 1; col++) 
		{
			if (topNode->mIndexes.back() == col || topNode->mVisited[col])
			{
				continue;
			}

			if (matrix[topNode->mIndexes.back()][col].mDijkstraDistance < minimum[col])
			{
				minimum[col] = matrix[topNode->mIndexes.back()][col].mDijkstraDistance;
			}

			if (matrix[topNode->mIndexes.back()][col].mDijkstraDistance > maximum[col])
			{
				maximum[col] = matrix[topNode->mIndexes.back()][col].mDijkstraDistance;
			}
		}

		for (int col = 0; col < visits.m_Vertices.size(); col++)
		{
			if (col == topNode->mIndexes.back() || topNode->mVisited[col] || col == visits.m_Vertices.size() - 1 && topNode->mIndexes.size() == 1)
			{
				continue;
			}

			inferiorHeight = topNode->mInferiorHeight - minimum[col] + matrix[topNode->mIndexes.back()][col].mDijkstraDistance;

			if (globalSuperiorHeight <= inferiorHeight)
			{
				continue;
			}

			superiorHeight = topNode->mSuperiorHeight - maximum[col] + matrix[topNode->mIndexes.back()][col].mDijkstraDistance + pow(10, -6);

			if (globalSuperiorHeight > superiorHeight)
			{
				globalSuperiorHeight = superiorHeight;
			}

			Node* newNode = new Node(*topNode);
			newNode->mInferiorHeight = inferiorHeight;
			newNode->mSuperiorHeight = superiorHeight;

			newNode->mIndexes.push_back(col);


			newNode->mLength = topNode->mLength + matrix[topNode->mIndexes.back()][col].mDijkstraDistance;

			priorityQueue.push(newNode);
		}
	}

	return CTrack(&graph);
}
