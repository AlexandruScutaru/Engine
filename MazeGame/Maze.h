#ifndef MAZE_H
#define MAZE_H

#include <fstream>
#include <vector>

class Maze{
public:
	Maze(int numRows, int numCols);
	Maze(const Maze& other) = delete;
	Maze& operator=(const Maze& other) = delete;
	~Maze();

	enum class Cells : int{
		F = 0,
		T,
		S,
		E
	};

	friend std::ostream& operator<<(std::ostream& out, Maze const& maze);
	std::vector<std::vector<Maze::Cells>> getMaze();

private:
	void generateMaze();
	void recursion(int r, int c);
	void genRandDirs();
	void deallocateData();
	int m_rows;
	int m_cols;
	int** m_maze;
	int m_dirs[4];
};

#endif // !MAZE_H
