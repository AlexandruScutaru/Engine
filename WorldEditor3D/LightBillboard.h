#ifndef LIGHT_BILLBOARD_H
#define LIGHT_BILLBOARD_H


#include "Actor.h"

namespace renderer{
	struct Light;
}

class LightBillboard : public Actor {
public:
	LightBillboard();
	//LightBillboard(renderer::TexturedModel* model);
	LightBillboard(renderer::TexturedModel* model,
			   renderer::Light* light = nullptr,
			   glm::vec3& pos = glm::vec3(0.0f),
			   glm::quat& rot = glm::quat(),
			   glm::vec3& scale = glm::vec3(1.0f)
	);
	//LightBillboard(const LightBillboard& other);
	virtual ~LightBillboard();

	virtual void setPosition(glm::vec3& pos) override;
	virtual void update(float deltatime) override;

	void setLight(renderer::Light* light){ m_light = light; }

	renderer::Light* getLight(){ return m_light; }

private:
	renderer::Light* m_light;

};

#endif // !LIGHT_BILLBOARD_H
