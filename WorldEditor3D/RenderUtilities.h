#ifndef RENDER_UTILITIES_H
#define RENDER_UTILITIES_H

#include <vector>
#include <GLM/glm.hpp>

class MainApp;
class Actor;

namespace renderer{
	struct CollisionBody;
}

class RenderUtilities{
public:
	static void DrawLightDepthMap(MainApp* app);
	static void DrawGameObjects(MainApp* app, bool drawCollisionBodies);
	static void DrawLightBillboards(MainApp* app);
	static void DrawColVolumesBillboards(MainApp* app);
	static void DrawCollisionBodies(MainApp* app, std::vector<renderer::CollisionBody*>& colBodies, glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
	static void PrePixelPickDraw(MainApp* app);
	static void DrawTransformGizmos(MainApp* app, bool useColorCode = false);
	static void DrawLines(MainApp* app);
	static void DrawGrid(MainApp* app);

private:
	RenderUtilities() {}
	static std::vector<Actor*> GetPixelPickActors(MainApp* app);
};

#endif // !RENDER_UTILITIES_H
