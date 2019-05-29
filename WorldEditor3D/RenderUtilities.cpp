#include "RenderUtilities.h"
#include "MainApp.h"
#include "Utilities.h"

#include <glm/gtc/matrix_transform.hpp>

void RenderUtilities::DrawGameObjects(MainApp* app, bool drawCollisionBodies){
	std::vector<renderer::CollisionBody*> colBodies;
	///first draw the normal gameobjects
	//prepare shader
	
	app->m_gameObjectsShader.use();

	if(app->m_gui.b_creationTab) {
		app->m_gameObjectsShader.loadLights(std::vector<renderer::Light*>{(renderer::Light*)&app->m_creationTabLight});
	} else {
		app->m_gameObjectsShader.loadFlashlight(app->m_player.isFlashLightOn());
		app->m_gameObjectsShader.loadLights(app->m_lights);
	}
	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_gameObjectsShader.loadViewPosition(app->m_player.getCamera()->getPos());
	app->m_gameObjectsShader.loadViewMatrix(view);
	app->m_gameObjectsShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	//get textured model batches
	auto batches = Utilities::BatchRenderables<GameObject>(app->m_objects_ToDraw);
	for(auto const& batch : batches){
		if(batch.first->isBillboard())
			continue;
		if(batch.first->isDoubleSided()) 
			renderer::Renderer::disableBackFaceCulling();
		app->m_gameObjectsShader.loadAtlasSize(batch.first->getAtlasSize());
		renderer::Renderer::BindTexturedModel(batch.first);
		for(auto const& gameObject : batch.second){
			app->m_gameObjectsShader.loadSelected(gameObject->isSelected());
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
			modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());
			modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
			app->m_gameObjectsShader.loadModelMatrix(modelMatrix);
			app->m_gameObjectsShader.loadAtlasOffset(gameObject->getTextureOffset());
			renderer::Renderer::DrawTexturedModel(batch.first);
		}
		renderer::Renderer::enableBackFaceCulling();
		if(drawCollisionBodies){
			std::vector<renderer::CollisionBody>* cb = &batch.second[0]->getColBodies();
			for(auto& body : *cb)
				colBodies.push_back(&body);
		}
	}
	app->m_gameObjectsShader.unuse();

	///now draw the billboards
	//prepare shader
	app->m_billboardShader.use();

	app->m_billboardShader.loadViewMatrix(view);
	app->m_billboardShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	for(auto const& batch : batches){
		if(batch.first->isBillboard()){
			renderer::Renderer::BindTexturedModel(batch.first);
			for(auto const& gameObject : batch.second){
				app->m_billboardShader.loadSelected(gameObject->isSelected());
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
				modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());

				modelMatrix[0][0] = view[0][0];
				modelMatrix[0][1] = view[1][0];
				modelMatrix[0][2] = view[2][0];
				modelMatrix[1][0] = view[0][1];
				modelMatrix[1][1] = view[1][1];
				modelMatrix[1][2] = view[2][1];
				modelMatrix[2][0] = view[0][2];
				modelMatrix[2][1] = view[1][2];
				modelMatrix[2][2] = view[2][2];
				modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
				app->m_billboardShader.loadModelMatrix(modelMatrix);

				renderer::Renderer::DrawTexturedModel(batch.first);
			}
			if(drawCollisionBodies){
				std::vector<renderer::CollisionBody>* cb = &batch.second[0]->getColBodies();
				for(auto& body : *cb)
					colBodies.push_back(&body);
			}
		}
	}
	app->m_billboardShader.unuse();

	if(drawCollisionBodies)
		DrawCollisionBodies(app, colBodies);
}

void RenderUtilities::DrawLightBillboards(MainApp * app){
	auto batches = Utilities::BatchRenderables<LightBillboard>(app->m_lightsBillboards);
	app->m_billboardShader.use();

	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_billboardShader.loadViewMatrix(view);
	app->m_billboardShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	for(auto const& batch : batches){
		if(batch.first->isBillboard()){
			renderer::Renderer::BindTexturedModel(batch.first);
			for(auto const& gameObject : batch.second){
				app->m_billboardShader.loadSelected(gameObject->isSelected());
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
				modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());

				modelMatrix[0][0] = view[0][0];
				modelMatrix[0][1] = view[1][0];
				modelMatrix[0][2] = view[2][0];
				modelMatrix[1][0] = view[0][1];
				modelMatrix[1][1] = view[1][1];
				modelMatrix[1][2] = view[2][1];
				modelMatrix[2][0] = view[0][2];
				modelMatrix[2][1] = view[1][2];
				modelMatrix[2][2] = view[2][2];
				modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
				app->m_billboardShader.loadModelMatrix(modelMatrix);

				renderer::Renderer::DrawTexturedModel(batch.first);
			}
		}
	}
	app->m_billboardShader.unuse();
}

void RenderUtilities::DrawColVolumesBillboards(MainApp * app){
	auto batches = Utilities::BatchRenderables<CollisionVolumeBillboard>(app->m_colVolumeBillboards);
	app->m_billboardShader.use();

	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_billboardShader.loadViewMatrix(view);
	app->m_billboardShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	for(auto const& batch : batches){
		if(batch.first->isBillboard()){
			renderer::Renderer::BindTexturedModel(batch.first);
			for(auto const& gameObject : batch.second){
				app->m_billboardShader.loadSelected(gameObject->isSelected());
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
				modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());

				modelMatrix[0][0] = view[0][0];
				modelMatrix[0][1] = view[1][0];
				modelMatrix[0][2] = view[2][0];
				modelMatrix[1][0] = view[0][1];
				modelMatrix[1][1] = view[1][1];
				modelMatrix[1][2] = view[2][1];
				modelMatrix[2][0] = view[0][2];
				modelMatrix[2][1] = view[1][2];
				modelMatrix[2][2] = view[2][2];
				modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
				app->m_billboardShader.loadModelMatrix(modelMatrix);

				renderer::Renderer::DrawTexturedModel(batch.first);
			}
		}
	}
	app->m_billboardShader.unuse();

	std::vector<renderer::CollisionBody*> bodies = {};
	for(auto const& colVolume : app->m_colVolumeBillboards)
		bodies.push_back(colVolume->getColBodyPtr());

	DrawCollisionBodies(app, bodies, glm::vec3(0.05f, 0.875f, 0.8125f));

}

void RenderUtilities::DrawCollisionBodies(MainApp* app, std::vector<renderer::CollisionBody*>& colBodies, glm::vec3& color){
	if(!colBodies.size())
		return;

	// view/projection transformations
	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_basicColorShader.use();
	app->m_basicColorShader.loadViewMatrix(view);
	app->m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	app->m_basicColorShader.loadColor(color);
	renderer::TexturedModel* td;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	renderer::Renderer::disableBackFaceCulling();

	for(auto body : colBodies){
		glm::mat4 model;
		model = glm::translate(model, body->colRelativePos);
		model = model * glm::toMat4(body->colRotQuat);
		model = glm::scale(model, body->colScale);
		app->m_basicColorShader.loadModelMatrix(model);

		td = utilities::ResourceManager::loadModel(body->shape);
		glBindVertexArray(td->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, td->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	app->m_basicColorShader.unuse();

	renderer::Renderer::enableBackFaceCulling();
}

void RenderUtilities::PrePixelPickDraw(MainApp* app){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 colorCode;

	app->m_basicColorShader.use();
	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_basicColorShader.loadViewMatrix(view);
	app->m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	auto actors = GetPixelPickActors(app);
	for(auto actor : actors){
		glm::mat4 model;
		if(actor->isBillboard()){
			model = glm::translate(model, actor->getPosition());
			model = model * glm::toMat4(actor->getRotation());

			model[0][0] = view[0][0];
			model[0][1] = view[1][0];
			model[0][2] = view[2][0];
			model[1][0] = view[0][1];
			model[1][1] = view[1][1];
			model[1][2] = view[2][1];
			model[2][0] = view[0][2];
			model[2][1] = view[1][2];
			model[2][2] = view[2][2];
			model = glm::scale(model, actor->getScale());
		} else{
			model = glm::translate(model, actor->getPosition());
			model = model * glm::toMat4(actor->getRotation());
			model = glm::scale(model, actor->getScale());
		}

		app->m_basicColorShader.loadModelMatrix(model);
		int code = actor->getCode();
		colorCode = glm::vec3(
			((code & 0x000000FF) >> 0) / 255.0f,
			((code & 0x0000FF00) >> 8) / 255.0f,
			((code & 0x00FF0000) >> 16) / 255.0f
		);
		app->m_basicColorShader.loadColor(colorCode);
		if(actor->isDoubleSided())
			renderer::Renderer::disableBackFaceCulling();
		glBindVertexArray(actor->getTexturedModel()->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, actor->getTexturedModel()->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
		renderer::Renderer::enableBackFaceCulling();
	}
	DrawTransformGizmos(app, true);
}

void RenderUtilities::DrawTransformGizmos(MainApp* app, bool useColorCode){
	if(!app->m_gizmos.canBeShown() || app->m_gui.b_creationTab)
		return;

	glClear(GL_DEPTH_BUFFER_BIT);
	const std::vector<Gizmo>* gzms = app->m_gizmos.getGizmos();

	app->m_basicColorShader.use();
	// view/projection transformations
	glm::mat4 view = app->m_player.getCamera()->getViewMatrix();
	app->m_basicColorShader.loadViewMatrix(view);
	app->m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	glm::vec3 scale = glm::vec3(0.5f) * glm::distance(app->m_gizmos.getPosition(), app->m_player.getCamera()->getPos());
	renderer::TexturedModel* td;
	for(auto gzm : *gzms){
		glm::mat4 model;
		model = glm::translate(model, app->m_gizmos.getPosition());

		if(app->m_gizmos.getGizmoMode() == (int)GizmoMode::ROTATE){
			auto baseRot = gzm.obj->getRotation();
			model = model * glm::toMat4(app->m_gizmos.getRot() * baseRot);
		} else{
			model = model * glm::toMat4(gzm.obj->getRotation());
		}
		if(useColorCode){
			int code = gzm.obj->getCode();
			glm::vec3 colorCode = glm::vec3(
				((code & 0x000000FF) >> 0) / 255.0f,
				((code & 0x0000FF00) >> 8) / 255.0f,
				((code & 0x00FF0000) >> 16) / 255.0f
			);
			app->m_basicColorShader.loadColor(colorCode);
		} else {
			app->m_basicColorShader.loadColor(gzm.color);
		}

		model = glm::scale(model, scale);

		app->m_basicColorShader.loadModelMatrix(model);

		td = gzm.obj->getTexturedModel();
		glBindVertexArray(td->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, td->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}
	app->m_basicColorShader.unuse();
}

void RenderUtilities::DrawLines(MainApp* app){
	///bind the line VAO
	glBindVertexArray(app->m_lineVAO);
	///tell opengl to draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///load appropiate uniforms
	app->m_basicColorShader.use();
	app->m_basicColorShader.loadViewMatrix(app->m_player.getCamera()->getViewMatrix());
	app->m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	glm::mat4 model;
	///directional lines
	//x
	model = glm::translate(model, glm::vec3(0.0f));
	app->m_basicColorShader.loadColor(glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	app->m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);
	//y
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f));
	app->m_basicColorShader.loadColor(glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	app->m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);
	//z
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f));
	app->m_basicColorShader.loadColor(glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	app->m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);

	///tell opengl to draw filled polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	///unbind line VAO
	glBindVertexArray(0);
	///stop using this shader
	app->m_basicColorShader.unuse();
}

void RenderUtilities::DrawGrid(MainApp* app){
	///bind the line VAO
	glBindVertexArray(app->m_lineVAO);
	///tell opengl to draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///load appropiate uniforms
	app->m_basicColorShader.use();
	app->m_basicColorShader.loadViewMatrix(app->m_player.getCamera()->getViewMatrix());
	app->m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	app->m_basicColorShader.loadColor(glm::vec3(0.3f, 0.3f, 0.3f));

	glm::vec3 gridPosition;
	gridPosition.x = std::round(app->m_player.getPosition().x / Grid::getStep()) * Grid::getStep();
	gridPosition.y = Grid::getHeight();
	gridPosition.z = std::round(app->m_player.getPosition().z / Grid::getStep()) * Grid::getStep();

	float scaleFactor = 0.069f;
	int ord = 0;
	while(Grid::getStep() * ord <= 7.0f){
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(gridPosition.x, Grid::getHeight(), gridPosition.z + Grid::getStep() * ord));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		app->m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(gridPosition.x, Grid::getHeight(), gridPosition.z - Grid::getStep() * ord));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		app->m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		ord++;
	}
	ord = 0;
	while(Grid::getStep() * ord <= 7.0f){
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(gridPosition.x + Grid::getStep() * ord, Grid::getHeight(), gridPosition.z));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		app->m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(gridPosition.x - Grid::getStep() * ord, Grid::getHeight(), gridPosition.z));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		app->m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		ord++;
	}

	///tell opengl to draw filled polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	///unbind line VAO
	glBindVertexArray(0);
	///stop using this shader
	app->m_basicColorShader.unuse();
}

std::vector<Actor*> RenderUtilities::GetPixelPickActors(MainApp * app){
	std::vector<Actor*> objects;
	for(auto gameobject : app->m_objectsInScene)
		objects.push_back(static_cast<Actor*>(gameobject));
	for(auto light : app->m_lightsBillboards)
		objects.push_back(static_cast<Actor*>(light));
	for(auto colVolume : app->m_colVolumeBillboards)
		objects.push_back(static_cast<Actor*>(colVolume));

	return objects;
}
