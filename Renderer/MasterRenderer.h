#ifndef MASTER_RENDERER_H
#define MASTER_RENDERER_H

#include "Camera.h"
#include "Lights.h"
#include "GameObject.h"
#include "ShaderProgram.h"

namespace renderer{

	class MasterRenderer{
	public:
		MasterRenderer();
		~MasterRenderer();

		void init();

		//the lights vector is as follows
		//at 0 = the directional light (only one - "sun")
		//at 1 = the spotlight (only one - flashlight)
		//-to_end point lights
		void renderScene(std::vector<GameObject*>& gameObjects,
						 std::vector<Light*>& lights,
						 Camera& camera
		);
		
		void setProjectionMatrix(Camera& camera);

		void renderSingleEntity(TexturedModel* object, DirLight& sun, Camera& camera);
		void renderCollisionBodies(std::vector<CollisionBody>& bodies, Camera& camera);
		int pixelPick(std::vector<GameObject*> objects, Camera& camera, glm::vec2& coords);

	private:
		void MasterRenderer::processObject(GameObject* gameObject);
		void MasterRenderer::processBillboard(GameObject* billboard);

		void render();
		void renderGameObjects();
		void renderBillBoards();
		void setUniforms(std::vector<Light*>& lights, Camera& camera);

		std::map<TexturedModel*, std::vector<GameObject*>> m_gameObjectsBatches;
		std::map<TexturedModel*, std::vector<GameObject*>> m_billboardBatches;

		ShaderProgram m_entityShader;
		ShaderProgram m_billBoardShader;
		ShaderProgram m_basicShader;
		ShaderProgram m_selectShader;

		bool m_flashlight;

		glm::mat4 m_projection;
		glm::mat4 m_view;

	};

}
#endif // !MASTER_RENDERER_H
