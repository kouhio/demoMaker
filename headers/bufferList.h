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

struct Item {
		void (*debug)(struct Item *buf);
		int (*parseBufferItem)(struct Item *buf, char *data);
		void (*updateStep)(struct Item *buf, int value);
		int (*activate)(struct Item *buf);
		void (*free)(struct Item *buf);

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
struct Item *initItem(struct Item *buf, int init);

struct bufferItem {
		struct Item *(*getNextItem)(struct bufferItem *buf);
		int (*addNewItemToList)(struct bufferItem *buf, char *item);
		int (*parseBufferItem)(struct bufferItem *buf, char *data, int item);
		int (*activateAllItems)(struct bufferItem *buf);
		struct Item *(*getItemByNumber)(struct bufferItem *buf, int item);
		void (*setLastItem)(struct bufferItem *buf);
		void (*free)(struct bufferItem *buf);

		struct Item *itemList;
		struct Item *current;
		int count;
};
struct bufferItem *initBufferItem(struct bufferItem *buf, int init);

struct bufferList {
		struct bufferItem *(*getNextBufferItem)(struct bufferList *buf);
		struct bufferItem *(*getBufferStep)(struct bufferList *buf, int step);
		char (*getBufferLetter)(struct bufferList *buf);
		int (*addNewBufferItem)(struct bufferList *buf, char *data, int step, int item);
		unsigned long long (*getTrigger)(struct bufferList *buf);
		int (*handleStepItems)(struct bufferList *buf);
		int (*getItemByNumber)(struct bufferList *buf, int item);
		void (*free)(struct bufferList *buf);

		ImageTriggers trigger;
		int step;
		char letter;
		unsigned long long currentPlaytime;
		int currentLoop;

		struct bufferItem *letterList;
		struct bufferItem *current;
		int count;
};
struct bufferList *initBufferList(struct bufferList, char c, int init);

struct buffer {
		struct bufferList *(*findBufferItem)(struct buffer *buf, char c);
		int (*activateCurrentItem)(struct buffer *buf);
		struct bufferList *(*getNextBufferItem)(struct buffer *buf);
		int (*addItemToBuffer)(struct buffer *buf, char *data, char button, int step, int item);
		void (*free)(struct buffer *buf);

		struct bufferList *list;
		struct bufferList *current;
		int count;
};
struct buffer *initBuffer(SDL_Surface *screen);

void freeCompleteBuffer(struct buffer *buf);

int readIniFile(std::string path, struct buffer *Buffer);
int handleNewButton(FILE *fd, struct buffer *Buffer, char button, int step);

#endif



