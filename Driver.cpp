// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

// assume always promotion to queen

// idea: use tree to come up with move sets.
// use neural net to decide which states are best aka positional play
// however to start out we can use simple material points system.

#include <File/FileManagerStatic.hpp>

#include <Container/Vector/Vector.hpp>
#include <Math/Random/RandomLehmer.hpp>

#include <Data/DataTools.hpp>

#include <iostream>
#include <string>
#include <time.h>

#define WHITE true
#define BLACK false

#define WPAWN 244
#define BPAWN 245
#define WROOK 'r'
#define BROOK 'R'
#define WKNIGHT 'n'
#define BKNIGHT 'N'
#define WBISHOP 'b'
#define BBISHOP 'B'
#define WQUEEN 'q'
#define BQUEEN 'Q'
#define WKING 'k'
#define BKING 'K'

RandomLehmer rng;

#include "Piece.hpp"
#include "Board.hpp"



int main (int narg, char ** arg)
{	
	rng.seed(time(NULL));
	
	Board mainBoard;
	mainBoard.reset();

	//std::cout<<"Board state:\n"<<mainBoard.getState()<<"\n\n";
	
	// get all white pieces
	//auto vPiece = mainBoard.getAllPieces(WHITE);
	//std::cout<<"White has "<<vPiece->size()<<" pieces.\n";
	
	//std::cout<<"Printing all white pieces:\n";
	// if (vPiece == 0)
	// {
		// //std::cout<<"Error: No white pieces found.\n";
		// return 0;
	// }
	// for (int i=0;i<vPiece->size();++i)
	// {
		// //std::cout<<(*vPiece)(i)->getName()<<"\n";
	// }
	
	// Vector <Board*> * vMoves = new Vector <Board*>;
	// //Vector <Board*> * vMoves;
	// // get all moves for all white pieces
	// for (int i=0;i<vPiece->size();++i)
	// {
		// //std::cout<<"Generating moves for piece: "<<(*vPiece)(i)->getName()<<"\n";

		// //Vector <Board*> * vMoves = mainBoard.getAllMovesFrom((*vPiece)(i));
		// //mainBoard.addAllMovesFrom((*vPiece)(i),vMoves);
	// }
	// std::cout<<"Found "<<vMoves->size()<<" moves.\n";
	
	// for(int i=0;i<vMoves->size();++i)
	// {
		// //std::cout<<(*vMoves)(i)->getState()<<"\n";
	// }
	
	// pick a random state.
	//mainBoard.pickRandomSub();
	
	// for (int i=0;i<3;++i)
	// {
		// std::cout<<"Turn: "<<i<<"\n";
		// std::cout<<mainBoard.getState()<<"\n";
	// }
	
	std::string gameLog = "";
	
	std::cout<<"\n\nBigThink chess engine\n";
	std::cout<<"Enter 4 digits to make move.\n";
	std::cout<<" a - let AI move\n";
	std::cout<<" b - let AI play full game\n";
	
	int currentTurn=0;
	while(++currentTurn<100)
	{
		int x1=-1;
		int y1=-1;
		int x2=-1;
		int y2=-1;
		
		std::string input;
		
		std::cout<<mainBoard.getState(true)<<"\n";
		//std::cout<<"Press enter to continue.\n";
		//std::cin.get();
		//std::cout<<"Which piece to move? (Enter 9 for AI move).\n";
		//std::cin >> x1 >> y1;
		//std::cout<<"Where to move it?\n";
		//std::cin >> x2 >> y2;
		
		// command options
		// a - AI turn
		// b - full AI vs AI game
		// [4 digits from 0-7] - make move if valid.
		
		std::cout<<"Enter command:\n";
		std::cin>>input;
		
		while (std::cin.fail())
		{
			std::cout<<"Invalid input, try again.\n";
			std::cin.clear();
			std::cin.ignore();
			
			input = "";
			
			// x1=-1;
			// y1=-1;
			// x2=-1;
			// y2=-1;
			--currentTurn;
		}
		
		// process input here.
		if (input.find('b') != std::string::npos)
		{
			// play entire game until somebody wins
			int i=1;
			while (true)
			{	
				std::cout<<"\n\nTurn: "<<i++<<"\n";
				gameLog+="\n\nTurn: "+DataTools::toString(i-1)+"\n\n";
				
				if ( mainBoard.boardStatus() == 1 )
				{
					std::cout<<"White is in check/checkmate.\n";
					gameLog+="White is in check/checkmate.\n";
				}
				else if ( mainBoard.boardStatus() == 2 )
				{
					std::cout<<"Stalemate.\n";
					gameLog+="Stalemate.\n";
					return 0;
				}
				
				if ( mainBoard.randomMove(BLACK) == false )
				{
					std::cout<<"White is unable to move.\n";
					return 0;
				}
				//mainBoard.materialMove(WHITE);
				//mainBoard.materialDepthMove(WHITE, 2);
				
				
				// analysis
				std::cout<<mainBoard.getState(true)<<"\n";
				gameLog+=mainBoard.getState(true)+"\n\n";
				
				if ( mainBoard.hasKing(BLACK)==false )
				{
					std::cout<<"White wins\n";
					break;
				}
				
				std::cout<<"Material scores: "<<mainBoard.getMaterialScore(WHITE)<<" / "<<mainBoard.getMaterialScore(BLACK)<<"\n";
				
				if ( mainBoard.boardStatus() == 1 )
				{
					std::cout<<"Black is in check/checkmate.\n";
					gameLog+="Black is in check/checkmate.\n";
				}
				else if ( mainBoard.boardStatus() == 2 )
				{
					std::cout<<"Stalemate.\n";
					gameLog+="Stalemate.\n";
					return 0;
				}
				
				if ( mainBoard.randomMove(BLACK) == false )
				{
					std::cout<<"Black is unable to move.\n";
					return 0;
				}
				//mainBoard.materialMove(BLACK);
				//mainBoard.materialDepthMove(BLACK,2);
				
				std::cout<<mainBoard.getState(true)<<"\n";
				gameLog+=mainBoard.getState(true)+"\n\n";
				
				if ( mainBoard.hasKing(WHITE)==false )
				{
					std::cout<<"Black wins\n";
					break;
				}
				
				std::cout<<"Material scores: "<<mainBoard.getMaterialScore(WHITE)<<" / "<<mainBoard.getMaterialScore(BLACK)<<"\n";
			}
			
			// finished
			std::cout<<"Writing game log\n";
			FileManagerStatic::writeFreshString(gameLog,"gamelog.txt");
			return 0;
		}
		
		if (input.find('a') != std::string::npos)
		{
			std::cout<<"AI turn\n";
			
			mainBoard.randomMove(WHITE);
			// analysis
			std::cout<<mainBoard.getState(true)<<"\n";
			
			if ( mainBoard.hasKing(BLACK)==false )
			{
				std::cout<<"White wins\n";
				return 0;
			}
			
			mainBoard.randomMove(BLACK);
			
			std::cout<<mainBoard.getState(true)<<"\n";
			
			if ( mainBoard.hasKing(WHITE)==false )
			{
				std::cout<<"Black wins\n";
				return 0;
			}
			
		}
		else
		{
			// process normally.
			// strip the first 4 numbers we find in the string.
			// valid input is 4 numbers from 0-7. Anything else is ignored.
			int digits[4]={-1};
			
			int currentDigit = 0;
			for (unsigned int i=0;i<input.size();++i)
			{
				if (std::isdigit(input[i]))
				{
					int digit = DataTools::toInt(input[i]);
					
					if (digit >= 0 && digit < 8)
					{
						digits[currentDigit] = DataTools::toInt(input[i]);
						++currentDigit;
					}
					if (currentDigit==4)
					{
						break;
					}
				}
			}
			
			// process digits
			if (currentDigit == 4)
			{
				//std::cout<<"Valid input: \n";
				//std::cout<<digits[0]<<digits[1]<<digits[2]<<digits[3]<<"\n";
				x1=digits[0];
				y1=digits[1];
				x2=digits[2];
				y2=digits[3];
				
				if (mainBoard.move(x1,y1,x2,y2))
				{
					std::cout<<mainBoard.getState(true)<<"\n";
					if ( mainBoard.hasKing(BLACK)==false )
					{
						std::cout<<"White wins\n";
						break;
					}
					
					std::cout<<"Material scores: "<<mainBoard.getMaterialScore(WHITE)<<" / "<<mainBoard.getMaterialScore(BLACK)<<"\n";
					
					char blackState = mainBoard.boardStatus();
					//std::cout<<"Blackstate: "<<(int)blackState<<"\n";
					
					if ( blackState == 1 )
					{
						std::cout<<"Black is in check/checkmate.\n";
						gameLog+="Black is in check/checkmate.\n";
					}
					else if ( blackState == 2 )
					{
						std::cout<<"Stalemate.\n";
						gameLog+="Stalemate.\n";
						return 0;
					}
					
					if ( mainBoard.randomMove(BLACK) == false )
					{
						std::cout<<"Black is unable to move.\n";
						// if we are in check, this is checkmate
						if (mainBoard.boardStatus() != 0)
						{
							std::cout<<"Black is in checkmate, white wins.\n";
						}
						return 0;
					}
					//mainBoard.materialMove(BLACK);
					//mainBoard.materialDepthMove(BLACK,2);
					
					//std::cout<<mainBoard.getState(true)<<"\n";
					gameLog+=mainBoard.getState(true)+"\n\n";
					
					// if ( mainBoard.hasKing(WHITE)==false )
					// {
						// std::cout<<"Black wins\n";
						// break;
					// }
					
					std::cout<<"Material scores: "<<mainBoard.getMaterialScore(WHITE)<<" / "<<mainBoard.getMaterialScore(BLACK)<<"\n";
				}
				
				//std::cout<<"\n\nTurn: "<<i++<<"\n";
				//gameLog+="\n\nTurn: "+DataTools::toString(i-1)+"\n\n";
				
				// if ( mainBoard.boardStatus(WHITE) == 1 )
				// {
					// std::cout<<"White is in check/checkmate.\n";
					// gameLog+="White is in check/checkmate.\n";
				// }
				// else if ( mainBoard.boardStatus(WHITE) == 2 )
				// {
					// std::cout<<"Stalemate.\n";
					// gameLog+="Stalemate.\n";
					// return 0;
				// }
				
				// if ( mainBoard.randomMove(WHITE) == false )
				// {
					// std::cout<<"White is unable to move.\n";
					// return 0;
				// }
				// //mainBoard.materialMove(WHITE);
				// //mainBoard.materialDepthMove(WHITE, 2);
				
				
				// // analysis
				// std::cout<<mainBoard.getState(true)<<"\n";
				// gameLog+=mainBoard.getState(true)+"\n\n";
				

			}
			else
			{
				std::cout<<"Error: Invalid input.\n";
			}
		}
		
		// if ( mainBoard.isSafe(x1,y1) )
		// {
			// if (mainBoard.isSafe(x2,y2))
			// {
				// std::cout<<"Command. Move "<<x1<<", "<<y1<<" to "<<x2<<", "<<y2<<"\n";
				
				// if ( mainBoard.aBoard[x1][y1] != 0)
				// {
					// if ( x1==x2 && y1==y2 )
					// {
						// std::cout<<"Error: Cannot move to same position.\n";
					// }
					// else
					// {
						// // move the piece
						// mainBoard.move(x1,y1,x2,y2);
						
						// std::cout<<mainBoard.getState(true)<<"\n";
						
						// // black moves
						// std::cout<<"\nBlack moves\n";
						
						// mainBoard.randomMove(BLACK);
						
						// ++currentTurn;
					// }
				// }
				// else
				// {
					// std::cout<<"Error: Nothing there to move.\n";
				// }
			// }
			// else
			// {
				// std::cout<<"Error: Bad move coordinate.\n";
			// }
		// }
		// else if (x1 == 9)
		// {
			// std::cout<<"Ai move\n";
		// }
		// else
		// {
			// std::cout<<"Error: Bad start coordinate.\n";
		// }
	}
	
	return 0;
}