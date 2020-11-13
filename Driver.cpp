// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

// assume always promotion to queen

// idea: use tree to come up with move sets.
// use neural net to decide which states are best.
// however to start out we can use simple material points system.

#include <Container/Vector/Vector.hpp>
#include <Math/Random/RandomLehmer.hpp>

#include <iostream>
#include <string>
#include <time.h>

#define WHITE true
#define BLACK false

#include "Piece.hpp"
#include "Board.hpp"

RandomLehmer rng;

int main (int narg, char ** arg)
{
	rng.seed(time(NULL));
	
	Board mainBoard;
	mainBoard.reset();

	std::cout<<"Board state:\n"<<mainBoard.getState()<<"\n\n";
	
	// get all white pieces
	auto vPiece = mainBoard.getAllPieces(WHITE);
	std::cout<<"White has "<<vPiece->size()<<" pieces.\n";
	
	std::cout<<"Printing all white pieces:\n";
	if (vPiece == 0)
	{
		std::cout<<"Error: No white pieces found.\n";
		return 0;
	}
	for (int i=0;i<vPiece->size();++i)
	{
		std::cout<<(*vPiece)(i)->getName()<<"\n";
	}
	
	Vector <Board*> * vMoves = new Vector <Board*>;
	//Vector <Board*> * vMoves;
	// get all moves for all white pieces
	for (int i=0;i<vPiece->size();++i)
	{
		std::cout<<"Generating moves for piece: "<<(*vPiece)(i)->getName()<<"\n";

		//Vector <Board*> * vMoves = mainBoard.getAllMovesFrom((*vPiece)(i));
		mainBoard.addAllMovesFrom((*vPiece)(i),vMoves);
	}
	std::cout<<"Found "<<vMoves->size()<<" moves.\n";
	
	for(int i=0;i<vMoves->size();++i)
	{
		std::cout<<(*vMoves)(i)->getState()<<"\n";
	}
	
	// pick a random state.
	//mainBoard.pickRandomSub();
	
	for (int i=0;i<3;++i)
	{
		std::cout<<"Turn: "<<i<<"\n";
		std::cout<<mainBoard.getState()<<"\n";
	}
	
	return 0;
}