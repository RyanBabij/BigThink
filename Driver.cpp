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
	
	// for (int i=0;i<3;++i)
	// {
		// std::cout<<"Turn: "<<i<<"\n";
		// std::cout<<mainBoard.getState()<<"\n";
	// }
	
	int i=0;
	while(++i<100)
	{
		int x1=-1;
		int y1=-1;
		int x2=-1;
		int y2=-1;

		std::cout<<"Turn: "<<i<<"\n";
		std::cout<<mainBoard.getState(true)<<"\n";
		
		std::cout<<"Which piece to move?\n";
		std::cin >> x1 >> y1;
		std::cout<<"Where to move it?\n";
		std::cin >> x2 >> y2;
		
		while (std::cin.fail())
		{
			std::cout<<"Invalid input, try again.\n";
			std::cin.clear();
			std::cin.ignore();
			
			x1=-1;
			y1=-1;
			x2=-1;
			y2=-1;
			--i;
		}
		
		if ( mainBoard.isSafe(x1,y1) )
		{
			if (mainBoard.isSafe(x2,y2))
			{
				std::cout<<"Command. Move "<<x1<<", "<<y1<<" to "<<x2<<", "<<y2<<"\n";
				
				if ( mainBoard.aBoard[x1][y1] != 0)
				{
					if ( x1==x2 && y1==y2 )
					{
						std::cout<<"Error: Cannot move to same position.\n";
					}
					else
					{
						// move the piece
						mainBoard.move(x1,y1,x2,y2);
						
						std::cout<<mainBoard.getState(true)<<"\n";
						
						// black moves
						std::cout<<"\nBlack moves\n";
						
						// make black move here.
						// get all black pieces
						auto vBlackPiece = mainBoard.getAllPieces(BLACK);
						std::cout<<"Black has "<<vBlackPiece->size()<<" pieces.\n";
						
						std::cout<<"Printing all black pieces:\n";
						if (vBlackPiece == 0)
						{
							std::cout<<"Error: No black pieces found.\n";
							return 0;
						}
						for (int i2=0;i2<vBlackPiece->size();++i2)
						{
							std::cout<<(*vBlackPiece)(i2)->getName()<<"\n";
						}
						
						Vector <Board*> * vBlackMoves = new Vector <Board*>;
						//Vector <Board*> * vMoves;
						// get all moves for all black pieces
						for (int i2=0;i2<vBlackPiece->size();++i2)
						{
							std::cout<<"Generating moves for piece: "<<(*vBlackPiece)(i2)->getName()<<"\n";
							mainBoard.addAllMovesFrom((*vBlackPiece)(i2),vBlackMoves);
						}
						std::cout<<"Found "<<vBlackMoves->size()<<" moves.\n";
						
						for(int i2=0;i2<vBlackMoves->size();++i2)
						{
							std::cout<<(*vBlackMoves)(i2)->getState()<<"\n";
						}
						
						// pick a random state.
						std::cout<<"Picking random move.\n";
						
						if (vBlackMoves->size() == 0)
						{
							std::cout<<"Error: No black moves found.\n";
							return 0;
						}
						int randMove = rng.rand(vBlackMoves->size());
						
						
						mainBoard = *(*vBlackMoves)(randMove);
						
						++i;
					}
				}
				else
				{
					std::cout<<"Error: Nothing there to move.\n";
				}
			}
			else
			{
				std::cout<<"Error: Bad move coordinate.\n";
			}
		}
		else
		{
			std::cout<<"Error: Bad start coordinate.\n";
		}
	}
	
	return 0;
}