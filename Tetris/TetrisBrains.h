#pragma once
#define GRID_HEIGHT 16
#define GRID_WIDTH 8
#define FIG_SIZE 4
#define SCALE 30
#include <Windows.h>
struct FigInfo
{
	int x; int fig[FIG_SIZE];
};
int DrawGrid(HDC hdc, int* piGrid);
int CheckLines(HDC hdc, int* piGrid);
int PushFig(HDC hdc, int* Grid, FigInfo* figInfo);
int InsertFig(HDC hdc, int* Grid, FigInfo* figInfo);
int DeleteFig(HDC hdc, int* Grid, FigInfo* figInfo);
int MoveFig(HDC hdc, int* Grid, FigInfo* figInfo, WPARAM wParam);

int CheckLeft(int* Grid, FigInfo* figInfo);
int CheckRight(int* Grid, FigInfo* figInfo); 