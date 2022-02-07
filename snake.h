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
	bool pause = true;
	bool help = true;
	bool died = false;
	// const expr
	static constexpr float startSpeed = 20.0f;
#ifdef USE_COLOR
	chtype headChar;
	chtype segmentChars[2];
#endif
	// move stuff
	int course = KEY_RIGHT;
	int level = 0;
	float speed = startSpeed;
	float ticks = 0;
	CPoint food;
	vector <CPoint> segments;

	void reset();
	void generateFood();

	bool eat();
	bool move();

	void draw();
	void drawDead();
	void drawPause();
	void drawHelp();

public:
	CSnake(CRect r, char _c = ' ') :
			CFramedWindow(r, _c) {
		srand(time(NULL));
		reset();
		help = true;
		pause = true;
#ifdef USE_COLOR
		init_pair( 1, COLOR_BLACK, COLOR_YELLOW );
		init_pair( 2, COLOR_BLACK, COLOR_GREEN );
		init_color( 100, 306, 510, 306 );
		init_pair( 3, COLOR_BLACK, 100 );
		headChar = '*' | COLOR_PAIR(1);
		segmentChars[0] = '+' | COLOR_PAIR(2);
		segmentChars[1] = '+' | COLOR_PAIR(3);
#endif
		draw();
	}

	bool handleEvent(int key) {
		bool refreshed = false;
		if (!pause && key == ERR) {
			ticks += 0.8f;
			if (speed <= ticks) {
				ticks = 0;
				refreshed = true;
			}
		}
		if (!died && tolower(key) == 'p') {
			pause = !pause;
			if (!pause) {
				help = false;
			}
			return true;
		}
		if (tolower(key) == 'h' && pause) {
			help = !help;
			return true;
		}
		if (pause && tolower(key) == 'q') {
			exit(0);
		}
		if (tolower(key) == 'r') {
			reset();
			return true;
		}
		if (!died && !pause && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT) ) {
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
			pause = true;
		}
		return refreshed || CFramedWindow::handleEvent(key);
	}

	void paint() {
		CFramedWindow::paint();
		draw();
		if (!died) {
			gotoyx(geom.topleft.y - 1, geom.topleft.x);
			printl("| LEVEL: %d |", level);
			if (speed == 1) {
				printl("  FULL THROTTLE!");
			}
			if (pause) {
				if (help) drawHelp();
				else drawPause();
			}
		} else {
			drawDead();
		}
	}
};

#endif