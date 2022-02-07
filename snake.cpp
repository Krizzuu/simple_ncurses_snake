#include "snake.h"

void CSnake::reset() {
	pause = false;
	help = false;
	died = false;
	course = KEY_RIGHT;
	speed = startSpeed;
	level = 0;
	int headPosX = rand() % (geom.size.x - 5) + 1;
	int headPosY = rand() % (geom.size.y - 3) + 1;
	segments.clear();
	segments.push_back(CPoint(headPosX, headPosY));
	segments.push_back(CPoint(headPosX - 1, headPosY));
	segments.push_back(CPoint(headPosX - 2, headPosY));
	generateFood();
	paint();
}

void CSnake::generateFood() {
	CPoint candidate;
	do {
		candidate = CPoint((rand() % (geom.size.x - 2)) + 1, (rand() % (geom.size.y - 2)) + 1);
		bool s = true;
		for (auto &part:segments) {
			if (part.x == candidate.x && candidate.y == part.x) {
				s = false;
				break;
			}
		}
		if (s) break;
	} while (true);
	food = candidate;
}

bool CSnake::eat() {
	bool ate = false;
	for (auto &part : segments)
	{
		if (part.x == food.x && part.y == food.y)
		{
			level++;
			ate = true;
			if (speed > 1.0f) speed -= 0.5f;
			break;
		}
	}
	if (ate)
	{
		generateFood();
		return true;
	}
	return false;
}

bool CSnake::move() {
	if (pause)
	{
		return true;
	}
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
	if (!pause && !move()) {
		died = true;
		pause = true;
	}
	gotoyx(food.y + geom.topleft.y, food.x + geom.topleft.x);
	printc('O');
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

void CSnake::drawDead() {
	int x = geom.topleft.x, y = geom.topleft.y;
	gotoyx(y + 1, x + 1);
	printl("GAME OVER, your score: %d", level);
}

void CSnake::drawPause() {
	int x = geom.topleft.x, y = geom.topleft.y;
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
}

void CSnake::drawHelp() {
	int x = geom.topleft.x, y = geom.topleft.y;
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
}
