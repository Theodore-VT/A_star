#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

#define POSITIVE_INFINITY 1000000000

class Node;

typedef std::vector<std::vector<Node>> Map;

class Node
{

public:

	Node(int X, int Y, bool IsWall, bool AllowDiag);

	void CalcHeuristic(Node EndNode);
	void GetNeighbours(Map map);
	
	inline int X(){return X_;};
	inline int Y(){return Y_;};
	inline int T(){return T_;};
	inline int H(){return H_;};
	inline int M(){return M_;};
	inline bool IsWall(){return IsWall;};

	struct Comparison
    {
        bool operator ()(Node &a, Node &b ) const
        { 
            return a.T() < b.T();
        }
    };

	std::vector<Node> Neighbours;
private:
	int X_, Y_;
	int T_, H_, M_;

	bool AllowDiags_, IsWall_;

	Node * CameFrom;

	Node * CameFrom;
};

Node::Node(int X, int Y, bool IsWall, bool AllowDiags):
X_(X),
Y_(Y),

IsWall_(IsWall),
AllowDiags_(AllowDiags),

T_(POSITIVE_INFINITY),
H_(POSITIVE_INFINITY),
M_(POSITIVE_INFINITY),

CameFrom(NULL)
{

}


//Make a guess about the distance from a node to the end
//we can under estimate it without a problem, but we can't under estimate it

void Node::CalcHeuristic(Node EndNode)
{
	if(AllowDiags_)
		H_ = std::sqrt(std::pow(X_ - EndNode.X(), 2.0) + (std::pow(Y_ - EndNode.Y(), 2.0))); 
	else
		H_ = std::abs(X_ - EndNode.X()) + std::abs(Y_ - EndNode.Y());
}

void Node::GetNeighbours(Map map)
{
	
}

bool operator == (Node &a, Node &b)
{
	return (a.X() == b.X()) && (a.Y() == b.Y());
}



//Return the index of the best node in an array of Node (the one with the lowest T_)
int GetLowestScoreNode(std::vector<Node> Vec)
{
	int BestScore = POSITIVE_INFINITY, Index;

	for(int i = 0; i < Vec.size(); ++i)
	{
		if(Vec[i].T() < BestScore)
		{
			BestScore = Vec[i].T();
			Index = i;
		}
	}
	return Index;
}

bool IsInVector(std::vector<Node> Vec, Node Target)
{
	return((std::find(Vec.begin(), Vec.end(), Target)) != Vec.end());
}

void A_star(Node Begin, Node End)
{
	std::vector<Node> ClosedSet;
	std::vector<Node> OpenSet;
	OpenSet.push_back(Begin);

	Node CurrentNode = Begin;

	Map map;

	while(OpenSet.size() > 0)
	{
		int BestNodeIndex = GetLowestScoreNode(OpenSet);
		CurrentNode = OpenSet[BestNodeIndex];

		if(CurrentNode == End)
			return;

		OpenSet.erase(OpenSet.begin() + BestNodeIndex);
		ClosedSet.push_back(CurrentNode);
		
		CurrentNode.GetNeighbours(map);

		
		for(int i = 0; i < CurrentNode.Neighbours.size(); ++i)
		{
			if(IsInVector(ClosedSet, CurrentNode.Neighbours[i]))
				continue;
			if(!IsInVector(OpenSet, CurrentNode.Neighbours[i]))
				OpenSet.push_back(CurrentNode.Neighbours[i]);
		}
	}
}
