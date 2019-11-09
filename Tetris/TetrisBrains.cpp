#pragma once
#include "Tetris.h"
#include <Windows.h>


int DrawGrid(HDC hdc, int* piGrid) {
	int pow = 1;
	for (int i = 0; i < GRID_HEIGHT; i++) {
		pow = 1;
		for (int j = 0; j < GRID_WIDTH; j++) {
			if (piGrid[i] & pow) {
				Rectangle(hdc, j * SCALE, i * SCALE, (j + 1) * SCALE, (i + 1) * SCALE);
			}
			pow = pow << 1;
		}
	}

	HGDIOBJ original = NULL;
	original = SelectObject(hdc, GetStockObject(DC_PEN));
	COLORREF oldColor = SetDCPenColor(hdc, RGB(0, 255, 255));

	for (int i = 0; i < GRID_HEIGHT; i++) {
		pow = 1;
		for (int j = 0; j < GRID_WIDTH; j++) {
			if (!(piGrid[i] & pow)) {
				Rectangle(hdc, j * SCALE, i * SCALE, (j + 1) * SCALE, (i + 1) * SCALE);
			}
			pow = pow << 1;
		}
	}

	SelectObject(hdc, original);
	return 0;
}
int PushFig(HDC hdc, int* Grid, FigInfo* figInfo) {
	DeleteFig(hdc, Grid, figInfo);
	int a = 0;
	for (int i = 0; i < FIG_SIZE; i++) {
		int currFigLine = ((figInfo->fig)[i]);

		if (currFigLine && (i + (figInfo->x) + 1 >= GRID_HEIGHT)) {
			a = 1;
			break;
		}

		int nextGridLine = Grid[i + (figInfo->x) + 1];

		a = (nextGridLine & currFigLine) | a;

	}
	if (a) {
		InsertFig(hdc, Grid, figInfo);
		CheckLines(hdc, Grid);
		figInfo->x = 0;
		InsertFig(hdc, Grid, figInfo);
	}
	else {

		(figInfo->x)++;
		InsertFig(hdc, Grid, figInfo);
	}



	return a;
}

int DrawFig(HDC hdc, FigInfo* figInfo) {
	int pow = 1;
	for (int i = 0; i < FIG_SIZE; i++) {
		pow = 1;
		for (int j = 0; j < GRID_WIDTH; j++) {
			if (figInfo->fig[i] & pow) {
				Rectangle(
					hdc, j * SCALE,
					(i + figInfo->x) * SCALE,
					(j + 1) * SCALE,
					(i + 1 + figInfo->x) * SCALE
				);
			}
			pow = pow << 1;
		}
	}
	return 0;
}

//possible abstraction: make general func with lambda as a parameter
int DeleteFig(HDC hdc, int* Grid, FigInfo* figInfo) {
	for (int i = 0; i < FIG_SIZE; i++) {
		Grid[i + figInfo->x] = Grid[i + figInfo->x] - figInfo->fig[i];
		//Grid[i] = (unsigned int)0;

	}
	//    Initializing original object
	HGDIOBJ original = NULL;


	original = SelectObject(hdc, GetStockObject(DC_PEN));
	COLORREF oldColor = SetDCPenColor(hdc, RGB(0, 255, 255));
	DrawFig(hdc, figInfo);
	SelectObject(hdc, original);
	return 0;
}
int InsertFig(HDC hdc, int* Grid, FigInfo* figInfo) {
	for (int i = 0; i < GRID_HEIGHT; i++) {
		for (int i = 0; i < FIG_SIZE; i++) {
			Grid[i + figInfo->x] = Grid[i + figInfo->x] | figInfo->fig[i];
		}
	}
	DrawFig(hdc, figInfo);
	return 0;
}



//TODO:CHANGE THE DESIGN!! DO THE DELETE INSERT STAFF IN DIF WAY!
//POSSIBLY, WE SHOULD MOVE FLIP LOGIC IN SEVERAL METHODS


int MoveFig(HDC hdc, int* Grid, FigInfo* figInfo, WPARAM wParam) {

	switch (wParam) {
	case VK_LEFT:

		if (CheckLeft(Grid, figInfo)) {
			DeleteFig(hdc, Grid, figInfo);
			for (int i = 0; i < FIG_SIZE; i++) {
				figInfo->fig[i] = figInfo->fig[i] >> 1;
			}
			InsertFig(hdc, Grid, figInfo);
		}
		break;
	case VK_RIGHT:
		if (CheckRight(Grid, figInfo)) {
			DeleteFig(hdc, Grid, figInfo);
			for (int i = 0; i < FIG_SIZE; i++) {
				figInfo->fig[i] = figInfo->fig[i] << 1;
			}
			InsertFig(hdc, Grid, figInfo);
		}
		break;
	case VK_SPACE:
		DeleteFig(hdc, Grid, figInfo);
		Flip(figInfo);
		InsertFig(hdc, Grid, figInfo);
		break;
	case VK_DOWN:
		PushFig(hdc, Grid, figInfo);
		break;
	}

	return 0;
}

int CheckLines(HDC hdc, int* piGrid) {

	
	int i = GRID_HEIGHT - 1, j = GRID_HEIGHT - 1;
	for (; i > 0; i--, j--) {
		while (piGrid[i] == FULL_LINE) {
			i--;
		}
		piGrid[j] = piGrid[i];
	}
	for (; j >= 0; j--) {
		piGrid[j] = 0;
	}
	DrawGrid(hdc, piGrid);
	return 0;
}


//TODO: Generalize with checkRIght
int CheckLeft(int* Grid, FigInfo* figInfo) {
	int leftBorder = 1;
	int result = 0;
	for (int i = 0; i < FIG_SIZE; i++) {
		int currFigLine = figInfo->fig[i];
		int currGridLine = Grid[i + (figInfo->x)] & ~currFigLine;
		result |= leftBorder & currFigLine;
		result |= (currFigLine >> 1) & currGridLine;
		if (result)
			break;
	}
	return !result;
}
//TODO: Generalize with checkLeft
int CheckRight(int* Grid, FigInfo* figInfo) {

	//static const int rightBorder = 1 << (GRID_WIDTH - 1);
	int result = 0;
	for (int i = 0; i < FIG_SIZE; i++) {
		int currFigLine = figInfo->fig[i];
		int currGridLine = Grid[i + (figInfo->x)] & ~currFigLine;
		result |= RIGHT_BORDER & currFigLine;
		result |= (currFigLine << 1) & currGridLine;
		if (result)
			break;
	}
	return !result;
}

int Flip(FigInfo* figInfo) {
	FigInfo result = { 0,0,0,0,0 };			//BAD PRACTISE
	int leftBorder = RIGHT_BORDER;
	for (int i=0; i < FIG_SIZE; i++) {
		if (figInfo->fig[i] == 0) continue;
		int curLeftBorder = 0;
		for (int j = 1;j< RIGHT_BORDER; j = j << 1) {
			curLeftBorder = j;
			if ((j & figInfo->fig[i])) break;
		}
		if (curLeftBorder < leftBorder)leftBorder = curLeftBorder;
	}

	for (int i = 0, figInfoCar=leftBorder; i < FIG_SIZE; i++, figInfoCar = figInfoCar <<1) {
		for (int j = 0, resCar = leftBorder /*leftBorder*/; j < FIG_SIZE; j++, resCar = resCar << 1) {
			result.fig[i] |= (figInfoCar) & (figInfo->fig[j])? resCar:0;
		}
		
	}
	for (int i = 0; i < FIG_SIZE; i++) {
		figInfo->fig[i] = result.fig[i];
	}
	return 0;
}