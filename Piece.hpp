class Piece
{
	std::string name;
	unsigned char shortName;

	public:
	bool hasMoved;
	bool isPromoted;
	bool team;
	
	int x,y;

	Piece(std::string _name, unsigned char _shortName, bool _team, int _x, int _y)
	{
		name = _name;
		shortName = _shortName;
		hasMoved=false;
		isPromoted=false;
		team=_team;
		x=_x;
		y=_y;
	}
	std::string getName()
	{
		return name;
	}
	unsigned char getShortName()
	{
		return shortName;
	}
};
