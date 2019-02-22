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
//std::cout << (char *)__FUNCTION__ << std::endl;
	if(!strncmp(data, "once", 4)) {
		return ONCE;
	} else if(!strncmp(data, "video", 5)) {
		return VIDEND;
	} else if(!strncmp(data, "timer", 5)) {
		return TIMER;
	} else if(!strncmp(data, "loop", 4)) {
	}
	return LOOP;
}

ImageHandlers getEffect(char *data) {
//std::cout << (char *)__FUNCTION__ << std::endl;
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
	return UNINITIALIZED;
}

/*******************************************************************************/

Item::Item() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	this->step = 0;
	this->steps = 0;
	this->action = UNINITIALIZED;
	this->trigger = LOOP;
	this->triggerTime = 0;

	this->playtime = 0;
	this->loop = 0;
	this->once = 0;
	this->videoOpen = 0;
	this->videoPath.clear();
	this->image1Path.clear();
	this->image2Path.clear();
	this->scrollerText.clear();
	this->name = "uninitialized";
	this->image1 = NULL;
	this->image2 = NULL;
	this->scrollText = NULL;
	this->fontSize = 20;
	if((this->font = initializeFont((char *)"Impact.ttf", fontSize)) == NULL) std::cout << "Unable to init base font" << std::endl;
	this->color = 0xFFFFFF;

	this->itemLoop = 0;
	this->x = -1;
	this->y = -1;
	this->fill = 0;
	this->count = tester++;

}

void Item::debug(void) {
	std::cout << "--> Step:" << this->step << " steps:" << this->steps << " action:" << this->action << " trigger:" << this->trigger << " time:" << this->triggerTime;
	std::cout << " playtime:" << this->playtime << " loop:" << this->loop << " once:" << this->once << " video:" << this->videoOpen << " " << this->name;
	std::cout << " font:" << this->fontSize << " color:" << this->color << " loop2:" << this->itemLoop << " x:" << this->x << " y:" << this->y << " fill:" << this->fill;
	std::cout << ((this->image1)? " Image 1": "") << ((this->image2)? " Image 2": "") << ((this->scrollText)? " Scroller": "") << ((this->font)? " Font": "");
	std::cout << " video:" << this->videoPath << " img1:" << this->image1Path << " img2:" << this->image2Path << " scr:" << this->scrollerText;
	std::cout << std::endl;
}

Item::~Item() {
//std::cout << (char *)__FUNCTION__ << " " << name << std::endl;
debug();
	this->videoPath.clear();
	this->image1Path.clear();
	this->image2Path.clear();
	this->scrollerText.clear();
	this->name.clear();

	//TODO: go through the list
	if(this->scrollText) {
		SDL_FreeSurface(this->scrollText);
		scrollText = NULL;
	}
}

int Item::parseBufferItem(char *data) {
std::cout << (char *)__FUNCTION__ <<  " " << data << " " << std::endl;
	name = data;
	if(!strncmp(data, "trigger=", 8)) {
		this->trigger = getTrigger(&data[8]);
	} else if (!strncmp(data, "video=", 6)) {
		this->videoPath = removeEndWhiteSpace(&data[6]);
	} else if(!strncmp(data, "image=", 6)) {
		if((this->image1 = loadImage(removeEndWhiteSpace(&data[6]))) == NULL) {
			std::cout << data << " doesn't exists" << std::endl;
		} else {
			this->image1Path = removeEndWhiteSpace(&data[6]);
		}
		debug();
	} else if(!strncmp(data, "image2=", 7)) {
		if((this->image2 = loadImage(removeEndWhiteSpace(&data[7]))) == NULL) {
			std::cout << data << " doesn't exists" << std::endl;
		} else {
			this->image2Path = removeEndWhiteSpace(&data[7]);
		}
		debug();
	} else if(!strncmp(data, "x=", 2)) {
		if(parseInt(&data[2], &this->x)) std::cout << "Unknown value: " << &data[2] << std::endl;
	} else if(!strncmp(data, "y=", 2)) {
		if(parseInt(&data[2], &this->y)) std::cout << "Unknown value: " << &data[2] << std::endl;
	} else if(!strncmp(data, "effect=", 7)) {
		this->action = getEffect(&data[7]);
	} else if(!strncmp(data, "step=", 5)) {
		if(parseInt(&data[5], &this->steps)) std::cout << "Unknown value: " << &data[5] << std::endl;
	} else if(!strncmp(data, "scroller=", 9)) {
		if((this->scrollText = renderText(removeEndWhiteSpace(&data[9]), font, color)) == NULL) {
			std::cout << "Error:" << data << std::endl;
		} else {
			this->scrollerText = removeEndWhiteSpace(&data[9]);
		}
		debug();
	} else if(!strncmp(data, "type=", 5)) {
		std::cout << "TODO (what is this?): " << data << std::endl;
	} else if(!strncmp(data, "fontsize=", 9)) {
		if(parseInt(&data[9], &this->fontSize)) std::cout << "Unknown value: " << &data[9] << std::endl;
	} else if(!strncmp(data, "font=", 5)) {
		if((this->font = initializeFont(removeEndWhiteSpace(&data[5]), fontSize)) == NULL) std::cout << "Not found:" << &data[5] << std::endl;
	} else if(!strncmp(data, "color=", 6)) {
		if(parseHex(&data[6], &this->color)) std::cout << "Unknown HEX: " << &data[6] << std::endl;
	} else {
		std::cout << data << " -- unknown input!" << std::endl;
	}
	return 0;
}

void Item::updateStep(int value) {
	step = (value == 1)? 0: step + 1;
}

int Item::activate(void) {
	int ret = 0;
	static int err = 0;

	switch(this->action) {
		case UNINITIALIZED:
			if(!err) std::cout << "Item uninitialized " << std::endl;
			err = 1;
			return 0;
		break;

		case DRAW_PICTURE:
std::cout << 1 << std::endl;
			ret = drawAlignedImage(mainScreen, image1, x, y);
		break;

		case PLAY_VIDEO:
std::cout << 2 << std::endl;
			if(!videoOpen) {
				closeVideoFile();
				videoOpen = openVideoFile((char *)videoPath.c_str(), mainScreen);
			} else {
				if((ret = playNextVideoFramerate(mainScreen)) == 2) {
					videoOpen = 0;
					closeVideoFile();
					ret = 0;
				}
			}
			
		break;

		case ZOOM_IMAGE_IN:
std::cout << 3 << std::endl;
			ret = zoomImageIn(mainScreen, image1, steps, step, x, y);
		break;

		case ZOOM_IMAGE_OUT:
std::cout << 4 << std::endl;
			ret = zoomImageOut(mainScreen, image1, steps, step, x, y);
		break;

		case ROTATE_IMAGE:
std::cout << 5 << std::endl;
			ret = rotateAndDrawImage(mainScreen, image1, step, x, y);
		break;

		case ZOOM_AND_ROTATE_IMAGE:
std::cout << 6 << std::endl;
			//ret = zoomRotateAndDrawImage(mainScreen, image1, 
		break;

		case FADE_IMAGE_IN:
std::cout << 7 << std::endl;
			//TODO xy-setting
			ret = fadeImageIn(mainScreen, image1, steps, step, fill);
		break;

		case FADE_IMAGE_OUT:
std::cout << 8 << std::endl;
			//TODO xy-setting
			ret = fadeImageOut(mainScreen, image1, steps, step, fill);
		break;

		case FADE_IMAGE_TO_IMAGE:
std::cout << 9 << std::endl;
			//TODO xy-setting
			ret = fadeImageToImage(mainScreen, image1, image2, steps, step, fill);
		break;

		case SLIDE_LEFT:
std::cout << 10 << std::endl;
			ret = slideImageFromRight(mainScreen, image1, steps, step, y);
		break;

		case SLIDE_RIGHT:
std::cout << 11 << std::endl;
			ret = slideImageFromLeft(mainScreen, image1, steps, step, y);
		break;

		case SLIDE_LEFT_COMPLETE:
std::cout << 12 << std::endl;
			ret = slideImageCompletelyFromRight(mainScreen, image1, steps, step, y, fill);
		break;

		case SLIDE_RIGHT_COMPLETE:
std::cout << 13 << std::endl;
			ret = slideImageCompletelyFromLeft(mainScreen, image1, steps, step, y, fill);
		break;

		case BOX_PICTURE_IN:
std::cout << 14 << std::endl;
			ret = boxPictureIn(mainScreen, image1, steps, step);
		break;

		case RANDOM_BOXING:
std::cout << 15 << std::endl;
			ret = randomlyBoxPictureIn(mainScreen, image1, steps);
		break;

		case SWIPE_PICTURE_TOP:
std::cout << 16 << std::endl;
			ret = swipePictureFromTop(mainScreen, image1, steps, step);
		break;

		case TEXT_SCROLLER:
std::cout << 17 << std::endl;
			ret = slideImageCompletelyFromLeft(mainScreen, image1, steps, step, y, 0);
		break;

		default:
			std::cout << (char *)__FUNCTION__ << " whoaaat! what happen?" << std::endl;
		break; 
	}

	if (ret) std::cout << (char *)__FUNCTION__ << " " << ret << std::endl;
	err = 0;
	step = (ret == 1)? 0: step + 1;

	return ret; 
}

/******************************************************************************************/

bufferItem::bufferItem() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	//itemList.clear();
	currentListItem = itemList.begin();
}

bufferItem::~bufferItem() {
//std::cout << (char *)__FUNCTION__ << " item:" << std::endl;
	itemList.clear();
}

std::vector<Item>::iterator bufferItem::getNextItem(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	if(++currentListItem >= itemList.end()) {
		currentListItem = itemList.begin();
		return itemList.end();
	}
	return currentListItem;
}

int bufferItem::activateAllItems(void) {
//std::cout << (char *)__FUNCTION__;
	int ret =  0, i = 0; //activate();
	for(currentListItem = itemList.begin(); currentListItem < itemList.end(); currentListItem++) {
		ret += currentListItem->activate();
		i++;
	}
//std::cout << ret << "->" << i << std::endl;
	return ret;
}

void bufferItem::setLastItem(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	currentListItem = itemList.end();
	currentListItem--;
}

int bufferItem::addNewItemToList(char *item) {
//std::cout << (char *)__FUNCTION__ << " " << item << std::endl;
	Item *temp = new Item;
	itemList.push_back(*temp);
	//currentListItem = *temp; 
	setLastItem();
	currentListItem->parseBufferItem(item);
	return 0;
}

int bufferItem::getItemByNumber(int item) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	int i = 0;
	for(currentListItem = itemList.begin(); currentListItem < itemList.end(); currentListItem++) {
		if(item == i) {
			return 0;
		}
		i++;
	}
	return -1;
}

int bufferItem::parseBufferItem(char *data, int item) {
//std::cout << (char *)__FUNCTION__ << " data:" << data << " item:" << item << std::endl;
	if(!getItemByNumber(item)) {
		currentListItem->parseBufferItem(data);
	} else {
		Item *tmp = new Item;
		itemList.push_back(*tmp);
		addNewItemToList(data);
	}
	return 0;
}

/**************************************************************************************************/

bufferList::bufferList() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	trigger = LOOP;
	step = 0;
	letter = 0;
	currentPlaytime = 0;
	currentLoop = 0;
	//letterList.clear();
	currentListItem = letterList.begin();
}

bufferList::bufferList(char c) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	trigger = LOOP;
	step = 0;
	letter = c;
	currentPlaytime = 0;
	currentLoop = 0;
	//letterList.clear();
	currentListItem = letterList.begin();
}

bufferList::~bufferList() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	//TODO: go through the list
	letterList.clear();
}

int bufferList::getNextBufferItem(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	if(++currentListItem >= letterList.end()) {
		currentListItem = letterList.begin();
		return 0; //*letterList.end();
	}
	return -1; //*currentListItem;
}

int bufferList::getBufferStep(int step) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	int curStep = 0;
	for(currentListItem = letterList.begin(); currentListItem < letterList.end(); currentListItem++) {
		if(step == curStep++) {
			return 0;
		}
	}
	return -1;
}

char bufferList::getBufferLetter(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	return letter;
}

int bufferList::addNewBufferItem(char *data, int step, int item) {
//std::cout << (char *)__FUNCTION__ << " data:" << data << " step:" << step << " item:" << item << std::endl;
	if(!getBufferStep(step)) {
		currentListItem->parseBufferItem(data, item);
	} else {
		bufferItem *tmp = new bufferItem;
		letterList.push_back(*tmp);
		tmp->addNewItemToList(data);
	}
	return 0;
}

unsigned long long bufferList::getTrigger(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	return currentPlaytime;
}

int bufferList::handleStepItems(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	return currentListItem->activateAllItems();
}

/**************************************************************************************************/

buffer::buffer() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	//list.clear();
}

buffer::~buffer() {
//std::cout << (char *)__FUNCTION__ << std::endl;
	//TODO: go through the list
	list.clear();
}

int buffer::findBufferItem(char c) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	for(ipos = list.begin(); ipos < list.end(); ipos++) {
		if(c == ipos->getBufferLetter()) {
			return 0;
		}
	}
	return -1;
}

int buffer::activeCurrentItem(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	//TODO: check timer and wait if needed
	return ipos->handleStepItems();
}

int buffer::getNextBufferItem(void) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	if(!ipos->getNextBufferItem()) {
		return ipos->handleStepItems();
	}
	return 0;
}

int buffer::addItemToBuffer(char *data, char button, int step, int item) {
//std::cout << (char *)__FUNCTION__ << std::endl;
	if(findBufferItem(button)) {
		bufferList *tmp = new bufferList(button);
		list.push_back(*tmp);
		tmp->addNewBufferItem(data, step, item);
	} else {
		ipos->addNewBufferItem(data, step, item);
	}
	return 0;
}

/********************************************************************************/

int handleNewButton(FILE *fd, class buffer *Buffer, char button, int step) {
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
			Buffer->addItemToBuffer(data, button, step, item);
		} else {
			std::cout << "what?" << data << std::endl;
		}
	}
	return 0;
}

int readIniFile(std::string path, class buffer *Buffer) {
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
	
	return readButtons;
}


