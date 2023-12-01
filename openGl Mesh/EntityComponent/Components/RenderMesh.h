#pragma once
#include "../Component.h"

class Model;
class Shader;
namespace Components {
    class Transform;
    class RenderMesh : public Component
    {
    private:
        Model* _model;
        static Shader* _defaultShader;
        Shader* _overrideShader;
        Transform* _transform;

        // reutns a ptr to the shader to use
        Shader* getShader() const;
    public:
        RenderMesh();

        void render() override;

        void setShader(Shader* shader);

        void setTransform(Transform* transform);
        void setModel(Model* model);
    };
}

