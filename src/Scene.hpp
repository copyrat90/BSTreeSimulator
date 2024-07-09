#pragma once

#include <optional>

namespace bs
{

enum class SceneType;

class Scene
{
public:
    virtual ~Scene() = default;

    Scene(SceneType type) : _type(type)
    {
    }

public:
    /// @return next scene type if scene change requested
    virtual auto update() -> std::optional<SceneType> = 0;

    virtual void render() const = 0;

public:
    auto get_scene_type() const -> SceneType
    {
        return _type;
    }

private:
    SceneType _type;
};

} // namespace bs
