#include <string>

#ifndef _COLOR_H_
#define _COLOR_H_

typedef enum { NONE, BLACK, WHITE, BORDER } Color;

const char* color_name(Color cl);
Color color_reverse(Color cl);

#endif
