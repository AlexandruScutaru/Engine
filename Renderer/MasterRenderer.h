#ifndef MASTER_RENDERER_H
#define MASTER_RENDERER_H

#include "Camera.h"
#include "Lights.h"
#include "Entity.h"
#include "BillBoard.h"
#include "ShaderProgram.h"

namespace renderer{

	class MasterRenderer{
	public:
		MasterRenderer();
		~MasterRenderer();

		void init();

		void renderScene(std::vector<Entity*>& entities,
						 std::vector<BillBoard*>& billboards,
						 DirLight& dirLight,
						 std::vector<PointLight>& pointLights,
						 SpotLight& spotLight,
						 Camera& camera
		);

		void renderSingleEntity(TexturedModel* object, DirLight& sun, Camera& camera);
		void renderBoundingBox(TexturedModel* object, glm::vec3& scale, glm::vec3& rot, Camera& camera);

	private:
		void MasterRenderer::processObject(Entity* entity);
		void MasterRenderer::processObject(BillBoard* billboard);
		void render();
		void renderEntities();
		void renderBillBoard();
		void setUniforms(DirLight& dirLight, std::vector<PointLight>& pointLights, SpotLight& spotLight, Camera& camera);

		std::map<TexturedModel*, std::vector<Entity*>> m_entityBatches;
		std::map<TexturedModel*, std::vector<BillBoard*>> m_billboardBatches;

		ShaderProgram m_entityShader;
		ShaderProgram m_billBoardShader;
		ShaderProgram m_basicShader;

	};

}
#endif // !MASTER_RENDERER_H
