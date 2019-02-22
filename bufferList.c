#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_gfxPrimitives.h"
#include "SDL/SDL_rotozoom.h"

#include "bufferList.h"
#include "draw.h"
#include "graph.h"
#include "video.h"
#include "dynamicPlatform.h"
#include "filesys.h"

SDL_Surface *mainScreen;
static int tester = 0;

ImageTriggers getTrigger(char *data) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(!strncmp(data, "once", 4)) {
		return ONCE;
	} else if(!strncmp(data, "video", 5)) {
		return VIDEND;
	} else if(!strncmp(data, "timer", 5)) {
		return TIMER;
	} else if(!strncmp(data, "loop", 4)) {
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return LOOP;
}

ImageHandlers getEffect(char *data) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(!strncmp(data, "picture", 7)) {
		return DRAW_PICTURE; 
	} else if(!strncmp(data, "video", 5)) {
		return PLAY_VIDEO;
	} else if(!strncmp(data, "zoom in", 7)) {
		return ZOOM_IMAGE_IN;
	} else if(!strncmp(data, "zoom out", 8)) {
		return ZOOM_IMAGE_OUT;
	} else if(!strncmp(data, "rotate", 6)) {
		return ROTATE_IMAGE;
	} else if(!strncmp(data, "zoom and rotate", 15)) {
		return ZOOM_AND_ROTATE_IMAGE;
	} else if(!strncmp(data, "fade in", 6)) {
		return FADE_IMAGE_IN;
	} else if(!strncmp(data, "fade out", 7)) {
		return FADE_IMAGE_OUT;
	} else if(!strncmp(data, "fade image", 9)) {
		return FADE_IMAGE_TO_IMAGE;
	} else if(!strncmp(data, "left", 4)) {
		return SLIDE_LEFT;
	} else if(!strncmp(data, "right", 5)) {
		return SLIDE_RIGHT;
	} else if(!strncmp(data, "slide left", 10)) {
		return SLIDE_LEFT_COMPLETE;
	} else if(!strncmp(data, "slide right", 11)) {
		return SLIDE_RIGHT_COMPLETE;
	} else if(!strncmp(data, "box", 3)) {
		return BOX_PICTURE_IN;
	} else if(!strncmp(data, "random box", 10)) {
		return RANDOM_BOXING;
	} else if(!strncmp(data, "swipe top", 9)) {
		return SWIPE_PICTURE_TOP;
	} else if(!strncmp(data, "scroller", 8)) {
		return TEXT_SCROLLER;
	}
	std::cout << data << std::endl;
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return UNINITIALIZED;
}

/*******************************************************************************/

void _debug(struct Item *buf) {
	std::cout << "--> Step:" << buf->step << " steps:" << buf->steps;
	std::cout << " action:" << buf->action << " trigger:" << buf->trigger << " time:" << buf->triggerTime;
	std::cout << " playtime:" << buf->playtime << " loop:" << buf->loop;
	std::cout << " once:" << buf->once << " video:" << buf->videoOpen << " " << buf->name;
	std::cout << " font:" << buf->fontSize << " color:" << buf->color << " loop2:" << buf->itemLoop;
	std::cout << " x:" << buf->x << " y:" << buf->y << " fill:" << buf->fill;
	std::cout << ((buf->image1)? " Image 1": "") << ((buf->image2)? " Image 2": "");
	std::cout << ((buf->scrollText)? " Scroller": "") << ((buf->font)? " Font": "");
	std::cout << " video:" << buf->videoPath << " img1:" << buf->image1Path;
	std::cout << " img2:" << buf->image2Path << " scr:" << buf->scrollerText;
	std::cout << std::endl;
}

void _freeItem(struct Item *buf) {
std::cout << (char *)__FUNCTION__ << " " << buf->name << std::endl;
	if(buf) {
		buf->debug(buf);
		buf->videoPath.clear();
		buf->image1Path.clear();
		buf->image2Path.clear();
		buf->scrollerText.clear();
		buf->name.clear();

		if(buf->scrollText) {
			SDL_FreeSurface(buf->scrollText);
			buf->scrollText = NULL;
		}
		free(buf);
	}
}

int _parseBufferItem(struct Item *buf, char *data) {
std::cout << (char *)__FUNCTION__ <<  " " << data << " " << std::endl;
	buf->name = data;
	if(!strncmp(data, "trigger=", 8)) {
		buf->trigger = getTrigger(&data[8]);
	} else if (!strncmp(data, "video=", 6)) {
		buf->videoPath = removeEndWhiteSpace(&data[6]);
	} else if(!strncmp(data, "image=", 6)) {
		if((buf->image1 = loadImage(removeEndWhiteSpace(&data[6]))) == NULL) {
			std::cout << data << " doesn't exists" << std::endl;
		} else {
			buf->image1Path = removeEndWhiteSpace(&data[6]);
		}
		buf->debug(buf);
	} else if(!strncmp(data, "image2=", 7)) {
		if((buf->image2 = loadImage(removeEndWhiteSpace(&data[7]))) == NULL) {
			std::cout << data << " doesn't exists" << std::endl;
		} else {
			buf->image2Path = removeEndWhiteSpace(&data[7]);
		}
		buf->debug(buf);
	} else if(!strncmp(data, "x=", 2)) {
		if(parseInt(&data[2], &buf->x)) std::cout << "Unknown value: " << &data[2] << std::endl;
	} else if(!strncmp(data, "y=", 2)) {
		if(parseInt(&data[2], &buf->y)) std::cout << "Unknown value: " << &data[2] << std::endl;
	} else if(!strncmp(data, "effect=", 7)) {
		buf->action = getEffect(&data[7]);
	} else if(!strncmp(data, "step=", 5)) {
		if(parseInt(&data[5], &buf->steps)) std::cout << "Unknown value: " << &data[5] << std::endl;
	} else if(!strncmp(data, "scroller=", 9)) {
		if((buf->scrollText = renderText(removeEndWhiteSpace(&data[9]), buf->font, buf->color)) == NULL) {
			std::cout << "Error:" << data << std::endl;
		} else {
			buf->scrollerText = removeEndWhiteSpace(&data[9]);
		}
		buf->debug(buf);
	} else if(!strncmp(data, "type=", 5)) {
		std::cout << "TODO (what is buf?): " << data << std::endl;
	} else if(!strncmp(data, "fontsize=", 9)) {
		if(parseInt(&data[9], &buf->fontSize)) std::cout << "Unknown value: " << &data[9] << std::endl;
	} else if(!strncmp(data, "font=", 5)) {
		if((buf->font = initializeFont(removeEndWhiteSpace(&data[5]), buf->fontSize)) == NULL) std::cout << "Not found:" << &data[5] << std::endl;
	} else if(!strncmp(data, "color=", 6)) {
		if(parseHex(&data[6], &buf->color)) std::cout << "Unknown HEX: " << &data[6] << std::endl;
	} else {
		std::cout << data << " -- unknown input!" << std::endl;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return 0;
}

void _updateStep(struct Item *buf, int value) {
	buf->step = (value == 1)? 0: buf->step + 1;
}

int _activate(struct Item *buf) {
	int ret = 0;
	static int err = 0;

	switch(buf->action) {
		case UNINITIALIZED:
			if(!err) std::cout << "Item uninitialized " << std::endl;
			err = 1;
			return 0;
		break;

		case DRAW_PICTURE:
std::cout << 1 << std::endl;
			ret = drawAlignedImage(mainScreen, buf->image1, buf->x, buf->y);
		break;

		case PLAY_VIDEO:
std::cout << 2 << std::endl;
			if(!buf->videoOpen) {
				closeVideoFile();
				buf->videoOpen = openVideoFile((char *)buf->videoPath.c_str(), mainScreen);
			} else {
				if((ret = playNextVideoFramerate(mainScreen)) == 2) {
					buf->videoOpen = 0;
					closeVideoFile();
					ret = 0;
				}
			}
			
		break;

		case ZOOM_IMAGE_IN:
std::cout << 3 << std::endl;
			ret = zoomImageIn(mainScreen, buf->image1, buf->steps, buf->step, buf->x, buf->y);
		break;

		case ZOOM_IMAGE_OUT:
std::cout << 4 << std::endl;
			ret = zoomImageOut(mainScreen, buf->image1, buf->steps, buf->step, buf->x, buf->y);
		break;

		case ROTATE_IMAGE:
std::cout << 5 << std::endl;
			ret = rotateAndDrawImage(mainScreen, buf->image1, buf->step, buf->x, buf->y);
		break;

		case ZOOM_AND_ROTATE_IMAGE:
std::cout << 6 << std::endl;
			//ret = zoomRotateAndDrawImage(mainScreen, buf->image1, 
		break;

		case FADE_IMAGE_IN:
std::cout << 7 << std::endl;
			//TODO xy-setting
			ret = fadeImageIn(mainScreen, buf->image1, buf->steps, buf->step, buf->fill);
		break;

		case FADE_IMAGE_OUT:
std::cout << 8 << std::endl;
			//TODO xy-setting
			ret = fadeImageOut(mainScreen, buf->image1, buf->steps, buf->step, buf->fill);
		break;

		case FADE_IMAGE_TO_IMAGE:
std::cout << 9 << std::endl;
			//TODO xy-setting
			ret = fadeImageToImage(mainScreen, buf->image1, buf->image2, buf->steps, buf->step, buf->fill);
		break;

		case SLIDE_LEFT:
std::cout << 10 << std::endl;
			ret = slideImageFromRight(mainScreen, buf->image1, buf->steps, buf->step, buf->y);
		break;

		case SLIDE_RIGHT:
std::cout << 11 << std::endl;
			ret = slideImageFromLeft(mainScreen, buf->image1, buf->steps, buf->step, buf->y);
		break;

		case SLIDE_LEFT_COMPLETE:
std::cout << 12 << std::endl;
			ret = slideImageCompletelyFromRight(mainScreen, buf->image1, buf->steps, buf->step, buf->y, buf->fill);
		break;

		case SLIDE_RIGHT_COMPLETE:
std::cout << 13 << std::endl;
			ret = slideImageCompletelyFromLeft(mainScreen, buf->image1, buf->steps, buf->step, buf->y, buf->fill);
		break;

		case BOX_PICTURE_IN:
std::cout << 14 << std::endl;
			ret = boxPictureIn(mainScreen, buf->image1, buf->steps, buf->step);
		break;

		case RANDOM_BOXING:
std::cout << 15 << std::endl;
			ret = randomlyBoxPictureIn(mainScreen, buf->image1, buf->steps);
		break;

		case SWIPE_PICTURE_TOP:
std::cout << 16 << std::endl;
			ret = swipePictureFromTop(mainScreen, buf->image1, buf->steps, buf->step);
		break;

		case TEXT_SCROLLER:
std::cout << 17 << std::endl;
			ret = slideImageCompletelyFromLeft(mainScreen, buf->image1, buf->steps, buf->step, buf->y, 0);
		break;

		default:
			std::cout << (char *)__FUNCTION__ << " whoaaat! what happen?" << std::endl;
		break; 
	}

	if (ret) std::cout << (char *)__FUNCTION__ << " " << ret << std::endl;
	err = 0;
	buf->step = (ret == 1)? 0: buf->step + 1;

std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret; 
}

struct Item *initItem(struct Item *buf, int init) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(init) {
		int size = sizeof(struct Item);
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
		if((buf = (struct Item *)malloc(size)) == NULL) {
			std::cout << (char *)__FUNCTION__ << " failed!" << std::endl;
			return NULL;
		}
		memset(buf, 0, size);
	}
	
	buf->action = UNINITIALIZED;
	buf->trigger = LOOP;

	buf->videoPath.clear();
	buf->image1Path.clear();
	buf->image2Path.clear();
	buf->scrollerText.clear();
	buf->name = "uninitialized";
	buf->fontSize = 20;
	if((buf->font = initializeFont((char *)"Impact.ttf", buf->fontSize)) == NULL) std::cout << "Unable to init base font" << std::endl;
	buf->color = 0xFFFFFF;

	buf->x = -1;
	buf->y = -1;
	buf->count = tester++;

	buf->debug = _debug;
	buf->parseBufferItem = _parseBufferItem;
	buf->updateStep = _updateStep;
	buf->activate = _activate;
	buf->free = _freeItem;

std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf;
}

/******************************************************************************************/

void _freeBufferItem(struct bufferItem *buf) {
std::cout << (char *)__FUNCTION__ << " item:" << std::endl;
	if(buf != NULL) {
		for(int i = 0; i < buf->count; i++) {
			buf->itemList[i].free(&buf->itemList[i]);
		}
		free(buf);
	}
}

struct Item *_buffergetNextItem(struct bufferItem *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(++buf->current == NULL) {
		buf->current = &buf->itemList[0];
		return NULL;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->current;
}

int _bufferactivateAllItems(struct bufferItem *buf) {
std::cout << (char *)__FUNCTION__;
	int ret =  0, i = 0; //activate();
	while(&buf->itemList[i] != NULL) {
		//TODO: FIX
		ret += buf->itemList[i].activate(&buf->itemList[i]);
		i++;
	}
	
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret;
}

void _buffersetLastItem(struct bufferItem *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	buf->current = &buf->itemList[buf->count];
}

int _bufferaddNewItemToList(struct bufferItem *buf, char *item) {
//std::cout << (char *)__FUNCTION__ << " " << item << std::endl;
	struct Item *tmp = NULL, *newBuf = NULL;
	int size = ((sizeof(struct Item) * (buf->count + 2))), ret = -1;

std::cout << (char *)__FUNCTION__ << " mallocin' FAIL" << size << " " << buf->count << std::endl;
	if((tmp = (struct Item *)malloc(size)) == NULL) {
		std::cout << (char *)__FUNCTION__ << " failed!" << std::endl;
		return -1;
	}
std::cout << (char *)__FUNCTION__ << " mallocin' 1" << std::endl;
	memset(tmp, 0, size);

	for(int i = 0; i < buf->count; i++) {
		//tmp[i] = buf->itemList[i];
		if(!memcpy(&tmp[i], &buf->itemList[i], sizeof(struct Item))) {
			free(tmp);
			std::cout << (char *)__FUNCTION__ << " memcpy failed!" << std::endl;
			return -1;
		}
	}
	if((newBuf = initItem(newBuf, 1))) {
		if(!(ret = newBuf->parseBufferItem(newBuf, item))) {
			if(memcpy(&tmp[buf->count], newBuf, sizeof(struct Item))) {
				free(buf->itemList);
				buf->itemList = tmp;
				buf->count++;
			}
		}
	}
	
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret;
}
 
struct Item *_buffergetItemByNumber(struct bufferItem *buf, int item) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(item < buf->count) {
		return &buf->itemList[item];
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return NULL;
}

int _bufferparseBufferItem(struct bufferItem *buf, char *data, int item) {
//std::cout << (char *)__FUNCTION__ << " data:" << data << " item:" << item << std::endl;
	struct Item *tmp = NULL;
	int ret = 0;
	if((tmp = buf->getItemByNumber(buf, item)) != NULL) {
		tmp->parseBufferItem(tmp, data);
	} else {
		ret = buf->addNewItemToList(buf, data);
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret;
}

struct bufferItem *initBufferItem(struct bufferItem *buf, int init) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(init) {
		int size = sizeof(struct bufferItem);
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
		if((buf = (struct bufferItem *)malloc(size)) == NULL) {
			std::cout << (char *)__FUNCTION__ << " failed!" << std::endl;
			return NULL;
		}
		memset(buf, 0, size);
	}
	
	buf->getNextItem = _buffergetNextItem;
	buf->addNewItemToList = _bufferaddNewItemToList;
	buf->parseBufferItem = _bufferparseBufferItem;
	buf->activateAllItems = _bufferactivateAllItems;
	buf->getItemByNumber = _buffergetItemByNumber;
	buf->setLastItem = _buffersetLastItem;
	buf->free = _freeBufferItem;
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf;
}

/**************************************************************************************************/

void _freeBufferList(struct bufferList *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(buf != NULL) {
		for(int i = 0; i < buf->count; i++) {
			buf->letterList[i].free(&buf->letterList[i]);
			i++;
		}
		free(buf);
	}
}

struct bufferItem *_getNextBufferItem(struct bufferList *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if((++buf->current) == NULL) {
		buf->current = &buf->letterList[0];
		return NULL;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->current;
}

struct bufferItem *_getBufferStep(struct bufferList *buf, int step) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(step < buf->count) {
		return &buf->letterList[step];
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return NULL;
}

char _getBufferLetter(struct bufferList *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->letter;
}

int _addNewBufferItem(struct bufferList *buf, char *data, int step, int item) {
std::cout << (char *)__FUNCTION__ << std::endl << " data:" << data << " step:" << step << " item:" << item << std::endl;
	struct bufferItem *tmp = NULL, *newBuf = NULL;
	int ret = -1;
	if((tmp = buf->getBufferStep(buf, step)) != NULL) {
std::cout << (char *)__FUNCTION__ << " uninit" << std::endl;
		ret = tmp->parseBufferItem(tmp, data, item);
	} else {
std::cout << (char *)__FUNCTION__ << " 0" << std::endl;
		int size = sizeof(struct bufferItem) * buf->count + 2;
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
		if((tmp = (struct bufferItem *)malloc(size)) == NULL) {
			std::cout << (char *)__FUNCTION__ << " failed" << std::endl;
			return -1;
		}
		memset(tmp, 0, size);
std::cout << (char *)__FUNCTION__ << " 1" << std::endl;
		for(int i = 0; i < buf->count; i++) {
			if(!memcpy(&tmp[i], &buf->letterList[i], sizeof(struct bufferItem))) {
				free(tmp);
				return -1;
			}
		}
std::cout << (char *)__FUNCTION__ << " 2" << std::endl;
		if(initBufferItem(&tmp[buf->count], 0)) {
			if(!(ret = tmp[buf->count].parseBufferItem(&tmp[buf->count], data, item))) {
				free(buf->letterList);
				buf->letterList = tmp;
				buf->count++;
			}
		}
std::cout << (char *)__FUNCTION__ << " 3" << std::endl;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret;
}

unsigned long long _getTrigger(struct bufferList *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->currentPlaytime;
}

int _handleStepItems(struct bufferList *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->current->activateAllItems(buf->current);
}

struct bufferList *initBufferList(struct bufferList *buf, char c, int init) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(init) {
		int size = sizeof(struct bufferList);
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
		if((buf = (struct bufferList *)malloc(size)) == NULL) {
			std::cout << (char *)__FUNCTION__ << " failed" << std::endl;
			return NULL;
		}
		memset(buf, 0, size);
	}

	buf->trigger = LOOP;
	buf->letter = c;

	buf->getNextBufferItem = _getNextBufferItem;
	buf->getBufferStep = _getBufferStep;
	buf->getBufferLetter = _getBufferLetter;
	buf->addNewBufferItem = _addNewBufferItem;
	buf->getTrigger = _getTrigger;
	buf->handleStepItems = _handleStepItems;
	//buf->getItemByNumber = _getItemByNumber;
	buf->free = _freeBufferList;

	buf->trigger = LOOP;

std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf;
}

/**************************************************************************************************/

struct bufferList *_findBufferItem(struct buffer *buf, char c) {
std::cout << (char *)__FUNCTION__ << std::endl;
	for(int i = 0; i < buf->count; i++) {
		if(c == buf->list[i].getBufferLetter(&buf->list[i])) {
			buf->current = &buf->list[i];
			return &buf->list[i];
		}
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return NULL;
}

int _activateCurrentItem(struct buffer *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	//TODO: check timer and wait if needed
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->current->handleStepItems(buf->current);
}

struct bufferList *_getNextBufferItem(struct buffer *buf) {
std::cout << (char *)__FUNCTION__ << std::endl;
	if(++buf->current == NULL) {
		buf->current = &buf->list[0];
		return NULL;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf->current;
}

int _addItemToBuffer(struct buffer *buf, char *data, char button, int step, int item) {
std::cout << (char *)__FUNCTION__ << std::endl;
	struct bufferList *tmp = NULL, *newBuf = NULL;
	int ret = -1;

	if((tmp = buf->findBufferItem(buf, button)) == NULL) {
		int size = sizeof(struct bufferList) * (buf->count + 2);
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
		if((tmp = (struct bufferList *)malloc(size)) != NULL) {
			memset(tmp, 0, size);
			for(int i = 0; i < buf->count; i++) {
				//tmp[i] = buf->list[i];
				if(!memcpy(&tmp[i], &buf->list[i], sizeof(struct bufferList))) {
					free(tmp);
					std::cout << (char *)__FUNCTION__ << " memcpy failed" << std::endl;
					return -1;
				}
			}

			if((newBuf = initBufferList(newBuf, button, 1))) {
				if(!(ret = newBuf->addNewBufferItem(newBuf, data, step, item))) {
					if(memcpy(&tmp[buf->count], newBuf, sizeof(struct bufferList))) {
						free(buf->list);
						buf->list = tmp;
						buf->count++;
					}
				}
				std::cout << "next" << std::endl;
			}
		} else {
			std::cout << (char *)__FUNCTION__ << " failed!" << std::endl;
		}
	} else { 
		ret = tmp->addNewBufferItem(tmp, data, step, item);
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return ret;
}

void _freeBuffer(struct buffer *buf) {
	if(buf) {
		for(int i = 0; i < buf->count; i++) {
			buf->list[i].free(&buf->list[i]);
		}
		free(buf);
	}
}

struct buffer *initBuffer(SDL_Surface *screen) {
	struct buffer *buf = NULL;
	int size = sizeof(struct buffer);
std::cout << (char *)__FUNCTION__ << " mallocin'" << std::endl;
	if((buf = (struct buffer *)malloc(size)) != NULL) {	
		memset(buf, 0, size);
		buf->findBufferItem = _findBufferItem;
		buf->activateCurrentItem = _activateCurrentItem;
		buf->getNextBufferItem = _getNextBufferItem;
		buf->addItemToBuffer = _addItemToBuffer;
		buf->free = _freeBuffer;
		mainScreen = screen;
	} else {
		std::cout << (char *)__FUNCTION__ << " failed!" << std::endl;
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return buf;
}

/********************************************************************************/

int handleNewButton(FILE *fd, struct buffer *Buffer, char button, int step) {
	char *data;
	int item = 0;
	while((data = readLine(fd)) != NULL) {
		if(!strncmp(data, "</button>", 9)) {
			std::cout << "Button end" << std::endl;
			return 1;
		} else if(!strncmp(data, "<next>", 6)) {
			std::cout << "Button next" << std::endl;
			item++;
		} else if (removeEndWhiteSpace(data) != NULL) {
			Buffer->addItemToBuffer(Buffer, data, button, step, item);
		} else {
			std::cout << "what?" << data << std::endl;
		}
	}
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return 0;
}

int readIniFile(std::string path, struct buffer *Buffer) {
	FILE *fd;
	char *data;
	int stepVal, readButtons = 0;

	if((fd = openFileRead((char *)path.c_str())) != NULL) {
		while((data = readLine(fd)) != NULL) {
			if(!strncmp(data, "<button=", 8)) {
				if(!parseInt((char *)&data[9], &stepVal)) {
					handleNewButton(fd, Buffer, data[8], stepVal);
				}
			} else if(!strncmp(data, "<start>", 7)) {
				std::cout << "enter basic init here" << std::endl;
			}
		}
	}
	closeFile(fd);
	
std::cout << "out " << (char *)__FUNCTION__ << std::endl;
	return readButtons;
}


