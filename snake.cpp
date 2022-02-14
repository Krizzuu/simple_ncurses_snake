#include "snake.h"

void CSnake::reset() {
	windowState = windowStates::gaming;
	dead = false;
	course = KEY_RIGHT;
	speed = startSpeed;
	score = 0;
	int headPosX = rand() % (geom.size.x - 5) + 1;
	int headPosY = rand() % (geom.size.y - 3) + 1;
	segments.clear();
	segments.emplace_back( headPosX, headPosY);
	segments.emplace_back( headPosX - 1, headPosY );
	segments.emplace_back( headPosX - 2, headPosY );
	spawnFood();
	paint();
}

void CSnake::spawnFood() {
	CPoint place;
	do {
		place = CPoint((rand() % (geom.size.x - 2)) + 1, (rand() % (geom.size.y - 2)) + 1);
		bool occupied = true;
		for (auto &part:segments)
		{
			if ( place == part )
			{
				occupied = false;
				break;
			}
		}
		if (occupied) break;
	} while (true);
	food = place;
}

bool CSnake::eat() {
	bool ate = false;
	for (auto &part : segments)
	{
		if ( part == food )
		{
			score++;
			ate = true;
			if (speed > 1.0f) speed *= 0.96f;
			break;
		}
	}
	if (ate)
	{
		if ( segments.size() == (unsigned int)(( geom.size.x - 1 ) * ( geom.size.y - 1 ))  )
		{
			dead = true;
		}
		else
		{
			spawnFood();
		}
		return true;
	}
	return false;
}

bool CSnake::move() {

	CPoint newTail = segments[segments.size() - 1];
	for (unsigned long i = segments.size() - 1; i > 0; i--)
	{
		segments[i] = segments[i - 1];
	}
	switch( course )
	{
	case KEY_UP:
		segments[0] += CPoint(0, -1);
		break;
	case KEY_DOWN:
		segments[0] += CPoint(0, 1);
		break;
	case KEY_LEFT:
		segments[0] += CPoint(-1, 0);
		break;
	case KEY_RIGHT:
		segments[0] += CPoint(1, 0);
		break;
	}
	for (uint i = 1; i < segments.size(); i++)
	{
		if ( segments[0] == segments[i] ) {
			return false;
		}
	}
	if (segments[0].x == 0)
	{
		segments[0].x = geom.size.x - 2;
	}
	else if (segments[0].x == geom.size.x - 1)
	{
		segments[0].x = 1;
	}
	if (segments[0].y == 0)
	{
		segments[0].y = geom.size.y - 2;
	}
	else if (segments[0].y == geom.size.y - 1)
	{
		segments[0].y = 1;
	}
	if (eat())
	{
		segments.push_back(newTail);
	}
	return true;
}

void CSnake::draw() {
	if ( windowState == windowStates::gaming && !move() ) {
		dead = true;
		windowState = windowStates::paused;
	}
	gotoyx(food.y + geom.topleft.y, food.x + geom.topleft.x);
	#ifdef USE_COLOR
	printc(' ' | COLOR_PAIR( 4 ) );
	#else
	printc('O');
	#endif
	int k = 0;
	for (uint i = 1; i < segments.size(); i++, k++)
	{
		gotoyx(segments[i].y + geom.topleft.y, segments[i].x + geom.topleft.x);
	#ifdef USE_COLOR
		if ( k == 2 )
		{
			k = 0;
		}
		printc( segmentChars[k] );
	#else
		printc('+');
	#endif
	}
	gotoyx(segments[0].y + geom.topleft.y, segments[0].x + geom.topleft.x);
	#ifdef USE_COLOR
	printc(headChar);
	#else
	printc('*');
	#endif
}

bool CSnake::handleEvent(int key) {
	bool refreshed = false;
	switch ( key )
	{
	case ERR:
		if ( windowState == windowStates::gaming )
		{
			ticks += 1.0f;
			if (speed <= ticks) {
				ticks = 0;
				refreshed = true;
			}
		}
		break;
	case '\t':
		windowState = windowStates::paused;
		break;
	case 'p':
	case 'P':
		if ( !dead && windowState != windowStates::gaming )
		{
			windowState = windowStates::gaming;
		}
		else
		{
			windowState = windowStates::paused;
		}
		return true;
	case 'h':
	case 'H':
		if ( !dead && windowState != windowStates::help )
		{
			windowState = windowStates::help;
		}
		else
		{
			windowState = windowStates::gaming;
		}
		return true;
	case 'q':
	case 'Q':
		if ( windowState != windowStates::gaming )
		exit(0);
	case 'r':
	case 'R':
		if ( windowState == windowStates::gaming || dead )
		{
			reset();
			windowState = windowStates::help;
		}
		return true;
	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
		if ( !dead && windowState == windowStates::gaming )
		{
			if ( (key == KEY_UP && course != KEY_DOWN)
				|| (key == KEY_DOWN && course != KEY_UP)
				|| (key == KEY_LEFT && course != KEY_RIGHT)
				|| (key == KEY_RIGHT && course != KEY_LEFT) )
			{
				course = key;
				ticks = 0;
			}
			return true;
		}
	default:
		break;
	}
	return refreshed || CFramedWindow::handleEvent(key);
}

void CSnake::paint() {
	CFramedWindow::paint();
	draw();
	if (!dead) {
		gotoyx(geom.topleft.y - 1, geom.topleft.x);
		printl("| SCORE: %d |", score);

		if (windowState == windowStates::help)
		{
			int x = geom.topleft.x, y = geom.topleft.y;
			gotoyx(y + 2, x + 2);
			printl("Use arrows to move snake");
			gotoyx(y + 4, x + 2);
			printl("Eat to grow and get points");
			gotoyx(y + 5, x + 2);
			printl("But don't eat yourself !");
			gotoyx(y + 7, x + 2);
			printl("press 'p' to play");
			gotoyx(y + 8, x + 8);
			printl("'r' to replay");
			gotoyx(y + 9, x + 8);
			printl("'h' for help");
		}
	}
	else
	{
		int x = geom.topleft.x, y = geom.topleft.y;
		gotoyx(y + 1, x + 1);
		printl("Game Over, your score: %d", score);
	}
}

