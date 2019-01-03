#ifndef RENDER_UTILITIES_H
#define RENDER_UTILITIES_H

#include <vector>

class MainApp;
class Actor;

namespace renderer{
	struct CollisionBody;
}

class RenderUtilities{
public:
	static void DrawGameObjects(MainApp* app, bool drawCollisionBodies);
	static void DrawLightBillboards(MainApp* app);
	static void DrawCollisionBodies(MainApp* app, std::vector<renderer::CollisionBody*>& colBodies);
	static void PrePixelPickDraw(MainApp* app);
	static void DrawTransformGizmos(MainApp* app, bool useColorCode = false);
	static void DrawLines(MainApp* app);
	static void DrawGrid(MainApp* app);

private:
	RenderUtilities() {}
	static std::vector<Actor*> GetPixelPickActors(MainApp* app);
};

#endif // !RENDER_UTILITIES_H
