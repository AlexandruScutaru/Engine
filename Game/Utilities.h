#ifndef UTILITIES_H
#define UTILITIES_H

#include "GameObject.h"
#include "Player.h"

#include <map>
#include <vector>

class GameObject;
class CollisionVolume;
class TerrainShader;
namespace renderer{
	struct Light;
	class Skybox;
	class Terrain;
}
namespace physics{
	class PhysicsWorld;
}

class Utilities{
public:
	static void OpenMap(const std::string& file, 
						std::vector<GameObject*>& objects, 
						std::vector<CollisionVolume*>& colVols, 
						std::vector<renderer::Light*>& lights, 
						physics::PhysicsWorld* world, 
						Player* player, 
						renderer::Skybox& skybox,
						renderer::Terrain& terrain,
						TerrainShader& terrainShader);

	static GameObject* OpenGameObject(const std::string& file, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, physics::PhysicsWorld* world);

	template <typename T> 
	static std::map<renderer::TexturedModel*, std::vector<T*>> BatchRenderables(std::vector<T*>& entities){
		std::map<renderer::TexturedModel*, std::vector<T*>> batches;

		for(auto entity : entities){			
			renderer::TexturedModel* model = entity->getTexturedModel();
			std::vector<T*> batch = batches[model];
			if(!batch.empty()){
				batch.push_back(entity);
				batches[model] = batch;
			} else{
				std::vector<T*> newBatch;
				newBatch.push_back(entity);
				batches[model] = newBatch;
			}
		}
		return batches;
	}

private:
	Utilities();

};

#endif // !UTILITIES_H
