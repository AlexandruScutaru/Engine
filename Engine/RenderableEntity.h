#ifndef RENDERABLEENTITY_H
#define RENDERABLEENTITY_H

#include <GLM/glm.hpp>


namespace renderer{

	class TexturedModel;

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
