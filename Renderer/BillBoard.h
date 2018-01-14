#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "GameObject.h"

namespace renderer{

	class BillBoard : public GameObject{
	public:
		BillBoard();
		BillBoard(unsigned int texturedModel);
		~BillBoard();

		void update(float deltaTime);
		void prepareToRender();
		void render();

	private:

	};

}

#endif // !BILLBOARD_H


