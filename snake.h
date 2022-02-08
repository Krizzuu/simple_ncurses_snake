#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <vector>
#include <cstdlib>
#include <ctime>

#include "winsys.h"
#include "cpoint.h"
#include "screen.h"

class CSnake : public CFramedWindow {
private:
	// states
	enum class windowStates {
		gaming,
		paused,
		help
	};
	windowStates windowState;
	bool died = false;

	// const expr
	static constexpr float startSpeed = 20.0f;
#ifdef USE_COLOR
	chtype headChar;
	chtype segmentChars[2];
#endif

	// move stuff
	int course = KEY_RIGHT;
	int score = 0;
	float speed = startSpeed;
	float ticks = 0;
	CPoint food;
	vector <CPoint> segments;

	// Window actions
	void reset();
	void spawnFood();
	void draw();

	// Snake itself actions
	bool eat();
	bool move();


public:
	CSnake(CRect r, char _c = ' ') :
			CFramedWindow(r, _c) {
		srand(time(NULL));
		reset();
		windowState = CSnake::windowStates::help;
#ifdef USE_COLOR
		init_pair( 1, COLOR_BLACK, COLOR_YELLOW );
		init_pair( 2, COLOR_BLACK, COLOR_GREEN );
		init_color( 100, 306, 510, 306 );
		init_pair( 3, COLOR_BLACK, 100 );
		init_pair( 4, COLOR_BLACK, COLOR_RED );
		headChar = ' ' | COLOR_PAIR(1);
		segmentChars[0] = ' ' | COLOR_PAIR(2);
		segmentChars[1] = ' ' | COLOR_PAIR(3);
#endif
		draw();
	}

	bool handleEvent(int key);
	void paint();
};

#endif