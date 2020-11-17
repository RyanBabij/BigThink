#include <Math/Random/RandomLehmer.hpp>

#include <time.h>

#define WHITE true
#define BLACK false

//RandomLehmer rng;

class Board
{
	// 0 = empty space
	
	// 244 = white pawn
	// r = white rook
	// n = white knight
	// b = white bishop
	// q = white queen
	// k = white king
	
	// 245 = black pawn
	// R = black rook
	// N = black knight
	// B = black bishop
	// Q = black queen
	// K = black king

	bool sideToMove;
	Vector <Board*> vSubstates;

	
	std::string transitionName;
	
	
	public:
	
	Piece* aBoard [8][8];
	
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
	}
	
	Board()
	{
		sideToMove=WHITE;
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				aBoard[x][y] = 0;
			}
		}
		transitionName="";
	}
	Board(const Board& board) // copy constructor 
	{
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
	}
	
	// Equals operator: Identify identical board states for pruning?
	
	//Assignment operator: Copy board state.
	Board operator=(const Board& board)
	{
		
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

		return board;
	}

	bool move (int x1, int y1, int x2, int y2)
	{
		// move piece from (x1,y1) to (x2,y2). Return false if invalid move.
		// assume any target piece is captured.
		if ( isSafe(x1,y1) == false || isSafe(x2,y2) == false ||
		aBoard[x1][y1] == 0 )
		{
			return false;
		}
		
		Piece* movePiece = aBoard[x1][y1];
		aBoard[x1][y1]=0;
		if (aBoard[x2][y2] != 0 )
		{
			delete aBoard[x2][y2];
		}
		aBoard[x2][y2] = movePiece;
		movePiece->x = x2;
		movePiece->y = y2;
		
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
			aBoard[i][1] = new Piece ("pawn", 244, WHITE, i, 1, 1);
			aBoard[i][6] = new Piece("pawn", 245, BLACK, i, 6, 1);
		}
		
		aBoard[0][0] = new Piece ("rook", 'r', WHITE, 0,0,5);
		aBoard[7][0] = new Piece ("rook", 'r', WHITE, 7,0,5);
		aBoard[0][7] = new Piece ("rook", 'R', BLACK, 0,7,5);
		aBoard[7][7] = new Piece ("rook", 'R', BLACK, 7,7,5);
		
		aBoard[1][0] = new Piece ("knight", 'n', WHITE, 1,0,3);
		aBoard[6][0] = new Piece ("knight", 'n', WHITE, 6,0,3);
		aBoard[1][7] = new Piece ("knight", 'N', BLACK, 1,7,3);
		aBoard[6][7] = new Piece ("knight", 'N', BLACK, 6,7,3);
		
		aBoard[2][0] = new Piece ("bishop", 'b', WHITE, 2,0,3);
		aBoard[5][0] = new Piece ("bishop", 'b', WHITE, 5,0,3);
		aBoard[2][7] = new Piece ("bishop", 'B', BLACK, 2,7,3);
		aBoard[5][7] = new Piece ("bishop", 'B', BLACK, 5,7,3);
		
		aBoard[3][0] = new Piece ("queen", 'q', WHITE, 3,0,9);
		aBoard[4][0] = new Piece ("king", 'k', WHITE, 4,0,1000);
		aBoard[3][7] = new Piece ("queen", 'Q', BLACK, 3,7,9);
		aBoard[4][7] = new Piece ("king", 'K', BLACK, 4,7,1000);
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
					//std::cout<<"adding move\n";
					vBoard->push(subBoard);
					
					// can it move up 2 spaces?
					if (piece->y==1 && isSafe(piece->x,piece->y+2))
					{
						if ( aBoard[piece->x][piece->y+2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y+2);
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
			
			// promote to queen if it reaches the end of the board.
			if (piece->y == 7)
			{
				piece->promote();
			}
		}
		// black pawn, can move down 1 space, attack diagonally.
		if (piece->getShortName() == 245)
		{
			// can it move up 1 space?
			if (isSafe(piece->x,piece->y-1))
			{
				if ( aBoard[piece->x][piece->y-1] == 0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,piece->y-1);
					vBoard->push(subBoard);
					
					// can it move up 2 spaces?
					if (piece->y==6 && isSafe(piece->x,piece->y-2))
					{
						if ( aBoard[piece->x][piece->y-2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y-2);
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
			// promote to queen if it reaches the end of the board.
			if (piece->y == 0)
			{
				piece->promote();
			}
		}
		
		
		// knight, moves in L shape
		if (piece->getShortName() == 'n' || piece->getShortName() == 'N')
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
		if (piece->getShortName() == 'r' || piece->getShortName() == 'R')
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
		if (piece->getShortName() == 'B' || piece->getShortName() == 'b')
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
		if (piece->getShortName() == 'Q' || piece->getShortName() == 'q')
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
		if (piece->getShortName() == 'K' || piece->getShortName() == 'k')
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
		}
		
		return vPieces;
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
		
		if ( vMove->size() > 0 )
		{
			return vMove;
		}
		return 0;
	}
	
	bool randomMove (bool _team)
	{
		std::string strTeam = "";
		if (_team == WHITE)
		{
			std::cout<<"*** Random move: WHITE ***\n";
			strTeam = "white";
		}
		else if (_team == BLACK)
		{
			std::cout<<"*** Random move: BLACK ***\n";
			strTeam = "black";
		}
		
		Vector <Board*> * vMove = getAllMoves(_team);
		
		if (vMove == 0)
		{
			std::cout<<"Error: No moveset vector returned.\n";
			return false;
		}
		
		if (vMove->size() == 0)
		{
			std::cout<<"Error: No "<<strTeam<<" moves found.\n";
			return false;
		}
		
		*this = *(*vMove)(rng.rand(vMove->size()));
			
		return true;
	}
	
	// Pick the best immediate move to reduce enemy material score.
	// Todo: We need to account for promotion aka material gain, therefore
	// we should calculate material gap between teams.
	bool materialMove(bool _team)
	{
		std::string strTeam = "";
		if (_team == WHITE)
		{
			std::cout<<"*** Material move: WHITE ***\n";
			strTeam = "white";
		}
		else if (_team == BLACK)
		{
			std::cout<<"*** Material move: BLACK ***\n";
			strTeam = "black";
		}
		
		// get all movable pieces
		auto vPiece = getAllPieces(_team);

		if (vPiece == 0)
		{
			std::cout<<"Error: No pieces found.\n";
			return false;
		}
		
		Vector <Board*> * vMove = new Vector <Board*>;
		// get all moves for all pieces
		for (int i2=0;i2<vPiece->size();++i2)
		{
			addAllMovesFrom((*vPiece)(i2),vMove);
		}
		std::cout<<"Found "<<vMove->size()<<" moves.\n";
		
		// pick move which lowers opponent's score the most.
		
		int currentOpponentScore = getMaterialScore(!_team);
		Board* bestState = 0;
		
		if (vMove->size() == 0)
		{
			std::cout<<"Error: No "<<strTeam<<" moves found.\n";
			return false;
		}
		
		// Store list of moves of equal score, so we can select on randomly
		Vector <Board*> vBestMoves;
		int bestScore = 0;
		int bestIndex = -1;
		int materialGap = 0;
		for (int i=0;i<vMove->size();++i)
		{
			// calculate material gap.
			materialGap = (*vMove)(i)->getMaterialScore(_team) - (*vMove)(i)->getMaterialScore(!_team);
			
			if ( bestIndex == -1 || materialGap > bestScore)
			{
				bestScore = materialGap;
				bestIndex = i;
				vBestMoves.clear();
				vBestMoves.push( (*vMove)(i) );
			}
			else if ( materialGap == bestScore)
			{
				bestIndex = i;
				vBestMoves.push( (*vMove)(i) );
			}
		}
		
		if (bestIndex != -1)
		{
			*this = *vBestMoves( rng.rand(vBestMoves.size()-1) );		
		}
		
		vBestMoves.clear();
		
		for (int i=0;i<vMove->size();++i)
		{
			delete (*vMove)(i);
		}
		
		return false;
	}
	
	bool materialDepthMove(int depth)
	{
		return false;
	}
	
	
	// 0000 0000
	// 1000 0000 - WHITE check
	// 1100 0000 - WHITE checkmate
	// 0100 0000 - WHITE stalemate
	// 0000 1000 - BLACK check
	// 0000 1100 - BLACK checkmate
	// 0000 0100 - BLACK stalemate
	
	char boardStatus(bool _team)
	{
		// need to check
		// check
		// checkmate
		// stalemate
		
		Vector <Board*> * vMove = getAllMoves(!_team);
		
		for (int i=0;i<vMove->size();++i)
		{
			if ( (*vMove)(i)->hasKing(_team) == false )
			{
				std::cout<<"Found check\n";
				// we are in check / checkmate
				for (int i2=0;i2<vMove->size();++i2)
				{
					delete (*vMove)(i2);
				}
				vMove->clear();
				
				return 1;
			}
		}
		
		for (int i=0;i<vMove->size();++i)
		{
			delete (*vMove)(i);
		}
		vMove->clear();
				
		return 0;
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

		return score;
	}
	
	bool hasKing(bool _team)
	{
		// get all movable pieces
		auto vPiece = getAllPieces(_team);
		
		// check if vector contains a king
		
		for (auto const& element : *vPiece)
		{
			if (element->getShortName() == 'k' || element->getShortName() == 'K')
			{
				return true;
			}
		}
		return false;
	}
};