#include "renderer.h"

namespace ho {
    bool Renderer::CreateDefaultFramebuffer(uint8_t* color_buffer, int width, int height) {
        if (color_buffer == nullptr || width <= 0 || height <= 0) {
            return false;
        }
        width_ = width;
        height_ = height;
        return VirtualGPU::GetInstance().Initialize(color_buffer, width, height, VG_BGRA, VG_UNSIGNED_BYTE);
    }

    void Renderer::PressKey(InputKey key) { input_states[key] = true; }
    void Renderer::ReleaseKey(InputKey key) { input_states[key] = false; }
    void Renderer::UpdateMousePos(float new_x, float new_y) {
        mouse_delta_x_ = new_x - mouse_x_;
        mouse_delta_y_ = new_y - mouse_y_;
        mouse_x_ = new_x;
        mouse_y_ = new_y;
    }
    void Renderer::UpdateMouseWheel(float delta) { mouse_wheel_delta_ = delta; }
}  // namespace ho