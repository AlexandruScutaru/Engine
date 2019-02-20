#ifndef UTILITIES_H
#define UTILITIES_H

#include "GameObject.h"
#include <map>
#include <vector>

class MainApp;

class Utilities{
public:

	static void openMap(MainApp* app, const std::string& file);
	static void saveMap(MainApp* app, const std::string& file);
	static void saveCreatedObject(MainApp* app, const std::string& buf);
	static void openCreatedObject(MainApp* app, const std::string& object);


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

	static bool checkIfMapCanBeSaved(MainApp* app);
};

#endif // !UTILITIES_H
