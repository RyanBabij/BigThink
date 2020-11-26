#include <Math/Random/RandomLehmer.hpp>

#include <time.h>

// Board store potential substates
// They should also store previous states for replay and also makes some
// analysis easier

class Board
{
	Vector <Board*> vSubstates; // substates if current side moves
	Vector <Board*> vSubstates2; // substates if opponent moves again
	
	std::string transitionName;
	
	public:
	bool sideToMove;
	
	Piece* aBoard [8][8];
	char status; // should be calculated whenever a board is generated.
	int id;
	static int STATIC_ID;
	
	~Board()
	{
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				if ( aBoard[x][y] != 0 )
				{
					delete aBoard[x][y];
				}
				aBoard[x][y] = 0;
			}
		}
		// recursively delete all substates
		clearSubs();
	}
	
	Board(bool _sideToMove = WHITE)
	{
		status=0;
		
		sideToMove=_sideToMove;
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				aBoard[x][y] = 0;
			}
		}
		transitionName="";
		
		id=STATIC_ID++;
	}
	Board(const Board& board) // copy constructor
	//(this should actually be a shift to substate)
	// but for now it's treated as a fresh board.
	// This needs to be updated to copy not just the board,
	// but any substates generated.
	{
		status=board.status;
		// I assume the default copy constructor copys the pointers in aBoard,
		// but doesn't copy the Piece objects.
		sideToMove=board.sideToMove;
		
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				if ( board.aBoard[x][y] != 0 )
				{
					// copy Piece
					aBoard[x][y] = new Piece (*board.aBoard[x][y]);
				}
				else
				{
					aBoard[x][y] = 0;
				}
			}
		}
		
		// recursively delete all substates
		clearSubs();
		
		id=STATIC_ID++;
	}
	
	// Equals operator: Identify identical board states for pruning?
	// Assignment operator: Copy board state.
	Board operator=( Board& board)
	{
		status=board.status;
		sideToMove=board.sideToMove;
		
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				if ( board.aBoard[x][y] != 0 )
				{
					// copy Piece
					aBoard[x][y] = new Piece (*board.aBoard[x][y]);
				}
				else
				{
					aBoard[x][y] = 0;
				}
			}
		}
		id=STATIC_ID++;
		
		//vSubstates.clearPtr();
		//vSubstates2.clearPtr();
		
		if ( board.vSubstates.size() != 0)
		{
			vSubstates = board.vSubstates;
		}
		if ( board.vSubstates2.size() != 0)
		{
			vSubstates2 = board.vSubstates2;
		}
		
		// this seems to cause a crash if you are assigning a substate
		// for now I'll call it after assignment finishes.
		//clearSubs();
		
		return board;
	}

	std::string getSideToMove()
	{
		if (sideToMove==WHITE)
		{
			return "WHITE";
		}
		return "BLACK";
	}

	// move piece from (x1,y1) to (x2,y2). Return false if invalid move.
	// if must be the piece's team's turn, and the move should be valid
	// flipSideToMove needs to be false when moving pieces for castling
	
	// calling this is bad because it requires modifying substates.
	// better to simply find the matching substate and move to it.
	bool move (int x1, int y1, int x2, int y2, bool flipSideToMove=true)
	{
		// assume any target piece is captured.
		if ( isSafe(x1,y1) == false || isSafe(x2,y2) == false ||
		aBoard[x1][y1] == 0 )
		{
			std::cout<<"There is no piece at "<<x1<<", "<<y1<<"\n";
			return false;
		}
		
		Piece* movePiece = aBoard[x1][y1];
		
		if ( movePiece->team != sideToMove )
		{
			//std::cout<<"Invalid move: Piece "<<movePiece->x<<", "<<movePiece->y<<" is not on the side to move.\n";
			//return false;
		}
		
		aBoard[x1][y1]=0;
		if (aBoard[x2][y2] != 0 )
		{
			delete aBoard[x2][y2];
		}
		aBoard[x2][y2] = movePiece;
		movePiece->x = x2;
		movePiece->y = y2;
		
		//promotion
		if ( movePiece->getShortName() == WPAWN )
		{
			if (movePiece->y == 7)
			{
				movePiece->promote();
			}
		}
		else if ( movePiece->getShortName() == BPAWN )
		{
			if (movePiece->y == 0)
			{
				movePiece->promote();
			}
		}
		
		// castling
		// if player moves king 2-3 spaces, this is a request for castling.
		
		// en passant
		
		
		movePiece->hasMoved=true;
		
		if (flipSideToMove)
		{
			sideToMove=!sideToMove;

			// set all opponent pieces no longer vulnerable to en passant
			Vector <Piece*> * vPiece = getAllPieces(!movePiece->team);
			
			for (int i=0;i<vPiece->size();++i)
			{
				(*vPiece)(i)->doubleMoved=false;
			}
			delete vPiece;
		}
		
		//substates must be cleared/merged
		clearSubs();
		status=0;
		
		return true;
	}

	void reset()
	{
		sideToMove=WHITE;
		
		// wipe the board
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				aBoard[x][y] = 0;
			}
		}
		
		for (int i=0;i<8;++i)
		{
			aBoard[i][1] = new Piece ("pawn", WPAWN, WHITE, i, 1, 1);
			aBoard[i][6] = new Piece("pawn", BPAWN, BLACK, i, 6, 1);
		}
		
		aBoard[0][0] = new Piece ("rook", WROOK, WHITE, 0,0,5);
		aBoard[7][0] = new Piece ("rook", WROOK, WHITE, 7,0,5);
		aBoard[0][7] = new Piece ("rook", BROOK, BLACK, 0,7,5);
		aBoard[7][7] = new Piece ("rook", BROOK, BLACK, 7,7,5);
		
		aBoard[1][0] = new Piece ("knight", WKNIGHT, WHITE, 1,0,3);
		aBoard[6][0] = new Piece ("knight", WKNIGHT, WHITE, 6,0,3);
		aBoard[1][7] = new Piece ("knight", BKNIGHT, BLACK, 1,7,3);
		aBoard[6][7] = new Piece ("knight", BKNIGHT, BLACK, 6,7,3);
		
		aBoard[2][0] = new Piece ("bishop", WBISHOP, WHITE, 2,0,3);
		aBoard[5][0] = new Piece ("bishop", WBISHOP, WHITE, 5,0,3);
		aBoard[2][7] = new Piece ("bishop", BBISHOP, BLACK, 2,7,3);
		aBoard[5][7] = new Piece ("bishop", BBISHOP, BLACK, 5,7,3);
		
		aBoard[3][0] = new Piece ("queen", WQUEEN, WHITE, 3,0,9);
		aBoard[4][0] = new Piece ("king", WKING, WHITE, 4,0,1000);
		aBoard[3][7] = new Piece ("queen", BQUEEN, BLACK, 3,7,9);
		aBoard[4][7] = new Piece ("king", BKING, BLACK, 4,7,1000);
	}
	
	// only return all moves for this piece, in the form of state vector
	Vector <Board*>* getAllMovesFrom(Piece* piece)
	{
		Vector <Board*>* vBoard = new Vector <Board*>;
		
		addAllMovesFrom(piece, vBoard);
			
		if (vBoard->size() == 0)
		{
			delete vBoard;
			return 0;
		}
		
		return vBoard;
	}
	
	// returns true if the board array is not 0.
	Piece* hasPieceOn(const short int _x, const short int _y)
	{
		return aBoard[_x][_y];
	}
	// returns true if the board array is not 0.
	Piece* hasPieceOn(const short int _x, const short int _y, const bool _team)
	{
		if ( aBoard[_x][_y]==0 || aBoard[_x][_y]->team != _team)
		{
			return 0;
		}
		
		return (aBoard[_x][_y]);
	}
	
	// return true if the team can send a piece to this tile.
	// for now we do this by generating sub boards and just looking to
	// see if a substate has a piece there
	Piece* canAttack(const short int _x, const short int _y, bool _team)
	{
		generateSubs();

		for (int i=0;i<vSubstates.size();++i)
		{
			if ( vSubstates(i)->hasPieceOn(_x,_y,_team) )
			{
				return vSubstates(i)->hasPieceOn(_x,_y,_team);
			}
		}

		for (int i=0;i<vSubstates2.size();++i)
		{
			if (vSubstates2(i)->hasPieceOn(_x,_y,_team))
			{
				return vSubstates2(i)->hasPieceOn(_x,_y,_team);
			}
		}
			
		return 0;
	}
	
	
	// only return all moves for this piece, in the form of state vector
	void addAllMovesFrom(Piece* piece, Vector <Board*> * vBoard)
	{
		if (piece==0 || isSafe(piece->x,piece->y)==false || vBoard==0)
		{
			std::cout<<"addallmoves error\n";
			return;
		}
	
		// white pawn, can move up 1 space, attack diagonally.
		if (piece->getShortName() == 244)
		{
			// can it move up 1 space?
			if (isSafe(piece->x,piece->y+1))
			{
				if ( aBoard[piece->x][piece->y+1] == 0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,piece->y+1);
					vBoard->push(subBoard);
					
					// can it move up 2 spaces?
					if (piece->y==1 && isSafe(piece->x,piece->y+2))
					{
						if ( aBoard[piece->x][piece->y+2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y+2);
							piece->doubleMoved=true;
							vBoard->push(subBoard2);
						}
					}
				}
			}

			// can it attack diagonally left?
			if (isSafe(piece->x-1,piece->y+1))
			{
				if ( aBoard[piece->x-1][piece->y+1] != 0 && aBoard[piece->x-1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
			// can it attack diagonally right?
			if (isSafe(piece->x+1,piece->y+1))
			{
				if ( aBoard[piece->x+1][piece->y+1] != 0 && aBoard[piece->x+1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y+1);
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
			
			// can it attack en passant?
			// normally we can assume the en passant attack square is empty
			// but in this case we will check
			// en passant left
			if (isSafe(piece->x-1,piece->y) && isSafe(piece->x-1,piece->y+1))
			{
				if ( aBoard[piece->x-1][piece->y] != 0 && aBoard[piece->x-1][piece->y]->team != piece->team &&
				aBoard[piece->x-1][piece->y]->doubleMoved
				&& aBoard[piece->x-1][piece->y+1]==0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					subBoard->aBoard[piece->x-1][piece->y] = 0;
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
			// en passant right
			if (isSafe(piece->x+1,piece->y) && isSafe(piece->x+1,piece->y+1))
			{
				if ( aBoard[piece->x+1][piece->y] != 0 && aBoard[piece->x+1][piece->y]->team != piece->team &&
				aBoard[piece->x+1][piece->y]->doubleMoved
				&& aBoard[piece->x+1][piece->y+1]==0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y+1);
					subBoard->aBoard[piece->x+1][piece->y] = 0;
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
		}
		// black pawn, can move down 1 space, attack diagonally.
		else if (piece->getShortName() == 245)
		{
			// can it move down 1 space?
			if (isSafe(piece->x,piece->y-1))
			{
				if ( aBoard[piece->x][piece->y-1] == 0 )
				{
					// piece can move down 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,piece->y-1);
					vBoard->push(subBoard);
					
					// can it move down 2 spaces?
					if (piece->y==6 && isSafe(piece->x,piece->y-2))
					{
						if ( aBoard[piece->x][piece->y-2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y-2);
							piece->doubleMoved=true;
							vBoard->push(subBoard2);
						}
					}
				}
			}

			
			// can it attack diagonally left?
			if (isSafe(piece->x-1,piece->y-1))
			{
				if ( aBoard[piece->x-1][piece->y-1] != 0 && aBoard[piece->x-1][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y-1);
					vBoard->push(subBoard);
				}
			}
			// can it attack diagonally right?
			if (isSafe(piece->x+1,piece->y-1))
			{
				if ( aBoard[piece->x+1][piece->y-1] != 0 && aBoard[piece->x+1][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y-1);
					vBoard->push(subBoard);
				}
			}
			// can it attack en passant?
			// normally we can assume the en passant attack square is empty
			// but in this case we will check
			// en passant left
			if (isSafe(piece->x-1,piece->y) && isSafe(piece->x-1,piece->y-1))
			{
				if ( aBoard[piece->x-1][piece->y] != 0 && aBoard[piece->x-1][piece->y]->team != piece->team &&
				aBoard[piece->x-1][piece->y]->doubleMoved
				&& aBoard[piece->x-1][piece->y-1]==0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y-1);
					subBoard->aBoard[piece->x-1][piece->y] = 0;
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
			// en passant right
			if (isSafe(piece->x+1,piece->y) && isSafe(piece->x+1,piece->y-1))
			{
				if ( aBoard[piece->x+1][piece->y] != 0 && aBoard[piece->x+1][piece->y]->team != piece->team &&
				aBoard[piece->x+1][piece->y]->doubleMoved
				&& aBoard[piece->x+1][piece->y-1]==0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y-1);
					subBoard->aBoard[piece->x+1][piece->y] = 0;
					subBoard->transitionName="Pawn capture";
					vBoard->push(subBoard);
				}
			}
		}
		// knight, moves in L shape
		else if (piece->getShortName() == 'n' || piece->getShortName() == 'N')
		{
			// left
			if (isSafe(piece->x-2,piece->y+1))
			{
				if ( aBoard[piece->x-2][piece->y+1] == 0 || aBoard[piece->x-2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y+1);
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-2,piece->y-1))
			{
				if ( aBoard[piece->x-2][piece->y-1] == 0 || aBoard[piece->x-2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y-1);
					vBoard->push(subBoard);
				}
			}
			// right
			if (isSafe(piece->x+2,piece->y+1))
			{
				if ( aBoard[piece->x+2][piece->y+1] == 0 || aBoard[piece->x+2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y+1);
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x+2,piece->y-1))
			{
				if ( aBoard[piece->x+2][piece->y-1] == 0 || aBoard[piece->x+2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y-1);
					vBoard->push(subBoard);
				}
			}
			// up
			if (isSafe(piece->x+1,piece->y+2))
			{
				if ( aBoard[piece->x+1][piece->y+2] == 0 || aBoard[piece->x+1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y+2);
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-1,piece->y+2))
			{
				if ( aBoard[piece->x-1][piece->y+2] == 0 || aBoard[piece->x-1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+2);
					vBoard->push(subBoard);
				}
			}
			// down
			if (isSafe(piece->x+1,piece->y-2))
			{
				if ( aBoard[piece->x+1][piece->y-2] == 0 || aBoard[piece->x+1][piece->y-2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y-2);
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-1,piece->y-2))
			{
				if ( aBoard[piece->x-1][piece->y-2] == 0 || aBoard[piece->x-1][piece->y-2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y-2);
					vBoard->push(subBoard);
				}
			}
			
		}
		
		// rook, moves in straight line
		else if (piece->getShortName() == 'r' || piece->getShortName() == 'R')
		{
			// down
			for (int file = piece->y-1; file >=0; --file)
			{
				if ( aBoard[piece->x][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[piece->x][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// up
			for (int file = piece->y+1; file<8; ++file)
			{
				if ( aBoard[piece->x][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[piece->x][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// left
			for (int rank = piece->x-1; rank >=0; --rank)
			{
				if ( aBoard[rank][piece->y] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][piece->y]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// right
			for (int rank = piece->x+1; rank <8; ++rank)
			{
				if ( aBoard[rank][piece->y] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][piece->y]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
		}
		
		// bishop, moves in diagonals
		else if (piece->getShortName() == 'B' || piece->getShortName() == 'b')
		{
			// down-left
			int file = piece->y-1;
			int rank = piece->x-1;
			
			while (file>=0 && rank >= 0)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				--file;
				--rank;
			}
			// down-right
			file = piece->y-1;
			rank = piece->x+1;
			
			while (file>=0 && rank <8)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				--file;
				++rank;
			}
			// up-left
			file = piece->y+1;
			rank = piece->x-1;
			
			while (file<8 && rank>=0)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				++file;
				--rank;
			}
			// up-right
			file = piece->y+1;
			rank = piece->x+1;
			
			while (file<8 && rank<8)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				++file;
				++rank;
			}
		}
		
		// queen, moves in diagonals and straight lines
		else if (piece->getShortName() == 'Q' || piece->getShortName() == 'q')
		{
			// down-left
			int file = piece->y-1;
			int rank = piece->x-1;
			
			while (file>=0 && rank >= 0)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				--file;
				--rank;
			}
			// down-right
			file = piece->y-1;
			rank = piece->x+1;
			
			while (file>=0 && rank <8)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				--file;
				++rank;
			}
			// up-left
			file = piece->y+1;
			rank = piece->x-1;
			
			while (file<8 && rank>=0)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				++file;
				--rank;
			}
			// up-right
			file = piece->y+1;
			rank = piece->x+1;
			
			while (file<8 && rank<8)
			{
				if ( aBoard[rank][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
				++file;
				++rank;
			}
			// down
			for (file = piece->y-1; file >=0; --file)
			{
				if ( aBoard[piece->x][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[piece->x][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// up
			for (file = piece->y+1; file<8; ++file)
			{
				if ( aBoard[piece->x][file] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[piece->x][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,file);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// left
			for (rank = piece->x-1; rank >=0; --rank)
			{
				if ( aBoard[rank][piece->y] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][piece->y]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
			// right
			for (rank = piece->x+1; rank <8; ++rank)
			{
				if ( aBoard[rank][piece->y] == 0 )
				{
					// rook can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][piece->y]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,piece->y);
					vBoard->push(subBoard);
					break;
				}
				else
				{
					// cannot move here
					break;
				}
			}
		}
		
		// king, moves in diagonals and straight lines but only 1 tile
		else if (piece->getShortName() == 'K' || piece->getShortName() == 'k')
		{
			// down-left
			int rank = piece->x-1;
			int file = piece->y-1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// down-right
			rank = piece->x+1;
			file = piece->y-1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// up-left
			rank = piece->x-1;
			file = piece->y+1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// up-right
			rank = piece->x+1;
			file = piece->y+1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// down
			rank = piece->x;
			file = piece->y-1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// up
			rank = piece->x;
			file = piece->y+1;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// left
			rank = piece->x-1;
			file = piece->y;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			// right
			rank = piece->x+1;
			file = piece->y;
			if (isSafe(rank,file))
			{
				if ( aBoard[rank][file] == 0 )
				{
					// piece can move here and further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
				else if (aBoard[rank][file]->team != piece->team)
				{
					// enemy piece here, can move here but no further
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,rank,file);
					vBoard->push(subBoard);
				}
			}
			
			// castling
			if (piece->hasMoved == false)
			{
				// castling WHITE
				if ( piece->team == WHITE )
				{
					if ( aBoard[0][0] != 0 && aBoard[1][0] == 0 &&
						aBoard[2][0] == 0 && aBoard[3][0] == 0
						&& aBoard[0][0]->getShortName() == 'r'
						&& aBoard[0][0]->hasMoved == false)
					{
						// make sure all tiles the king visits are not in check
						if ( canAttack(2,0,BLACK) || canAttack(3,0,BLACK) ||
						canAttack(4,0,BLACK) )
						{
							// White cannot castle queenside
							// as the king would be passing through check.
						}
						else
						{
							// Queenside castling is possible
							Board* subBoard = new Board(*this);
							Piece * rook = aBoard[0][0];
							subBoard->move(piece->x,piece->y,2,0,false);
							subBoard->move(rook->x,rook->y,3,0);
							vBoard->push(subBoard);
						}
					}
					if ( aBoard[7][0] != 0 && aBoard[6][0] == 0 &&
						aBoard[5][0] == 0
						&& aBoard[7][0]->getShortName() == 'r'
						&& aBoard[7][0]->hasMoved == false)
					{
						// make sure all tiles the king visits are not in check
						if ( canAttack(4,0,BLACK) || canAttack(5,0,BLACK) ||
						canAttack(6,0,BLACK) )
						{
							// White cannot castle kingside
							// as the king would be passing through check.
						}
						else
						{
							// Kingside castling is possible
							Board* subBoard = new Board(*this);
							Piece * rook = aBoard[7][0];
							subBoard->move(piece->x,piece->y,6,0,false);
							subBoard->move(rook->x,rook->y,5,0);
							vBoard->push(subBoard);
						}
					}
				}
				else
				{
					// castling BLACK
					if ( aBoard[0][7] != 0 && aBoard[1][7] == 0 &&
						aBoard[2][7] == 0 && aBoard[3][7] == 0
						&& aBoard[0][7]->getShortName() == 'R'
						&& aBoard[0][7]->hasMoved == false)
					{
						// make sure all tiles the king visits are not in check
						if ( canAttack(2,7,WHITE) || canAttack(3,7,WHITE) ||
						canAttack(4,7,WHITE) )
						{
							// Black cannot castle queenside
							// as the king would be passing through check.
						}
						else
						{
							// Queenside castling is possible
							Board* subBoard = new Board(*this);
							Piece * rook = aBoard[0][7];
							subBoard->move(piece->x,piece->y,2,7,false);
							subBoard->move(rook->x,rook->y,3,7);
							vBoard->push(subBoard);
						}
					}
					if ( aBoard[7][7] != 0 && aBoard[6][7] == 0 &&
						aBoard[5][7] == 0
						&& aBoard[7][7]->getShortName() == 'R'
						&& aBoard[7][7]->hasMoved == false)
					{
						// make sure all tiles the king visits are not in check
						if ( canAttack(4,7,WHITE) || canAttack(5,7,WHITE) ||
						canAttack(6,7,WHITE) )
						{
							// Black cannot castle kingside
							// as the king would be passing through check.
						}
						else
						{
							// Kingside castling is possible
							Board* subBoard = new Board(*this);
							Piece * rook = aBoard[7][7];
							subBoard->move(piece->x,piece->y,6,7,false);
							subBoard->move(rook->x,rook->y,5,7);
							vBoard->push(subBoard);
						}
					}
				}
			}
		}
	}
	
	bool isSafe(const int x, const int y)
	{
		return (x<8 && x>=0 && y<8 && y>=0);
	}
	
	std::string getState(bool displayCoordinates=false)
	{
		unsigned char vertLine = 179;
		unsigned char horizontalLine = 196;
		unsigned char intersect = 197;
		unsigned char dot = 249;
		
		std::string strBoard = "  ";
		for (int i=0;i<16;++i)
		{
			//strBoard+=horizontalLine;
		}
		strBoard+="\n";
		
		for (int y=7;y>=0;--y)
		{
			if (displayCoordinates)
			{
				strBoard+=std::to_string(y);
				strBoard+=" ";
			}
			for (int x=0;x<8;++x)
			{
				if ( aBoard[x][y]==0 )
				{
					strBoard+=vertLine;
					strBoard+=dot;
				}
				else
				{
					strBoard+=vertLine;
					strBoard += aBoard[x][y]->getShortName();
				}
			}
			strBoard+=vertLine;
			strBoard+="\n";
		}
		if (displayCoordinates)
		{
			strBoard+="   0 1 2 3 4 5 6 7";
		}
		return strBoard;
	}
	
	Vector <Piece*>* getAllPieces(bool _team)
	{
		Vector <Piece*>* vPieces = new Vector <Piece*>;
		// wipe the board
		for (int y=7;y>=0;--y)
		{
			for (int x=0; x<8; ++x)
			{
				if (aBoard[x][y]!=0)
				{
					if (aBoard[x][y]->team == _team)
					{
						vPieces->push(aBoard[x][y]);
					}
				}
			}
		}
		
		if (vPieces->size() == 0)
		{
			delete vPieces;
			return 0;
		}
		
		return vPieces;
	}
	
	int getNPieces (bool _team)
	{
		Vector <Piece*>* vPiece = getAllPieces(_team);
		int n = vPiece->size();
		
		for (int i=0;i<vPiece->size();++i)
		{
			//delete (*vPiece)(i);
		}
		vPiece->clear();
		delete vPiece;
		return n;
	}
	
	bool checkMatePossible ()
	{
		// requirments for checkmate: A team must have at least a king plus
		// * Queen
		// * Rook
		// * 2 bishops
		// * knight and bishop
		return false;
	}
	
	Vector <Board*> * getAllMoves(bool _team)
	{
		// get all movable pieces
		auto vPiece = getAllPieces(_team);
		
		Vector <Board*> * vMove = new Vector <Board*>;
		// get all moves for all pieces
		for (int i2=0;i2<vPiece->size();++i2)
		{
			addAllMovesFrom((*vPiece)(i2),vMove);
		}
		delete vPiece;
		
		if ( vMove->size() > 0 )
		{
			return vMove;
		}
		return 0;
	}
	
	// skip the turn and let the other side move again
	// this is obviously not legal in a real game
	// we still check if the side can move, and return false if
	// this isn't possible
	bool skipTurn(bool _team)
	{
		const bool bCanMove = canMove(_team);
		
		sideToMove = !sideToMove;
		clearSubs();
		return bCanMove;
	}
	
	bool canMove(bool _team)
	{
		Vector <Board*> * vMove = getAllMoves(_team);
		
		if ( vMove == 0 || vMove->size() == 0 )
		{
			delete vMove;
			return false;
		}
		vMove->clearPtr();
		delete vMove;
		return true;
	}
	
	// return list of legal board states.
	// basically all states that don't involve a side putting themself
	// into check.
	Vector <Board*> * getLegalMoves(bool _team)
	{
		generateSubs();

		if (vSubstates.size() == 0)
		{
			std::cout<<"Error: No moves found.\n";
			return 0;
		}
		
		// build legal move vector
		Vector <Board*> * vLegal = new Vector <Board*>;
		
		// push only legal moves to the vector
		for (int i=0; i<vSubstates.size(); ++i)
		{
			if ( vSubstates(i)->boardStatus() != WHITE_NO_KING
			 && vSubstates(i)->boardStatus() != BLACK_NO_KING )
			{
				vLegal->push( vSubstates(i) );
			}
		}

		if ( vLegal->size() == 0 )
		{
			// there is no legal move to make...
			// This is either a stalemate or checkmate.
			delete vLegal;
			return 0;
		}
		return vLegal;
	}
	
	bool randomMove (bool _team)
	{
		generateSubs();
		
		if (vSubstates.size() == 0 )
		{
			return false;
		}

		// build legal move vector
		Vector <Board*> * vLegal = getLegalMoves(_team);
		
		if ( vLegal == 0 || vLegal->size() == 0 )
		{
			// there is no legal move to make...
			// This is either a stalemate or checkmate.
			delete vLegal;
			return false;
		}
		
		*this = *(*vLegal)(rng.rand(vLegal->size()));
		//clearSubs();
		
		delete vLegal;
		return true;
	}
	
	// Pick the best immediate move to reduce enemy material score.
	// Todo: We need to account for promotion aka material gain, therefore
	// we should calculate material gap between teams.
	// Note that if there isn't a single move that makes a material gain,
	// a random move will be chosen.
	bool greedyMove(bool _team)
	{
		generateSubs();
		
		int materialGap = 0;
		int bestScore = 0;
		Vector <int> vBestIndex;
		
		for (int i=0;i<vSubstates.size();++i)
		{
			// calculate material gap.
			materialGap = vSubstates(i)->getMaterialScore(_team) - vSubstates(i)->getMaterialScore(!_team);
			
			if ( vBestIndex.size() == 0 || materialGap > bestScore)
			{
				bestScore = materialGap;
				vBestIndex.clear();
				vBestIndex.push(i);
			}
			// we found another good move of equivalent value
			else if ( materialGap == bestScore)
			{
				vBestIndex.push(i);
			}
		}
		
		if (vBestIndex.size()==0)
		{
			std::cout<<"Couldn't find a material move\n";
			return false;
		}
		int chosenIndex = vBestIndex(rng.rand(vBestIndex.size()-1));
		*this = *vSubstates( chosenIndex );
		clearSubs();
		return true;
	}
	
	// find a move which results in best material gain over 2 turns...
	bool materialDepthMove(bool _team, int depth)
	{
		generateSubs();
		
		for (int i=0; i<vSubstates.size(); ++i)
		{
		}
		
		return false;
	}
	
	char boardStatus()
	{
		// Check: If some of the next moves result in loss of king.
		// Checkmate: If no substate avoids check
		// check for stalemate endgame here (not enough pieces to checkmate)
		
		if ( getNPieces(WHITE) == 1 && getNPieces(BLACK) == 1)
		{
			status!=STALEMATE_MATERIAL;
		}
		
		//stalemate: lack of material
		
		generateSubs();

		// If we find a move which can capture the king, the board state is in check.
		for (int i=0;i<vSubstates.size();++i)
		{
			if (vSubstates(i)->hasKing(BLACK) == false)
			{
				status |= BLACK_CHECK;
				status |= BLACK_NO_KING;
			}
			if (vSubstates(i)->hasKing(WHITE) == false)
			{
				status |= WHITE_CHECK;
				status |= WHITE_NO_KING;
			}
		}

		for (int i=0;i<vSubstates2.size();++i)
		{
			if (vSubstates2(i)->hasKing(BLACK) == false)
			{
				status |= BLACK_CHECK;
				status |= BLACK_NO_KING;
			}
			if (vSubstates2(i)->hasKing(WHITE) == false)
			{
				status |= WHITE_CHECK;
				status |= WHITE_NO_KING;
			}
		}
		return status;
	}
	
	bool hasState(unsigned char state)
	{
		return ((status & state) == state);
	}
	
	// calculate the score for this board state based on material.
	int getMaterialScore(bool _team)
	{
		int score = 0;
		
		// get all movable pieces
		auto vPiece = getAllPieces(_team);
		
		// sum material value
		for (auto const& element : *vPiece)
		{
			score += element->materialValue;
		}
		delete vPiece;

		return score;
	}

	bool hasKing(bool _team)
	{	
		// get all movable pieces
		Vector <Piece*>* vPiece = getAllPieces(_team);
		
		if (vPiece == 0)
		{
			// null vector means 0 pieces
			return false;
		}
		
		// check if vector contains a king
		for (int i=0;i<vPiece->size();++i)
		{
			if ((*vPiece)(i)->getShortName() == 'k' || (*vPiece)(i)->getShortName() == 'K')
			{
				delete vPiece;
				return true;
			}
		}
		delete vPiece;
		return false;
	}
	
	// generate all possible moves and store in memory
	// this should be done automatically by the Board class when required.
	void generateSubs()
	{
		// generate moves if current side moves
		if ( vSubstates.size() == 0 )
		{
			// get all movable pieces
			auto vPiece = getAllPieces(sideToMove);

			// get all moves for all pieces
			for (int i2=0;i2<vPiece->size();++i2)
			{
				addAllMovesFrom((*vPiece)(i2),&vSubstates);
			}
			for (int i=0;i<vSubstates.size();++i)
			{
				vSubstates(i)->sideToMove=!sideToMove;
			}
			delete vPiece;
		}
		// generate moves if opponent moves again (to determine check)
		if ( vSubstates2.size() == 0 )
		{
			// get all movable pieces
			auto vPiece = getAllPieces(!sideToMove);

			// get all moves for all pieces
			for (int i2=0;i2<vPiece->size();++i2)
			{
				addAllMovesFrom((*vPiece)(i2),&vSubstates2);
			}
			for (int i=0;i<vSubstates2.size();++i)
			{
				vSubstates2(i)->sideToMove=!sideToMove;
			}
			delete vPiece;
		}
	}
	void clearSubs()
	{
		// recursively delete all substates
		vSubstates.clearPtr();
		vSubstates2.clearPtr();
	}
};

int Board::STATIC_ID = 0;
