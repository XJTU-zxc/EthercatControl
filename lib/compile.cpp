#include "compile.h"

#include <string.h>

#include "command.h"
#include "paramscpp.h"

int sg_initState = 28;
char sg_charMap[128] =
{-1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 18, -1, -1, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, 56, 30, 32, 55, 54, 53, 57, 58, 59, 60, 61, 12, 0, 35, 62, 9, 8, 16, 7, 6, 5, 4, 3, 2, 1, 22, 19, 10, 13, 11, 52, 51, 26, 25, 27, 28, 23, 29, 31, 33, 34, 15, 14, 63, 64, 65, 66, 67, 68, 69, 20, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, -1, -1};
int16_t sg_stateMatrix[COMPILER_STATE_MATRIX_ROW][COMPILER_STATE_MATRIX_COL] = {
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, 25, 25, 25, 25, 25, 25, 25, 25, 25, -1, -1, -1, -1, -1, -1, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, 26, 26, 26, 26, 26, 26, 26, 26, 26, -1, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{29, 25, 25, 25, 25, 25, 25, 25, 25, 25, 22, 21, 20, 19, 17, 16, 25, 31, 5, 2, 18, 4, 1, 11, 6, 8, 7, 9, 10, 12, 32, 13, 0, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, 25, 25, 25, 25, 25, 25, 25, 25, 25, -1, -1, -1, -1, -1, -1, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, 26, 26, 26, 26, 26, 26, 26, 26, 26, -1, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, -1, -1, 32, 32, -1, 32, 32, 32, 32, 32, 32, 32, 32, 27, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32}};

int CompilerInit(Compiler* compiler)
{
    /* 初始状态 */
    compiler->initState = sg_initState;
    /* 当前编译器处在的状态 */
    compiler->cstate = sg_initState;
    /* 字母映射表 */
    compiler->charMap = sg_charMap;
    /* 状态转移矩阵 */
    compiler->stateMatrix = sg_stateMatrix;
    /* 当前保存的 token ，最大 ? 个字节 */
    compiler->token[0] = 0;

    return 0;
}

int CompilerReadToken(Compiler* compiler, struct StringWrapper* wrapper)
{
    int rc = -1;
    int i = 0;
    int rt = 0;
    int state = -1;

    /* 当前不处于初始状态 */
    if (compiler->cstate != compiler->initState) {return -1;}

    for(i=0; i<wrapper->len; ++i) {
        /* 将 c 通过 charMap数组进行映射 */
        int cid = compiler->charMap[wrapper->buf[i]];

        /* 当前字符不存在或者状态转移后是一个错误的状态 */
        if (cid == -1 ||
            ((compiler->cstate = compiler->stateMatrix[compiler->cstate][cid]) == -1))
        {
            /**
             * @brief 跳出去检查上一个记录的正确状态
             */
            break;
        }

        /* compiler->cstate 是一个终止态 */
        if (compiler->cstate < compiler->initState) {
            state = compiler->cstate;
            rt = i + 1;
        }
    }

    /* 之前没有检测到关键字 */
    if (0 == rt) {
        rc = -1;
    } else {
        // 之前检测到了关键字
        rc = state;

        /* 将检测到的 token 复制到 compiler->token 中去 */
        memcpy(compiler->token, wrapper->buf, rt);
        compiler->token[rt] = 0;
        compiler->tokenlen = rt;

        /* 消耗已经读取的字符 */
        wrapper->buf += rt;
        wrapper->len -= rt;
    }

    /* 重置当前的状态，用于继续下一次的检测 */
    compiler->cstate = compiler->initState;

    return rc;
}

/**
 * @brief 读取关键字，跳过 tab、空格
 */
int CompilerReadImportantToken(Compiler* compiler, struct StringWrapper* wrapper)
{
    int id = -1;

    do {
        id = CompilerReadToken(compiler, wrapper);
    } while(id == IDENTIFIER_TAB || id == IDENTIFIER_SPACE);

    return id;
}

/**
 * 跳过开头的空格和 tab，寻找第一个字符
 * @return 第一个不是空格字符的索引
 * */
void Strip(StringWrapper* wrapper)
{
    while (wrapper->len>0 && (wrapper->buf[0]==' ' || wrapper->buf[0]=='\t')) {
        ++wrapper->buf;
        --wrapper->len;
    }
}
/**
 * 跳过开头的空格和 tab，寻找第一个字符,判断是否为#
 * @return 0 是 ，     -1 否
 * */
int isProgName(const char* buf, int length){
    int i = 0;
    while(buf[i] == ' ' || buf[i] == '\t'){
        i++;
    }
    if(i >= length){
        return -1;
    }
    if(buf[i] == '#'){
        return 0;
    }

    return -1;
}

/**
 * 消耗 wrapper 指定个数字符
 * */
int StringWrapperResume(StringWrapper* wrapper, int n)
{
    if (wrapper->len < n) {
        return -1;
    }
    wrapper->len -= n;
    wrapper->buf += n;

    return 0;
}

int cmd_first_is_A(Compiler* compiler, StringWrapper* wrapper, bool isMdi)
{
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);
    switch (ch0) {
    case 'I':
        if (ch1 == 'T') {
            // AIT 指令
            rc = Command_AIT(compiler, wrapper, isMdi);
        }
        break;
    case 'M':
        if (ch1 == 'E') {
            // AME 指令

        }
        else if (ch1 == 'P') {
            // AMP 指令
            rc = Command_AMP(compiler, wrapper, isMdi);
        }
        break;
    case 'P':
        if (ch1 == 'G') {
            // APG 指令

        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_B(Compiler* compiler, StringWrapper* wrapper, bool isMdi)
{
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'C':
        if (ch1 == 'P') {
            // BCP 指令
            rc = Command_BCP(compiler, wrapper, isMdi);
        }
        break;
    case 'G':
        if (ch1 == 'M') {
            // BGM 指令
            rc = Command_BGM(compiler, wrapper, isMdi);
        }
        break;
    case 'K':
        if (ch1 == 'C') {
            // BKC 指令
            rc = Command_BKC(compiler, wrapper, isMdi);
        }
        break;
    case 'P':
        if (ch1 == 'M') {
            // BPM 指令
            rc = Command_BPM(compiler, wrapper, isMdi);
        }
        else if (ch1 == 'V') {
            // BPV 指令
        }
        break;
    case 'S':
        if (ch1 == 'N') {
            // BSN 指令
            rc = Command_BSN(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_C(Compiler* compiler, StringWrapper* wrapper, bool isMdi)
{
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'L':
        if (ch1 == 'B') {
            // CLB 指令
            rc = Command_CLB(compiler, wrapper, isMdi);
        }else if(ch1 == 'R'){
            //CLR
            rc = Command_CLR(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'B') {
            // CSB 指令
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_D(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'A':
        if (ch1 == 'O') {
            // DAO 指令
        }else if(ch1 == 'T'){
            //DAT
        }
        break;
    case 'C':
        if (ch1 == 'E') {
            // DCE 指令
        }
        break;
    case 'E':
        if (ch1 == 'C') {
            // DEC 指令
            rc = Command_DEC(compiler, wrapper, isMdi);
        }
        break;
    case 'F':
        if (ch1 == 'N') {
            // DFN 指令
            rc = Command_DFN(compiler, wrapper, isMdi);
        }
        break;
    case 'I':
        if (ch1 == 'T') {
            // DEC 指令
            rc = Command_DIT(compiler, wrapper, isMdi);
        }
        break;
    case 'L':
        if (ch1 == 'P') {
            // DLP 指令
        }
        break;
    case 'M':
        if (ch1 == 'T') {
            // DMT 指令
        }
        break;
    case 'P':
        if (ch1 == 'C') {
            // DPC 指令
            rc = Command_DPC(compiler, wrapper, isMdi);
        }else if(ch1 == 'E'){
            //DPE
            rc = Command_DPE(compiler, wrapper, isMdi);
        }else if(ch1 == 'A'){
            //DPA
        }
        break;
    case 'R':
        if (ch1 == 'N') {
            // DEC 指令
            rc = Command_DRN(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'P') {
            // DSP 指令
        }
        break;
    case 'Z':
        if (ch1 == 'N') {
            // DEC 指令
            rc = Command_DZN(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}


int cmd_first_is_E(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'G':
        if (ch1 == 'M') {
            // EGM 指令
            rc = Command_EGM(compiler, wrapper, isMdi);
        } else if (ch1 == 'R') {
            //EGR
            rc = Command_EGR(compiler, wrapper, isMdi);
        }
        break;
    case 'N':
        if (ch1 == 'D') {
            // END 指令
            rc = Command_END(compiler, wrapper, isMdi);
        }
        break;
    case 'P':
        if (ch1 == 'G') {
            // EPG 指令
            rc = Command_EPG(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'T') {
            // EST 指令
            rc = Command_EST(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_F(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'I':
        if (ch1 == 'D') {
            // FID 指令
            rc = Command_FID(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'W') {
            // FSW 指令 zjq
            rc = Command_FSW(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}


int cmd_first_is_G(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'O':
        if (ch1 == 'T') {
            if(wrapper->buf[0] == 'O'){
                // GOTO
                StringWrapperResume(wrapper, 1);
                rc = Command_GOTO(compiler, wrapper, isMdi);
            }
        }
        break;
    default:
        break;
    }

    return rc;
}


int cmd_first_is_H(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'P':
        if (ch1 == 'G') {
            // HPG 指令
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_I(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'P':
        if (ch1 == 'A') {
            // IPA 指令
            rc = Command_IPA(compiler, wrapper, isMdi);
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_J(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'O':
        if (ch1 == 'G') {
            // JOG 指令
            rc = Command_JOG(compiler, wrapper, isMdi);
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_L(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'M':
        if (ch1 == 'D') {
            // LMD 指令
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_M(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'A':
        if (ch1 == 'C') {
            // MAC 指令
            rc = Command_MAC(compiler, wrapper, isMdi);
        }
        break;
    case 'C':
        if (ch1 == 'M') {
            // MCM 指令
            rc = Command_MCM(compiler, wrapper, isMdi);
        }
        break;
    case 'D':
        if (ch1 == 'C') {
            // MDC 指令
        }
        break;
    case 'I':
        if (ch1 == 'T') {
            // MIT 指令
            rc = Command_MIT(compiler, wrapper, isMdi);
        }
        break;
    case 'O':
        if (ch1 == 'F') {
            // MOF 指令
            rc = Command_MOF(compiler, wrapper, isMdi);
        }else if (ch1 == 'N'){
            //MON
            rc = Command_MON(compiler, wrapper, isMdi);
        }

        break;
    case 'R':
        if (ch1 == 'T') {
            // MRT 指令
            rc = Command_MRT(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'P') {
            // MSP 指令
            rc = Command_MSP(compiler, wrapper, isMdi);
        }
        break;
    case 'V':
        if (ch1 == 'P') {
            // MVP 指令

        }
        else if (ch1 == 'R') {
            // MVR 指令
            rc = Command_MVR(compiler, wrapper, isMdi);
        }
        else if (ch1 == 'A') {
            // MVA 指令
            rc = Command_MVA(compiler, wrapper, isMdi);
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_P(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'G':
        if (ch1 == 'L') {
            // PGL 指令
        }
        break;
    case 'V':
        if (ch1 == 'L') {
            // PVL 指令
        }
        break;

    default:
        break;
    }

    return rc;
}


int cmd_first_is_R(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'A':
        if (ch1 == 'N') {
            // RAN 指令
        }
        break;
    case 'D':
        if (ch1 == 'I') {
            // RDI 指令  zjq
            rc = Command_RDI(compiler, wrapper, isMdi);
        }else if(ch1 == 'O'){
            //RDO zjq
            rc = Command_RDO(compiler, wrapper, isMdi);
        }
        break;
    case 'P':
        if (ch1 == 'B') {
            // RPB 指令
            rc = Command_RPB(compiler, wrapper, isMdi);
        }else if(ch1 == 'G'){
            //RPG
        }
        break;
    case 'S':
        if (ch1 == 'T') {
            // RST 指令
            rc = Command_RST(compiler, wrapper, isMdi);
        }
        break;
    case 'T':
        if (ch1 == 'I') {
            // RTI 指令
        }
        break;
    default:
        break;
    }

    return rc;
}


int cmd_first_is_S(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'E':
        if (ch1 == 'T') {
            // SET 指令
            rc = Command_SET(compiler, wrapper, isMdi);
        }else if(ch1 == 'N'){
            //SEN
            rc = Command_SEN(compiler, wrapper, isMdi);
        }
        break;
    case 'F':
        if (ch1 == 'L') {
            // SFL 指令
            rc = Command_SFL(compiler, wrapper, isMdi);
        }
        break;
    case 'R':
        if (ch1 == 'L') {
            // SRL 指令
            rc = Command_SRL(compiler, wrapper, isMdi);
        }
        break;
    case 'T':
        if (ch1 == 'M') {
            // RTI 指令
            rc =  Command_STM(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_T(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {

    case 'C':
        if (ch1 == 'A') {
            // TCA 指令
        }else if(ch1 == 'E'){
            //TCE
            rc = Command_TCE(compiler, wrapper, isMdi);
        }else if(ch1 == 'P'){
            //TCP
            rc = Command_TCP(compiler, wrapper, isMdi);
        }else if(ch1 == 'V'){
            //TCV
            rc = Command_TCV(compiler, wrapper, isMdi);
        }
        break;
    case 'E':
        if (ch1 == 'C') {
            // TEC 指令
            rc = Command_TEC(compiler, wrapper, isMdi);
        }else if(ch1 == 'R'){
            //TER
            rc = Command_TER(compiler, wrapper, isMdi);
        }
        break;
    case 'F':
        if (ch1 == 'V') {
            // TFV 指令
            rc = Command_TFV(compiler, wrapper, isMdi);
        }
        break;
    case 'I':
        if (ch1 == 'P') {
            // TIP 指令
        }
        break;
    case 'S':
        if (ch1 == 'V') {
            // TSV 指令
            rc = Command_TSV(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}


int cmd_first_is_U(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'L':
        if (ch1 == 'P') {
            // ULP 指令
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_V(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'A':
        if (ch1 == 'C') {
            // VAC 指令
            rc = Command_VAC(compiler, wrapper, isMdi);
        }else if(ch1 == 'W'){
            //VAW
            rc = Command_VAW(compiler, wrapper, isMdi);
        }
        break;
    case 'C':
        if (ch1 == 'W') {
            // VCW 指令
            rc = Command_VCW(compiler, wrapper, isMdi);
        }
        break;
    case 'D':
        if (ch1 == 'C') {
            // VDC 指令
        }
        break;
    case 'I':
        if (ch1 == 'C') {
            // VIC 指令
            rc = Command_VIC(compiler, wrapper, isMdi);
        } else if (ch1 == 'E') {
            rc = Command_VIE(compiler, wrapper, isMdi);
        }

        break;
    case 'L':
        if (ch1 == 'I') {
            // VLI 指令
            rc = Command_VLI(compiler, wrapper, isMdi);
        }
        break;
    case 'S':
        if (ch1 == 'P') {
            // VSP 指令
            rc = Command_VSP(compiler, wrapper, isMdi);
        }else if(ch1 == 'R'){
            // VSR 指令
            rc = Command_VSR(compiler, wrapper, isMdi);
        }
        break;
    case 'T':
        if (ch1 == 'N') {
            // VTN 指令
            rc = Command_VTN(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_W(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'T':
        if (ch1 == 'M') {
            rc = Command_WTM(compiler, wrapper, isMdi);
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is__(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'F':
        if (ch1 == 'L') {
            if(wrapper->buf[0] == 'S'){
                StringWrapperResume(wrapper, 1);
                // _FLS 指令
                rc = Command__FLS(compiler, wrapper, isMdi);
            }
        }
        break;
    case 'R':
        if (ch1 == 'L') {
            if(wrapper->buf[0] == 'S') {
                StringWrapperResume(wrapper, 1);
                // _RLS 指令
                rc = Command__RLS(compiler, wrapper, isMdi);
            }
        }
        break;
    default:
        break;
    }

    return rc;
}

int cmd_first_is_Shift6(Compiler* compiler, StringWrapper* wrapper, bool isMdi){
    int rc = -1;
    char ch0 = wrapper->buf[0];
    char ch1 = wrapper->buf[1];
    /* 消耗前两个字符 */
    StringWrapperResume(wrapper, 2);

    switch (ch0) {
    case 'M':
        if (ch1 == 'R') {
            // ^MR 指令

        }
        break;

    default:
        break;
    }

    return rc;
}
/**
 * 上位机已经保证的下发的指令是正确的
 * */
int CompilerParse(Compiler* compiler, const char* buf, int n, bool isMdi)
{
    int rc = 0;
    /* 跳过空格和 tab 找到第一个字符 */

    StringWrapper wrapper;
    wrapper.buf = buf;
    wrapper.len = n;

    /* 跳过空格和 tab 找到第一个字符 */
    Strip(&wrapper);
    /* 如果字符串的数量不足 3 个，肯定有错误 */
    if (wrapper.len < 3) {
        return -1;
    }

    /* 循环解析 wrapper 中的内容 */
    while (wrapper.len > 0) {
        char ch = *wrapper.buf;
        ++wrapper.buf;
        --wrapper.len;

        if (rc < 0) {
            /* 前面的程序已经执行出错了 */
            break;
        }

        switch (ch) {
        case '\r':
            /* 只是换行，说明程序执行结束了 */
            rc = 0;
            break;
        case '\n':
            /* 只是换行，说明程序执行结束了 */
            rc = 0;
            break;
        case 'A':
            rc = cmd_first_is_A(compiler, &wrapper, isMdi);
            break;
        case 'B':
            rc = cmd_first_is_B(compiler, &wrapper, isMdi);
            break;
        case 'C':
            rc = cmd_first_is_C(compiler, &wrapper, isMdi);
            break;
        case 'D':
            rc = cmd_first_is_D(compiler, &wrapper, isMdi);
            break;
        case 'E':
            rc = cmd_first_is_E(compiler, &wrapper, isMdi);
            break;
        case 'F':
            rc = cmd_first_is_F(compiler, &wrapper, isMdi);
            break;
        case 'G':
            rc = cmd_first_is_G(compiler, &wrapper, isMdi);
            break;
        case 'H':
            rc = cmd_first_is_H(compiler, &wrapper, isMdi);
            break;
        case 'I':
            rc = cmd_first_is_I(compiler, &wrapper, isMdi);
            break;
        case 'J':
            rc = cmd_first_is_J(compiler, &wrapper, isMdi);
            break;
        case 'L':
            rc = cmd_first_is_L(compiler, &wrapper, isMdi);
            break;
        case 'M':
            rc = cmd_first_is_M(compiler, &wrapper, isMdi);
            break;
        case 'P':
            rc = cmd_first_is_P(compiler, &wrapper, isMdi);
            break;
        case 'R':
            rc = cmd_first_is_R(compiler, &wrapper, isMdi);
            break;
        case 'S':
            rc = cmd_first_is_S(compiler, &wrapper, isMdi);
            break;
        case 'T':
            rc = cmd_first_is_T(compiler, &wrapper, isMdi);
            break;
        case 'U':
            rc = cmd_first_is_U(compiler, &wrapper, isMdi);
            break;
        case 'V':
            rc = cmd_first_is_V(compiler, &wrapper, isMdi);
            break;
        case 'W':
            rc = cmd_first_is_W(compiler, &wrapper, isMdi);
            break;
        case '_':
            rc = cmd_first_is__(compiler, &wrapper, isMdi);
            break;
        case '^':
            rc = cmd_first_is_Shift6(compiler, &wrapper, isMdi);
            break;
        default:
            /* 错误指令 */
            rc = -1;
            break;
        }
    }

    return rc;
}
