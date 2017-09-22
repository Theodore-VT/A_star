#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

#define SUCCES_ 1;
#define FAILURE_ 0;

#define POSITIVE_INFINITY 1000000000

class Node;

typedef std::vector<std::vector<Node>> Map;

class Node
{

public:

	Node(int X, int Y, bool IsWall, bool AllowDiag);


	void GetNeighbours(Map map);
	inline std::vector<Node> Neighbours(){return Neighbours_;};
	
	inline int X(){return X_;};
	inline int Y(){return Y_;};
	

	inline void CalcF_Score(){F_Score_ = G_Score_ + H_Score_;};
	inline int F_Score(){return F_Score_;};

	int CalcHeuristic(Node EndNode);				//may also return value to simplifie main code
	inline int H_Score(){return H_Score_;};
	
	inline void SetG_Score(int G_Score){G_Score_ = G_Score;};
	inline int G_Score(){return G_Score_;};

	inline void SetIsWall(bool IsWall){IsWall_ = IsWall;};
	inline bool IsWall(){return IsWall_;};

	inline void SetCameFrom(Node *CameFrom){CameFrom_ = CameFrom;};
	inline Node * GetCameFrom(){return CameFrom_;};

private:
	int X_, Y_;
	int F_Score_, H_Score_, G_Score_;

	bool AllowDiags_, IsWall_;

	std::vector<Node> Neighbours_;
	Node * CameFrom_;
};

Node::Node(int X = 0, int Y = 0, bool IsWall = false, bool AllowDiags = true):
X_(X),
Y_(Y),

IsWall_(IsWall),
AllowDiags_(AllowDiags),

F_Score_(POSITIVE_INFINITY),
H_Score_(POSITIVE_INFINITY),
G_Score_(POSITIVE_INFINITY),

CameFrom_(NULL)
{

}


//Make a guess about the distance from a node to the end
//we can under estimate it without a problem, but we can't under estimate it

int Node::CalcHeuristic(Node EndNode)
{
	if(AllowDiags_)
		H_Score_ = std::sqrt(std::pow(X_ - EndNode.X(), 2.0) + (std::pow(Y_ - EndNode.Y(), 2.0))); 
	else
		H_Score_ = std::abs(X_ - EndNode.X()) + std::abs(Y_ - EndNode.Y());

	return H_Score_;
}

void Node::GetNeighbours(Map map)
{
	//TODO : add a map of walls
	Neighbours_.push_back(Node(X_ + 1, Y_));
	Neighbours_.push_back(Node(X_ - 1, Y_));
	Neighbours_.push_back(Node(X_, Y_ + 1));
	Neighbours_.push_back(Node(X_, Y_ - 1));

	if(AllowDiags_)
	{
		Neighbours_.push_back(Node(X_ + 1, Y_ + 1));
		Neighbours_.push_back(Node(X_ - 1, Y_ + 1));
		Neighbours_.push_back(Node(X_ + 1, Y_ - 1));
		Neighbours_.push_back(Node(X_ - 1, Y_ - 1));
	}
}



bool operator == (Node &a, Node &b)
{
	return (a.X() == b.X()) && (a.Y() == b.Y());
}



//Return the index of the best node in an array of Node (the one with the lowest F_Score_)
int GetLowestScoreNode(std::vector<Node> Vec)
{
	int BestScore = POSITIVE_INFINITY, Index;

	for(int i = 0; i < Vec.size(); ++i)
	{
		if(Vec[i].F_Score() < BestScore)
		{
			BestScore = Vec[i].F_Score();
			Index = i;
		}
	}
	return Index;
}

bool IsInVector(std::vector<Node> Vec, Node Target)
{
	return((std::find(Vec.begin(), Vec.end(), Target)) != Vec.end());
}

bool A_star(Node Begin, Node End)
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
			return SUCCES_;

		OpenSet.erase(OpenSet.begin() + BestNodeIndex);
		ClosedSet.push_back(CurrentNode);
		
		CurrentNode.GetNeighbours(map);

		
		for(int i = 0; i < CurrentNode.Neighbours().size(); ++i)
		{
			Node Neighbour = CurrentNode.Neighbours()[i];

			if(IsInVector(ClosedSet, Neighbour))	//Already evaluated
				continue;

			if(!IsInVector(OpenSet, Neighbour))
				OpenSet.push_back(Neighbour);

			int Tentative_GScore = CurrentNode.G_Score() + CurrentNode.CalcHeuristic(Neighbour);
			if(Tentative_GScore >= OpenSet.back().F_Score())			//Not a new best path
				continue;

			//New best path
			Neighbour.SetCameFrom(&CurrentNode);
			Neighbour.SetG_Score(Tentative_GScore);
			Neighbour.CalcF_Score();
		}
	}

	return FAILURE_;
}
