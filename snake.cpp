#include "snake.h"

void CSnake::reset() {
	windowState = windowStates::gaming;
	died = false;
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
		for (auto &part:segments) {
			if (part.x == place.x && place.y == part.x)
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
		if (part.x == food.x && part.y == food.y)
		{
			score++;
			ate = true;
			if (speed > 1.0f) speed -= 0.5f;
			break;
		}
	}
	if (ate)
	{
		if ( segments.size() == (unsigned int)(( geom.size.x - 1 ) * ( geom.size.y - 1 ))  )
		{
			died = true;
		}
		spawnFood();
		return true;
	}
	return false;
}

bool CSnake::move() {

	CPoint tail = segments[segments.size() - 1];
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
		if (segments[0].x == segments[i].x && segments[0].y == segments[i].y) {
			return false;
		}
	}
	if (segments[0].x == 0)
	{
		segments[0].x = geom.size.x - 2;
	}
	if (segments[0].x == geom.size.x - 1)
	{
		segments[0].x = 1;
	}
	if (segments[0].y == 0)
	{
		segments[0].y = geom.size.y - 2;
	}
	if (segments[0].y == geom.size.y - 1)
	{
		segments[0].y = 1;
	}
	if (eat())
	{
		segments.push_back(tail);
	}
	return true;
}

void CSnake::draw() {
	if ( ( windowState != windowStates::paused && windowState != windowStates::help ) && !move()) {
		died = true;
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
	if (windowState != windowStates::paused && key == ERR) {
		ticks += 0.8f;
		if (speed <= ticks) {
			ticks = 0;
			refreshed = true;
		}
	}
	if (!died && tolower(key) == 'p') {
		if ( windowState == windowStates::paused || windowState == windowStates::help )
		{
			windowState = windowStates::gaming;
		}
		else
		{
			windowState = windowStates::paused;
		}
		return true;
	}
	if (tolower(key) == 'h') {
		if ( windowState == windowStates::paused || windowState == windowStates::gaming )
		{
			windowState = windowStates::help;
		}
		else
		{
			windowState = windowStates::gaming;
		}
		return true;
	}
	if (windowState == windowStates::paused && tolower(key) == 'q') {
		exit(0);
	}
	if (tolower(key) == 'r') {
		reset();
		return true;
	}
	if (!died && windowState != windowStates::paused && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT) ) {
		if ((key == KEY_UP && course != KEY_DOWN)
			|| (key == KEY_DOWN && course != KEY_UP)
			|| (key == KEY_LEFT && course != KEY_RIGHT)
			|| (key == KEY_RIGHT && course != KEY_LEFT) )
		{
			course = key;
			ticks = 0;
		}
		return true;
	}
	if (key == '\t')
	{
		windowState = windowStates::paused;
	}
	return refreshed || CFramedWindow::handleEvent(key);
}

void CSnake::paint() {
	CFramedWindow::paint();
	draw();
	if (!died) {
		gotoyx(geom.topleft.y - 1, geom.topleft.x);
		printl("| SCORE: %d |", score);
		if (speed == 1) {
			printl("  FULL THROTTLE!");
		}
		int x, y;
		switch ( windowState ) {
		case windowStates::help:
			x = geom.topleft.x, y = geom.topleft.y;
			gotoyx(y + 2, x + 2);
			printl("Use arrows to move snake");
			gotoyx(y + 4, x + 2);
			printl("Eat to grow and get points");
			gotoyx(y + 5, x + 2);
			printl("But don't eat yourself !");
			gotoyx(y + 7, x + 2);
			printl("press 'p' to play | 'r' to replay");
			gotoyx(y + 8, x + 2);
			printl("'h' for help");
			break;
		case windowStates::paused:
				x = geom.topleft.x, y = geom.topleft.y;
				gotoyx(y + 2, x + 3);
				printl("h - toggle help information");
				gotoyx(y + 3, x + 3);
				printl("p - toggle play/pause mode");
				gotoyx(y + 4, x + 3);
				printl("r - restart game");
				gotoyx(y + 5, x + 3);
				printl("q - quit");
				gotoyx(y + 6, x + 3);
				printl("arrows - move snake (in play mode)");
				gotoyx(y + 7, x + 12);
				printl(" or move window (in pause mode)");
				break;
		default:
			break;
		}
	}
	else
	{
		int x = geom.topleft.x, y = geom.topleft.y;
		gotoyx(y + 1, x + 1);
		printl("Game Over, your score: %d", score);
	}
}

