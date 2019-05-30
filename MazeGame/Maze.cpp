#include "Maze.h"

#include <ctime>
#include <algorithm>


Maze::Maze(int numRows, int numCols):
	m_rows(numRows),
	m_cols(numCols)
{
	m_maze = new int*[m_rows];
	for(int i = 0; i < m_rows; i++){
		m_maze[i] = new int[m_cols];
		std::fill(m_maze[i], m_maze[i] + m_cols, 1);
	}
	generateMaze();
}

Maze::~Maze(){
	deallocateData();
}

std::vector<std::vector<Maze::Cells>> Maze::getMaze(){
	std::vector<std::vector<Maze::Cells>> data;
	if(m_maze){
		data.reserve(m_rows);
		for(int i = 1; i < m_rows; i += 2){
			std::vector<Maze::Cells> row;
			row.reserve(m_cols);
			for(int j = 1; j < m_cols; j += 2)
				if(m_maze[i - 1][j] == 1 && m_maze[i][j - 1])
					row.push_back(Cells::F);
				else if(m_maze[i - 1][j] == 1)
					row.push_back(Cells::T);
				else if(m_maze[i][j - 1] == 1)
					row.push_back(Cells::S);
				else
					row.push_back(Cells::E);

			row.push_back(Cells::F);
			data.push_back(row);
		}
		data.push_back(std::vector<Maze::Cells>(m_cols / 2 + 1, Cells::F));
	}

	return data;
}

void Maze::generateMaze(){
	// r for row, c for column
	// Generate random r
	int r = rand() % m_rows;
	while(!(r & 1))
		r = rand() % m_rows;

	// Generate random c
	int c = rand() % m_cols;
	while(c % 2 == 0)
		c = rand() % m_cols;

	// Starting cell
	m_maze[r][c] = 0;

	recursion(r, c);
}

void Maze::recursion(int r, int c){
	// 4 random directions
	genRandDirs();
	// Examine each direction
	for(int i = 0; i < 4; i++) {
		switch(m_dirs[i]){
		case 1: // Up
				//Whether 2 cells up is out or not
			if(r - 2 <= 0)
				continue;
			if(m_maze[r - 2][c] != 0) {
				m_maze[r - 2][c] = 0;
				m_maze[r - 1][c] = 0;
				recursion(r - 2, c);
			}
			break;
		case 2: // Right
				// Whether 2 cells to the right is out or not
			if(c + 2 >= m_cols - 1)
				continue;
			if(m_maze[r][c + 2] != 0) {
				m_maze[r][c + 2] = 0;
				m_maze[r][c + 1] = 0;
				recursion(r, c + 2);
			}
			break;
		case 3: // Down
				// Whether 2 cells down is out or not
			if(r + 2 >= m_rows - 1)
				continue;
			if(m_maze[r + 2][c] != 0) {
				m_maze[r + 2][c] = 0;
				m_maze[r + 1][c] = 0;
				recursion(r + 2, c);
			}
			break;
		case 4: // Left
				// Whether 2 cells to the left is out or not
			if(c - 2 <= 0)
				continue;
			if(m_maze[r][c - 2] != 0) {
				m_maze[r][c - 2] = 0;
				m_maze[r][c - 1] = 0;
				recursion(r, c - 2);
			}
			break;
		}
	}
}

void Maze::genRandDirs(){
	for(int i = 0; i < 4; i++)
		m_dirs[i] = i + 1;
	std::random_shuffle(m_dirs, m_dirs + 4);
}

void Maze::deallocateData(){
	if(m_maze){
		for(int i = 0; i < m_rows; i++)
			delete[] m_maze[i];
		delete[] m_maze;
		m_maze = nullptr;
	}
}

std::ostream & operator<<(std::ostream & out, Maze const & maze){
	if(!maze.m_maze){
		out << "null" << std::endl;
	} else {
		for(int i = 0; i < maze.m_rows; i++){
			for(int j = 0; j < maze.m_cols; j++)
				out << maze.m_maze[i][j];
			out << std::endl;
		}
	}
	return out;
}
