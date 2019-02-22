
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"
#include "SDL/SDL_rotozoom.h"

#include "dynamicPlatform.h"
#include "imageList.h"
#include "draw.h"
#include "rect.h"

SDL_Surface *zoomAndRotate(SDL_Surface *image, int angle, float zoom) {
	return rotozoomSurface(image, angle, zoom, SMOOTHING_ON);
}

SDL_Surface *zoom(SDL_Surface *image, float zoom) {
	return rotozoomSurface(image, 0, zoom, SMOOTHING_ON);
}

SDL_Surface *rotate(SDL_Surface *image, int angle) {
	return rotozoomSurface(image, angle, 1, SMOOTHING_ON);
}

int rotateAndDrawImage(SDL_Surface *screen, SDL_Surface *image, int angle) {
	SDL_Surface *rotated = rotate(image, angle);
	if(rotated != NULL) {
		drawCenteredImage(screen, rotated);
		SDL_FreeSurface(rotated);
		return 1;
	}
	return 0;
}

int zoomAndDrawImage(SDL_Surface *screen, SDL_Surface *image, float zoom1) {
	SDL_Surface *zoomed = zoom(image, zoom1);
	if(zoomed != NULL) {
		drawCenteredImage(screen, zoomed);
		SDL_FreeSurface(zoomed);
		return 1;
	}
	return 0;
}

int zoomRotateAndDrawImage(SDL_Surface *screen, SDL_Surface *image, int angle, float zoom) {
	SDL_Surface *rotated = zoomAndRotate(image, angle, zoom);
	if(rotated != NULL) {
		drawCenteredImage(screen, rotated);
		SDL_FreeSurface(rotated);
		return 1;
	}
	return 0;
}

int zoomImageIn(SDL_Surface *screen, SDL_Surface *image, float steps, int step) {
	if(screen != NULL && image != NULL) {
		float Steps = 1 / steps;
		Steps += Steps * step;

		zoomAndDrawImage(screen, image, Steps);
		return (Steps >= 1)? 1: 2;
	}
	return 0;
}

int zoomImageOut(SDL_Surface *screen, SDL_Surface *image, float steps, int step) {
	if(screen != NULL && image != NULL) {
		float Steps = 1 / steps;
		Steps = 1 - (Steps * step);

		zoomAndDrawImage(screen, image, Steps);
		return (Steps <= 0)? 1: 2;
	}
	return 0;
}

int slideImageFromLeft(SDL_Surface *screen, SDL_Surface *image, int steps, int step) {
	if(screen != NULL && image != NULL) {
		float Steps = screen->w / steps;
		Steps += Steps * step;

		drawImage(screen, image, screen->w - Steps, 0, image->w, image->h);
		return ((screen->w - Steps) <= 0)? 1: 2;
	}
	return 0;
}

int slideImageFromRight(SDL_Surface *screen, SDL_Surface *image, int steps, int step) {
	if(screen != NULL && image != NULL) {
		float Steps = screen->w / steps;
		Steps += Steps * step;
		float pos = (-image->w + Steps);
		pos = (pos > 0)? 0: pos;

		drawImage(screen, image, pos, 0, image->w, image->h);
		return (pos >= 0)? 1: 2;
	}
	return 0;
}


int fadeImageIn(SDL_Surface *screen, SDL_Surface *image, int steps, int step) {
	if(screen != NULL) {
		float Steps = 255 / steps;
		Steps += Steps * step;
		float pos = Steps;

		SDL_FillRect(screen, NULL, 0x0);
		SDL_SetAlpha(image, SDL_SRCALPHA, pos);
		drawImage(screen, image, 0, 0, image->w, image->h);
		SDL_SetAlpha(image, SDL_SRCALPHA, 255);
		return (pos >= 255)? 1: 2;
	}

	return 0;
}

int fadeImageOut(SDL_Surface *screen, SDL_Surface *image, int steps, int step) {
	if(screen != NULL) {
		float Steps = 255 / steps;
		Steps += Steps * step;
		float pos = 255 - Steps;

		SDL_FillRect(screen, NULL, 0x0);
		SDL_SetAlpha(image, SDL_SRCALPHA, pos);
		drawImage(screen, image, 0, 0, image->w, image->h);
		SDL_SetAlpha(image, SDL_SRCALPHA, 255);
		return (pos <= 0)? 1: 2;
	}

	return 0;
}

int fadeImageToImage(SDL_Surface *screen, SDL_Surface *old, SDL_Surface *image, int steps, int step) {
	if(screen != NULL && old != NULL && image != NULL) {
		float Steps = 255 / steps;
		Steps += Steps * step;
		float pos = 255 - Steps;
		
		SDL_FillRect(screen, NULL, 0x0);
		SDL_SetAlpha(old, SDL_SRCALPHA, 255 - Steps);
		drawImage(screen, old, 0, 0, image->w, image->h);
		SDL_SetAlpha(image, SDL_SRCALPHA, Steps);
		drawImage(screen, image, 0, 0, image->w, image->h);
		SDL_SetAlpha(old, SDL_SRCALPHA, 255);
		SDL_SetAlpha(image, SDL_SRCALPHA, 255);
		return (pos <= 0)? 1: 2;
	}
	return 0;
}

int slideImageCompletelyFromLeft(SDL_Surface *screen, SDL_Surface *image, int steps, int step) {
	if(screen != NULL && image != NULL) {
		float Steps = (screen->w + image->w) / steps;
		Steps += Steps * step;

		drawImage(screen, image, screen->w - Steps, 0, image->w, image->h);
		if((screen->w - Steps) <= 0) {
			SDL_Rect rect;
			initRectangle(&rect, screen->w - Steps + image->w, 0, image->w, image->h);
			SDL_FillRect(screen, &rect, 0x0);
			//drawImage(screen, image, screen->w - Steps + image->w, 0, image->w, image->h);
		}
		return (((screen->w + image->w) - Steps) <= 0)? 1: 2;
	}
	return 0;
}



