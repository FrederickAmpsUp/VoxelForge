#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace VoxelForge::GL {

class WritableScreen;

/**
 * An object that can be drawn onto a writable screen
 */
class Drawable {
public:
    /**
     * Draw the object onto the specified writable screen
     * @param screen The screen to draw onto
     * @param origin The bottom-left corner of the affected area, in screen UV space
     * @param size The size of the affected area, in screen UV space
     */
    virtual void drawOn(WritableScreen& screen, glm::vec2 origin=glm::vec2(0.0), glm::vec2 size=glm::vec2(0.0)) = 0;
};

/**
 * A 3d object that can be rendered into 2d
 */
class Renderable {
public:
    /**
     * Draw the object and create a drawable that can be placed onto a screen
     * @param view The camera view matrix to be used
     * @param proj The projection matrix to be used
     * @retval The drawable object that this object was rendered onto
     */
    virtual std::shared_ptr<Drawable> render(glm::mat4x4 view, glm::mat4x4 proj) = 0;
    /**
     * Draw the object onto the specified writable screen
     * @param screen The screen to draw onto
     * @param view The camera view matrix to be used
     * @param proj The camera projection matrix to be used
     */
    virtual void renderTo(WritableScreen& screen, glm::mat4x4 view, glm::mat4x4 proj) = 0;
};

/**
 * An abstract representation of a camera
 */
class Camera {
public:
    /**
     * Render an object and create a drawable that can be placed onto a screen
     * @param object The renderable object to render
     * @retval The drawable object that the specified renderable was rendered onto
     */
    virtual std::shared_ptr<Drawable> render(Renderable& object) = 0;
    /**
     * Render an object into the specified writable screen
     * @param screen The screen to draw onto
     * @param object The object to render
     */
    virtual void renderTo(WritableScreen& screen, Renderable& object) = 0;
};

/**
 * A surface that can be drawn onto
 */
class WritableScreen {
public:
    /**
     * Draw an object onto this screen
     * @param drawable The object to draw
     * @param origin The bottom-left corner of the affected area, in screen UV space
     * @param size The size of the affected area, in screen UV space
     */
    virtual void blit(Drawable& drawable, glm::vec2 origin=glm::vec2(0.0), glm::vec2 size=glm::vec2(1.0)) = 0;
};
}