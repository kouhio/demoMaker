#ifndef __BUFFERLIST_H__
#define __BUFFERLIST_H__


#include <string>
#include <vector>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

extern SDL_Surface *mainScreen;

enum ImageHandlers {
	UNINITIALIZED = 0,
	DRAW_PICTURE,
	PLAY_VIDEO,
	ZOOM_IMAGE_IN,
	ZOOM_IMAGE_OUT,
	ROTATE_IMAGE,
	ZOOM_AND_ROTATE_IMAGE,
	FADE_IMAGE_IN,
	FADE_IMAGE_OUT,
	FADE_IMAGE_TO_IMAGE,
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_LEFT_COMPLETE,
	SLIDE_RIGHT_COMPLETE,
	BOX_PICTURE_IN,
	RANDOM_BOXING,
	SWIPE_PICTURE_TOP,
	TEXT_SCROLLER,
};

enum ImageTriggers {
	LOOP = 0,
	TIMER,
	VIDEND,
	ONCE,
};

class Item {
	public:
		Item();
		~Item();
		void debug(void);
		int parseBufferItem(char *data);
		void updateStep(int value);
		int activate(void);

		int step;
		int steps;
		ImageHandlers action;
		unsigned long long playtime;
		int loop;
		int once;
		int videoOpen;
		int fontSize;
		std::string videoPath;
		std::string image1Path;
		std::string image2Path;
		std::string scrollerText;

		SDL_Surface *image1;
		SDL_Surface *image2;
		SDL_Surface *scrollText;
		ImageTriggers trigger;
		unsigned long long triggerTime;

		std::string name;
		TTF_Font *font;
		long unsigned int color;

		int itemLoop;
		int x;
		int y;
		int fill;
		int count;
};

class bufferItem {
	public:
		bufferItem();
		bufferItem(std::string item);
		~bufferItem();

		std::vector<Item>::iterator getNextItem(void);
		int addNewItemToList(char *item);
		int parseBufferItem(char *data, int item);
		int activateAllItems(void);
		int getItemByNumber(int item);
	private:
		void setLastItem(void);
		
		std::vector<Item>::iterator currentListItem;
		std::vector<Item> itemList;
};

class bufferList {
	public:
		bufferList();
		bufferList(char c);
		~bufferList();

		int getNextBufferItem(void);
		int getBufferStep(int step);
		char getBufferLetter(void);
		int addNewBufferItem(char *data, int step, int item);
		unsigned long long getTrigger(void);
		int handleStepItems(void);
		int getItemByNumber(int item, bufferItem *buf);
	private:
		ImageTriggers trigger;
		int step;
		char letter;
		unsigned long long currentPlaytime;
		int currentLoop;

		std::vector<bufferItem>::iterator currentListItem;
		std::vector<bufferItem> letterList;
};

class buffer {
	public:
		buffer();
		~buffer();

		int findBufferItem(char c);
		int activeCurrentItem(void);
		int getNextBufferItem(void);
		int addItemToBuffer(char *data, char button, int step, int item);
	private:
		std::vector<bufferList>::iterator ipos;
		std::vector<bufferList> list;
};

int readIniFile(std::string path, class buffer *Buffer);
int handleNewButton(FILE *fd, class buffer *Buffer, char button, int step);

#endif



