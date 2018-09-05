#ifndef RENDERABLEENTITY_H
#define RENDERABLEENTITY_H

#include <string>
#include <vector>

#include <GLM/glm.hpp>
#include "TexturedModel.h"


namespace renderer{

	class RenderableEntity{
	public:
		RenderableEntity();
		RenderableEntity(TexturedModel* model);
		RenderableEntity(const RenderableEntity& other);
		
		virtual ~RenderableEntity();

		void setTexturedModel(TexturedModel* model){ m_model = model; }
		TexturedModel* getTexturedModel(){ return m_model; }


	protected:
		TexturedModel* m_model;
	
	};

}

#endif // !RENDERABLEENTITY_H
