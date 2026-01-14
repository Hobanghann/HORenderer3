#pragma once

#include "input_key.h"
#include "resource/resource_manager.h"
#include "virtual_gpu/virtual_gpu.h"

namespace ho {
    class Renderer {
        friend class RendererAdapter;

       public:
        bool CreateDefaultFramebuffer(uint8_t* color_buffer, int width, int height);

        virtual bool Initialize() = 0;

        virtual bool PreUpdate(float delta_time) = 0;

        virtual bool Render() = 0;

        virtual bool PostUpdate(float delta_time) = 0;

        virtual bool Quit() = 0;

       protected:
        void PressKey(InputKey key);
        void ReleaseKey(InputKey key);
        void UpdateMousePos(float new_x, float new_y);
        void UpdateMouseWheel(float delta);
        ResourceManager resource_manager_;
        int width_;
        int height_;
        bool input_states[INPUT_KEY_COUNT] = {false};
        float mouse_x_;
        float mouse_y_;
        float mouse_delta_x_;
        float mouse_delta_y_;
        float mouse_wheel_delta_;
    };
}  // namespace ho