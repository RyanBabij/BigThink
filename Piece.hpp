class Piece
{
	std::string name;
	char shortName;

	public:
	bool hasMoved;
	bool isPromoted;
	bool team;
	
	int x,y;

	Piece(std::string _name, char _shortName, bool _team, int _x, int _y)
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
	char getShortName()
	{
		return shortName;
	}
};
