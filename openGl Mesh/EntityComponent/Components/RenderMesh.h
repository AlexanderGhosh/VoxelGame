#pragma once
#include "../Component.h"

class Model;
class Shader;
namespace Components {
    class RenderMesh : public Component
    {
    private:
        Model* _model;
        static Shader* _defaultShader;
        Shader* _overrideShader;

        void bindShader() const;
    public:
        RenderMesh();

        void render() override;

        void setShader(Shader* shader);
    };
}

