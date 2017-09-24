#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <conio.h>
#include <string>
#include <sstream>

#include "Map.h"
#include "Map_SteamWorks.h"


#define POSITIVE_INFINITY 1000000000


struct Pos
{
	Pos(int X, int Y);
	Pos();
	int X_, Y_;
};

Pos::Pos(int X, int Y)
{
	X_ = X;
	Y_ = Y;
}

Pos::Pos()
{
	X_ = 0;
	Y_ = 0;
}

class Node
{

public:

	Node(int X, int Y, bool IsWall, bool AllowDiag, bool IsStart);

	void NewPath(std::vector<Pos> Path, Node PrevNode);

	void GetNeighbours(Map map);
	inline std::vector<Node> Neighbours(){return Neighbours_;};
	
	
	inline int X(){return X_;};
	inline int Y(){return Y_;};
	

	inline void CalcF_Score(){F_Score_ = G_Score_ + H_Score_;};
	inline int F_Score(){return F_Score_;};

	float CalcHeuristic(Node EndNode);				//may also return value to simplifie main code
	inline float H_Score(){return H_Score_;};
	
	inline void SetG_Score(int G_Score){G_Score_ = G_Score;};
	inline float G_Score(){return G_Score_;};

	inline void SetIsWall(bool IsWall){IsWall_ = IsWall;};
	inline bool IsWall(){return IsWall_;};

	inline void SetCameFrom(Node *CameFrom){CameFrom_ = CameFrom;};
	inline Node * GetCameFrom(){return CameFrom_;};

	inline std::vector<Pos> OptimumPath(){return OptimumPath_;};

private:
	int X_, Y_;
	float F_Score_, H_Score_, G_Score_;

	bool AllowDiags_, IsWall_;

	std::vector<Node> Neighbours_;
	Node * CameFrom_;
	std::vector<Pos> OptimumPath_;
};

Node::Node(int X = 0, int Y = 0, bool IsWall = false, bool AllowDiags = true, bool IsStart = false):
X_(X),
Y_(Y),

IsWall_(IsWall),
AllowDiags_(AllowDiags),

F_Score_(POSITIVE_INFINITY * !IsStart),
H_Score_(POSITIVE_INFINITY * !IsStart),
G_Score_(POSITIVE_INFINITY * !IsStart),

CameFrom_(NULL)
{
	if(IsStart)
		OptimumPath_.push_back(Pos(X_, Y_));
}


//Make a guess about the distance from a node to the end
//we can under estimate it without a problem, but we can't under estimate it

float Node::CalcHeuristic(Node EndNode)
{
	if(AllowDiags_)
		H_Score_ = std::sqrt(std::pow(X_ - EndNode.X(), 2.0) + (std::pow(Y_ - EndNode.Y(), 2.0))); 
	else
		H_Score_ = std::abs(X_ - EndNode.X()) + std::abs(Y_ - EndNode.Y());

	return H_Score_;
}

void Node::GetNeighbours(Map map)
{	
	if(map[X_ + 1][Y_] != 1)
		Neighbours_.push_back(Node(X_ + 1, Y_));
	if(map[X_ - 1][Y_] != 1)
		Neighbours_.push_back(Node(X_ - 1, Y_));
	if(map[X_][Y_ + 1] != 1)
		Neighbours_.push_back(Node(X_, Y_ + 1));
	if(map[X_][Y_ - 1] != 1)
		Neighbours_.push_back(Node(X_, Y_ - 1));

	if(AllowDiags_)
	{
		if(map[X_ + 1][Y_ + 1] != 1)
			Neighbours_.push_back(Node(X_ + 1, Y_ + 1));
		if(map[X_ - 1][Y_ + 1] != 1)
			Neighbours_.push_back(Node(X_ - 1, Y_ + 1));
		if(map[X_ + 1][Y_ - 1] != 1)
			Neighbours_.push_back(Node(X_ + 1, Y_ - 1));
		if(map[X_ - 1][Y_ - 1] != 1)
			Neighbours_.push_back(Node(X_ - 1, Y_ - 1));
	}
}

void Node::NewPath(std::vector<Pos> Path, Node PrevNode)
{
	OptimumPath_ = Path;
	OptimumPath_.push_back(Pos(PrevNode.X(), PrevNode.Y()));
}

bool operator == (Node &a, Node &b)
{
	return (a.X() == b.X()) && (a.Y() == b.Y());
}

std::ostream &operator<<(std::ostream &os, Node &a)
{
	return os<<"[ "<<a.X()<<", "<<a.Y()<<" ]\tF : "<<a.F_Score()<<",  G : "<<a.G_Score()<<",  H : "<<a.H_Score();
}

//Return the index of the best node in an array of Node (the one with the lowest F_Score_)
int GetLowestScoreNode(std::vector<Node> Vec)
{
	int BestScore = POSITIVE_INFINITY, Index = 0;

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
	//return((std::find(Vec.begin(), Vec.end(), Target)) != Vec.end());
	//return 0;

	for(int i = 0; i < Vec.size(); ++i)
	{
		if(Vec[i].X() == Target.X() && Vec[i].Y() == Target.Y())
			return 1;
	}

	return 0;
}

void Draw(std::vector<Pos> Path, Pos Target, Map map)
{
	for(int i = 0; i < HEIGHT; ++i)
	{
		for(int j = 0; j < WIDTH; ++j)
		{
			bool PrintPath = false;

			for(int k = 0; k < Path.size(); ++k)
			{
				if(Path[k].X_ == j && Path[k].Y_ == i)
				{
					PrintPath = true;
					break;
				}
			}

			if(PrintPath)
				std::cout<<"*";
			else if(j == Target.X_ && i == Target.Y_)
				std::cout<<"@";
			else if(map[j][i] == 1)
				std::cout<<"#";
			else
				std::cout<<" ";
		}
		std::cout<<"\n";
	}
}

Node A_star(Node Begin, Node End)
{
	std::vector<Node> ClosedSet;
	std::vector<Node> OpenSet;
	OpenSet.push_back(Begin);

	Node CurrentNode = Begin;

	while(OpenSet.size() > 0 && Map_SteamWorks[End.X()][End.Y()] != 1)
	{
		int BestNodeIndex = GetLowestScoreNode(OpenSet);
		CurrentNode = OpenSet[BestNodeIndex];

		system("cls");
		Draw(CurrentNode.OptimumPath(), Pos(End.X(), End.Y()), Map_SteamWorks);

		if(CurrentNode == End)
		{
			std::cout<<"Succes!!!\n\n";
			system("pause");
			return CurrentNode;
		}

		OpenSet.erase(OpenSet.begin() + BestNodeIndex);
		
		ClosedSet.push_back(CurrentNode);
		
		CurrentNode.GetNeighbours(Map_SteamWorks);

		
		for(int i = 0; i < CurrentNode.Neighbours().size(); ++i)
		{
			Node Neighbour = CurrentNode.Neighbours()[i];
			
			if(IsInVector(ClosedSet, CurrentNode.Neighbours()[i]))	//Already evaluated
				continue;
			
			if(!IsInVector(OpenSet, CurrentNode.Neighbours()[i]))
				OpenSet.push_back(CurrentNode.Neighbours()[i]);

			double Tentative_GScore = CurrentNode.G_Score() + CurrentNode.CalcHeuristic(OpenSet.back());
			
			if(Tentative_GScore >= OpenSet.back().G_Score())			//Not a new best path
				continue;
			
			//New best path
			
			OpenSet.back().NewPath(CurrentNode.OptimumPath(), CurrentNode);
			
			
			OpenSet.back().SetG_Score(Tentative_GScore);
			OpenSet.back().CalcHeuristic(End);
			OpenSet.back().CalcF_Score();
		}
	}


	std::cout<<"No solution!\n";
	system("pause");
	return CurrentNode;
}



int main()
{
	int StartNode_X = 1, StartNode_Y = 1, GoalNode_X = 28, GoalNode_Y = 28;

	std::cout<<"Welcome on a FRC SteamWorks map!\n\n\n";
	std::string Command;
	while(1)
	{
		std::getline(std::cin, Command);
		
		if(Command == "help")
		{
			std::cout<<"\n\n\t   ********** Help **********\n\n";
			std::cout<<"set start node -> set the start position of the virtual robot\n\n";
			std::cout<<"set goal node  -> set the goal position of the virtual robot\n\n";
			std::cout<<"find path      -> start the A_star algorithm\n\n";

			std::cout<<"\t   **************************\n\n";
		}

		if(Command == "set start node")
		{
			std::cout<<"Enter X : ";
			std::cin>>StartNode_X;
			std::cout<<"Enter Y : ";
			std::cin>>StartNode_Y;
			std::cout<<"\n\n";
		}

		else if(Command == "set goal node")
		{
			std::cout<<"Enter X : ";
			std::cin>>GoalNode_X;
			std::cout<<"Enter Y : ";
			std::cin>>GoalNode_Y;
			std::cout<<"\n\n";
		}

		else if(Command == "find path")
		{
			Node StartNode(StartNode_X, StartNode_Y, 0, 1, 1);
			Node GoalNode(GoalNode_X, GoalNode_Y);
			Node node = A_star(StartNode, GoalNode);
			std::cout<<"\n\n";
		}
		else
		{
			std::cout<<"\""<<Command<<"\" : unknown command\n";
		}
	}
	system("pause");

	return 0;
}
