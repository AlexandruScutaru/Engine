#include "BillBoard.h"

namespace renderer{

	BillBoard::BillBoard(){}

	BillBoard::BillBoard(unsigned int texturedModel) :
		GameObject(texturedModel)
	{}

	BillBoard::~BillBoard(){}

	void BillBoard::update(float deltaTime){}

	void BillBoard::prepareToRender(){}

	void BillBoard::render(){}

}