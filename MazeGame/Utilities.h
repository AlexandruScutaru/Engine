#ifndef UTILITIES_H
#define UTILITIES_H

#include <map>
#include <vector>
#include <GLM/glm.hpp>

namespace physics{
	class PhysicsWorld;
}
namespace renderer{
	class TexturedModel;
}
class GameObject;


class Utilities{
public:
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

	static GameObject* Utilities::OpenGameObject(const std::string& file, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, physics::PhysicsWorld* world);

private:
	Utilities();

};

#endif // !UTILITIES_H
