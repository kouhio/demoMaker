#ifndef __FFMPEGAPI_H__
#define __FFMPEGAPI_H__

#include "SDL/SDL.h"

#ifdef __cplusplus
	extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
int playVideoFileFrame(SDL_Surface *screen, char *path);
void closeVideo(void);

#ifdef __cplusplus
}
#endif

#endif

