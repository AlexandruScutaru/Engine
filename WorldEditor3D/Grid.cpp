#include "Grid.h"


float Grid::m_gridStep = 0.16f;
float Grid::m_gridHeight = 0.0f;
bool  Grid::m_showGrid = false;
bool  Grid::m_snapToGrid = false;


Grid::Grid(){}


void Grid::setStep(float step){
	int val = step * 100;
	if((val != 0) && ((val & (val - 1)) == 0)){
		m_gridStep = step;
	}
}

void Grid::incHeight(){
	m_gridHeight += 2.56f;
}

void Grid::decHeight(){
	m_gridHeight -= 2.56f;
}

void Grid::setEnabled(bool value){
	m_showGrid = value;
}

void Grid::setSnapEnabled(bool value){
	m_snapToGrid = value;
}

float Grid::getHeight(){
	return m_gridHeight;
}

float Grid::getStep(){
	return m_gridStep;
}

bool Grid::isEnabled(){
	return m_showGrid;
}

bool Grid::isSnapEnabled(){
	return m_snapToGrid;
}
