#pragma once

#include "core/time/game_timer.h"
#include "renderer/renderer_adapter.h"
#include "windows_app.h"
#include "windows_key_fallback.h"

class RendererApp : public WindowsApp {
   public:
    static void CreateApp(HINSTANCE hInstance);
    static RendererApp& GetApp();
    ~RendererApp() = default;

    void Initialize() override;

    void Run() override;

    void Quit() override;

   private:
    RendererApp() = delete;
    RendererApp(const RendererApp&) = delete;
    RendererApp(HINSTANCE hInstance);
    RendererApp& operator=(const RendererApp&) = delete;

    void OnResize();

    void OnKEYDOWN(WPARAM wParam, LPARAM lParam) override;
    void OnKEYUP(WPARAM wParam, LPARAM lParam) override;
    void OnMOUSEDOWN(WPARAM wParam, LPARAM lParam) override;
    void OnMOUSEUP(WPARAM wParam, LPARAM lParam) override;
    void OnMOUSEMOVE(WPARAM wParam, LPARAM lParam) override;
    void OnMOUSEWHEEL(WPARAM wParam, LPARAM lParam) override;
    void OnSETFOCUS(WPARAM wParam, LPARAM lParam) override;
    void OnKILLFOCUS(WPARAM wParam, LPARAM lParam) override;
    void OnSize(WPARAM wParam, LPARAM lParam) override;
    void OnENTERSIZEMOVE(WPARAM wParam, LPARAM lParam) override;
    void OnEXITSIZEMOVE(WPARAM wParam, LPARAM lParam) override;
    void OnACTIVE(WPARAM wParam, LPARAM lParam) override;
    void OnINACTIVE(WPARAM wParam, LPARAM lParam) override;

    ho::GameTimer timer_;
    ho::RendererAdapter renderer_adapter_;

    static RendererApp* app_;
    bool is_paused_;
    bool is_minimized_;
    bool is_maximized_;
    bool is_resizing_;

    Window* main_window_;
};