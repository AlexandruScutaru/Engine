#ifndef GRID_H
#define GRID_H


class Grid{
public:
	static void setStep(float step);
	static void incHeight();
	static void decHeight();
	static void setEnabled(bool value);

	static float getHeight();
	static float getStep();
	static bool isEnabled();

private:
	static float m_gridStep;
	static float m_gridHeight;
	static bool m_showGrid;
	
	Grid();

};

#endif // !GRID_H
