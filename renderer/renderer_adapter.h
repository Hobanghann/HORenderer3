#pragma once

#include <memory>

#include "renderer.h"
#include "samples/blinn_phong_sample/renderer/blinn_phong_renderer.h"
#include "samples/pbr_shadow_sample/renderer/pbr_shadow_renderer.h"

namespace ho {
    class RendererAdapter {
       public:
        RendererAdapter() : renderer_(new BlinnPhongRenderer()) {
            // TODO: set your own renderer on 'renderer_' member.
            // renderer_ = new OwnRenderer();
        }

        bool Initialize(uint8_t* color_buffer, int width, int height) {
            if (!renderer_->CreateDefaultFramebuffer(color_buffer, width, height)) {
                return false;
            }
            return renderer_->Initialize();
        }

        bool PreUpdate(float delta_time) { return renderer_->PreUpdate(delta_time); }

        bool Render() { return renderer_->Render(); }

        bool PostUpdate(float delta_time) { return renderer_->PostUpdate(delta_time); }

        bool Quit() { return renderer_->Quit(); }

        void PressKey(InputKey key) { return renderer_->PressKey(key); }
        void ReleaseKey(InputKey key) { return renderer_->ReleaseKey(key); }
        void UpdateMousePos(float new_x, float new_y) { return renderer_->UpdateMousePos(new_x, new_y); }
        void UpdateMouseWheel(float delta) { return renderer_->UpdateMouseWheel(delta); }

       private:
        std::unique_ptr<Renderer> renderer_;
    };

}  // namespace ho