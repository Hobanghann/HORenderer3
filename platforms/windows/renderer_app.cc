
#include "renderer_app.h"

#include <cassert>

#include "renderer/input_key.h"

static bool TranslateWin32Key(WPARAM vk, ho::InputKey& out);

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    return RendererApp::GetApp().MsgHandler(hWnd, iMessage, wParam, lParam);
}

void RendererApp::CreateApp(HINSTANCE hInstance) {
    assert(app_ == nullptr);
    app_ = new RendererApp(hInstance);
}

RendererApp& RendererApp::GetApp() {
    assert(app_ != nullptr);
    return *app_;
}

RendererApp::RendererApp(HINSTANCE hInstance)
    : WindowsApp(hInstance),
      is_paused_(false),
      is_minimized_(false),
      is_maximized_(false),
      is_resizing_(false),
      main_window_(nullptr) {}

void RendererApp::Initialize() {
    // Create Window
    windows_.emplace_back(std::make_unique<Window>(hInstance_, L"HORenderer3", 1280, 720, WndProc));
    main_window_ = windows_.begin()->get();
    uint32_t* back_buffer = main_window_->CreateCPUBackBuffer(1280, 720);

    // Initialize renderer
    if (!renderer_adapter_.Initialize((uint8_t*)back_buffer, 1280, 720)) {
        main_window_->ShowMessageBox(L"Error", L"Renderer Initialize() failed");
        return;
    }
}

void RendererApp::Run() {
    MSG msg = {0};
    POINT pt;
    timer_.Reset();

    while (msg.message != WM_QUIT) {
        HandleMessages(msg);

        timer_.Tick();

        if (is_paused_) {
            Sleep(100);
        } else {
            // =============================================================
            // Calculate Frame Stats
            // =============================================================
            static int frame_count;
            static float elapsed_time;

            frame_count++;

            if ((timer_.TotalTime() - elapsed_time) >= 1.f) {
                float fps = (float)frame_count;
                float ms_per_frame = 1000.f / fps;

                wchar_t buffer[256];
                swprintf_s(buffer, L"FPS: %.1f | per frame: %.3fms", fps, ms_per_frame);

                main_window_->PrintTextInTitle(buffer);

                frame_count = 0;
                elapsed_time += 1.f;
            }
            // =============================================================
            // Update Mouse Position
            // =============================================================
            if (GetCursorPos(&pt)) {
                ScreenToClient(main_window_->handle(), &pt);
                renderer_adapter_.UpdateMousePos(static_cast<float>(pt.x), static_cast<float>(pt.y));
            }
            // =============================================================
            // PreUpdate
            // =============================================================
            if (!renderer_adapter_.PreUpdate(timer_.DeltaTime())) {
                main_window_->ShowMessageBox(L"Error", L"Renderer PreUpdate() failed");
                return;
            }
            // =============================================================
            // Draw
            // =============================================================
            if (!renderer_adapter_.Render()) {
                main_window_->ShowMessageBox(L"Error", L"Renderer Render() failed");
                return;
            }

            // =============================================================
            // Buffer swap
            // =============================================================
            main_window_->SwapCPUBuffer(0);

            // =============================================================
            // PostUpdate
            // =============================================================
            if (!renderer_adapter_.PostUpdate(timer_.DeltaTime())) {
                main_window_->ShowMessageBox(L"Error", L"Renderer PostUpdate() failed");
                return;
            }
        }
    }
}

void RendererApp::Quit() { renderer_adapter_.Quit(); }

void RendererApp::OnResize() {}

void RendererApp::OnKEYDOWN(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    ho::InputKey key;
    if (TranslateWin32Key(wParam, key)) {
        renderer_adapter_.PressKey(key);
    }
}

void RendererApp::OnKEYUP(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    ho::InputKey key;
    if (TranslateWin32Key(wParam, key)) {
        renderer_adapter_.ReleaseKey(key);
    }
}

void RendererApp::OnMOUSEDOWN(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    if (wParam & MK_LBUTTON) renderer_adapter_.PressKey(ho::InputKey::INPUT_KEY_MOUSE_LEFT);
    if (wParam & MK_RBUTTON) renderer_adapter_.PressKey(ho::InputKey::INPUT_KEY_MOUSE_RIGHT);
    if (wParam & MK_MBUTTON) renderer_adapter_.PressKey(ho::InputKey::INPUT_KEY_MOUSE_MIDDLE);
}
void RendererApp::OnMOUSEUP(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    if (!(wParam & MK_LBUTTON)) renderer_adapter_.ReleaseKey(ho::InputKey::INPUT_KEY_MOUSE_LEFT);
    if (!(wParam & MK_RBUTTON)) renderer_adapter_.ReleaseKey(ho::InputKey::INPUT_KEY_MOUSE_RIGHT);
    if (!(wParam & MK_MBUTTON)) renderer_adapter_.ReleaseKey(ho::InputKey::INPUT_KEY_MOUSE_MIDDLE);
}
void RendererApp::OnMOUSEMOVE(WPARAM wParam, LPARAM lParam) { wParam, lParam; }
void RendererApp::OnMOUSEWHEEL(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    short delta = GET_WHEEL_DELTA_WPARAM(wParam);

    renderer_adapter_.UpdateMouseWheel(static_cast<float>(delta) / WHEEL_DELTA);
}
void RendererApp::OnSETFOCUS(WPARAM wParam, LPARAM lParam) { wParam, lParam; }
void RendererApp::OnKILLFOCUS(WPARAM wParam, LPARAM lParam) { wParam, lParam; }

void RendererApp::OnSize(WPARAM wParam, LPARAM lParam) {
    if (!main_window_) {
        return;
    }

    main_window_->ResizeClient(LOWORD(lParam), HIWORD(lParam));

    if (wParam == SIZE_MINIMIZED) {
        is_paused_ = true;
        is_minimized_ = true;
        is_maximized_ = false;
    } else if (wParam == SIZE_MAXIMIZED) {
        is_paused_ = false;
        is_minimized_ = false;
        is_maximized_ = true;
        OnResize();
    } else if (wParam == SIZE_RESTORED) {
        if (is_minimized_) {
            is_paused_ = false;
            is_minimized_ = false;
            OnResize();
        } else if (is_maximized_) {
            is_paused_ = false;
            is_maximized_ = false;
            OnResize();
        } else if (is_resizing_) {
            // Do nothing
        } else {
            OnResize();
        }
    }
}

void RendererApp::OnENTERSIZEMOVE(WPARAM wParam, LPARAM lParam) {
    wParam, lParam;
    is_paused_ = true;
    is_resizing_ = true;
    timer_.Stop();
}

void RendererApp::OnEXITSIZEMOVE(WPARAM wParam, LPARAM lParam) {
    wParam, lParam;
    is_paused_ = false;
    is_resizing_ = false;
    timer_.Start();
}

void RendererApp::OnACTIVE(WPARAM wParam, LPARAM lParam) {
    wParam, lParam;
    is_paused_ = false;
    timer_.Start();
}

void RendererApp::OnINACTIVE(WPARAM wParam, LPARAM lParam) {
    wParam, lParam;
    is_paused_ = true;
    timer_.Stop();
}

RendererApp* RendererApp::app_ = nullptr;

static bool TranslateWin32Key(WPARAM vk, ho::InputKey& out) {
    switch (vk) {
        // Control
        case VK_BACK:
            out = ho::InputKey::INPUT_KEY_BACKSPACE;
            return true;
        case VK_TAB:
            out = ho::InputKey::INPUT_KEY_TAB;
            return true;
        case VK_CLEAR:
            out = ho::InputKey::INPUT_KEY_CLEAR;
            return true;
        case VK_RETURN:
            out = ho::InputKey::INPUT_KEY_ENTER;
            return true;
        case VK_SHIFT:
            out = ho::InputKey::INPUT_KEY_SHIFT;
            return true;
        case VK_CONTROL:
            out = ho::InputKey::INPUT_KEY_CONTROL;
            return true;
        case VK_MENU:
            out = ho::InputKey::INPUT_KEY_ALT;
            return true;
        case VK_PAUSE:
            out = ho::InputKey::INPUT_KEY_PAUSE;
            return true;
        case VK_CAPITAL:
            out = ho::InputKey::INPUT_KEY_CAPS_LOCK;
            return true;
        case VK_ESCAPE:
            out = ho::InputKey::INPUT_KEY_ESCAPE;
            return true;
        case VK_SPACE:
            out = ho::InputKey::INPUT_KEY_SPACE;
            return true;

        case VK_PRIOR:
            out = ho::InputKey::INPUT_KEY_PAGE_UP;
            return true;
        case VK_NEXT:
            out = ho::InputKey::INPUT_KEY_PAGE_DOWN;
            return true;
        case VK_END:
            out = ho::InputKey::INPUT_KEY_END;
            return true;
        case VK_HOME:
            out = ho::InputKey::INPUT_KEY_HOME;
            return true;

        case VK_LEFT:
            out = ho::InputKey::INPUT_KEY_LEFT;
            return true;
        case VK_UP:
            out = ho::InputKey::INPUT_KEY_UP;
            return true;
        case VK_RIGHT:
            out = ho::InputKey::INPUT_KEY_RIGHT;
            return true;
        case VK_DOWN:
            out = ho::InputKey::INPUT_KEY_DOWN;
            return true;

        case VK_SELECT:
            out = ho::InputKey::INPUT_KEY_SELECT;
            return true;
        case VK_PRINT:
            out = ho::InputKey::INPUT_KEY_PRINT;
            return true;
        case VK_EXECUTE:
            out = ho::InputKey::INPUT_KEY_EXECUTE;
            return true;
        case VK_SNAPSHOT:
            out = ho::InputKey::INPUT_KEY_PRINT_SCREEN;
            return true;
        case VK_INSERT:
            out = ho::InputKey::INPUT_KEY_INSERT;
            return true;
        case VK_DELETE:
            out = ho::InputKey::INPUT_KEY_DELETE;
            return true;
        case VK_HELP:
            out = ho::InputKey::INPUT_KEY_HELP;
            return true;

            // Numbers
        case VK_NUM_0:
            out = ho::InputKey::INPUT_KEY_0;
            return true;
        case VK_NUM_1:
            out = ho::InputKey::INPUT_KEY_1;
            return true;
        case VK_NUM_2:
            out = ho::InputKey::INPUT_KEY_2;
            return true;
        case VK_NUM_3:
            out = ho::InputKey::INPUT_KEY_3;
            return true;
        case VK_NUM_4:
            out = ho::InputKey::INPUT_KEY_4;
            return true;
        case VK_NUM_5:
            out = ho::InputKey::INPUT_KEY_5;
            return true;
        case VK_NUM_6:
            out = ho::InputKey::INPUT_KEY_6;
            return true;
        case VK_NUM_7:
            out = ho::InputKey::INPUT_KEY_7;
            return true;
        case VK_NUM_8:
            out = ho::InputKey::INPUT_KEY_8;
            return true;
        case VK_NUM_9:
            out = ho::InputKey::INPUT_KEY_9;
            return true;

        // Alphabet
        case VK_A:
            out = ho::InputKey::INPUT_KEY_A;
            return true;
        case VK_B:
            out = ho::InputKey::INPUT_KEY_B;
            return true;
        case VK_C:
            out = ho::InputKey::INPUT_KEY_C;
            return true;
        case VK_D:
            out = ho::InputKey::INPUT_KEY_D;
            return true;
        case VK_E:
            out = ho::InputKey::INPUT_KEY_E;
            return true;
        case VK_F:
            out = ho::InputKey::INPUT_KEY_F;
            return true;
        case VK_G:
            out = ho::InputKey::INPUT_KEY_G;
            return true;
        case VK_H:
            out = ho::InputKey::INPUT_KEY_H;
            return true;
        case VK_I:
            out = ho::InputKey::INPUT_KEY_I;
            return true;
        case VK_J:
            out = ho::InputKey::INPUT_KEY_J;
            return true;
        case VK_K:
            out = ho::InputKey::INPUT_KEY_K;
            return true;
        case VK_L:
            out = ho::InputKey::INPUT_KEY_L;
            return true;
        case VK_M:
            out = ho::InputKey::INPUT_KEY_M;
            return true;
        case VK_N:
            out = ho::InputKey::INPUT_KEY_N;
            return true;
        case VK_O:
            out = ho::InputKey::INPUT_KEY_O;
            return true;
        case VK_P:
            out = ho::InputKey::INPUT_KEY_P;
            return true;
        case VK_Q:
            out = ho::InputKey::INPUT_KEY_Q;
            return true;
        case VK_R:
            out = ho::InputKey::INPUT_KEY_R;
            return true;
        case VK_S:
            out = ho::InputKey::INPUT_KEY_S;
            return true;
        case VK_T:
            out = ho::InputKey::INPUT_KEY_T;
            return true;
        case VK_U:
            out = ho::InputKey::INPUT_KEY_U;
            return true;
        case VK_V:
            out = ho::InputKey::INPUT_KEY_V;
            return true;
        case VK_W:
            out = ho::InputKey::INPUT_KEY_W;
            return true;
        case VK_X:
            out = ho::InputKey::INPUT_KEY_X;
            return true;
        case VK_Y:
            out = ho::InputKey::INPUT_KEY_Y;
            return true;
        case VK_Z:
            out = ho::InputKey::INPUT_KEY_Z;
            return true;

        // Function
        case VK_F1:
            out = ho::InputKey::INPUT_KEY_F1;
            return true;
        case VK_F2:
            out = ho::InputKey::INPUT_KEY_F2;
            return true;
        case VK_F3:
            out = ho::InputKey::INPUT_KEY_F3;
            return true;
        case VK_F4:
            out = ho::InputKey::INPUT_KEY_F4;
            return true;
        case VK_F5:
            out = ho::InputKey::INPUT_KEY_F5;
            return true;
        case VK_F6:
            out = ho::InputKey::INPUT_KEY_F6;
            return true;
        case VK_F7:
            out = ho::InputKey::INPUT_KEY_F7;
            return true;
        case VK_F8:
            out = ho::InputKey::INPUT_KEY_F8;
            return true;
        case VK_F9:
            out = ho::InputKey::INPUT_KEY_F9;
            return true;
        case VK_F10:
            out = ho::InputKey::INPUT_KEY_F10;
            return true;
        case VK_F11:
            out = ho::InputKey::INPUT_KEY_F11;
            return true;
        case VK_F12:
            out = ho::InputKey::INPUT_KEY_F12;
            return true;
        case VK_F13:
            out = ho::InputKey::INPUT_KEY_F13;
            return true;
        case VK_F14:
            out = ho::InputKey::INPUT_KEY_F14;
            return true;
        case VK_F15:
            out = ho::InputKey::INPUT_KEY_F15;
            return true;
        case VK_F16:
            out = ho::InputKey::INPUT_KEY_F16;
            return true;
        case VK_F17:
            out = ho::InputKey::INPUT_KEY_F17;
            return true;
        case VK_F18:
            out = ho::InputKey::INPUT_KEY_F18;
            return true;
        case VK_F19:
            out = ho::InputKey::INPUT_KEY_F19;
            return true;
        case VK_F20:
            out = ho::InputKey::INPUT_KEY_F20;
            return true;
        case VK_F21:
            out = ho::InputKey::INPUT_KEY_F21;
            return true;
        case VK_F22:
            out = ho::InputKey::INPUT_KEY_F22;
            return true;
        case VK_F23:
            out = ho::InputKey::INPUT_KEY_F23;
            return true;
        case VK_F24:
            out = ho::InputKey::INPUT_KEY_F24;
            return true;

        // Numpad
        case VK_NUMLOCK:
            out = ho::InputKey::INPUT_KEY_NUM_LOCK;
            return true;
        case VK_SCROLL:
            out = ho::InputKey::INPUT_KEY_SCROLL_LOCK;
            return true;
        case VK_NUMPAD0:
            out = ho::InputKey::INPUT_KEY_NUMPAD_0;
            return true;
        case VK_NUMPAD1:
            out = ho::InputKey::INPUT_KEY_NUMPAD_1;
            return true;
        case VK_NUMPAD2:
            out = ho::InputKey::INPUT_KEY_NUMPAD_2;
            return true;
        case VK_NUMPAD3:
            out = ho::InputKey::INPUT_KEY_NUMPAD_3;
            return true;
        case VK_NUMPAD4:
            out = ho::InputKey::INPUT_KEY_NUMPAD_4;
            return true;
        case VK_NUMPAD5:
            out = ho::InputKey::INPUT_KEY_NUMPAD_5;
            return true;
        case VK_NUMPAD6:
            out = ho::InputKey::INPUT_KEY_NUMPAD_6;
            return true;
        case VK_NUMPAD7:
            out = ho::InputKey::INPUT_KEY_NUMPAD_7;
            return true;
        case VK_NUMPAD8:
            out = ho::InputKey::INPUT_KEY_NUMPAD_8;
            return true;
        case VK_NUMPAD9:
            out = ho::InputKey::INPUT_KEY_NUMPAD_9;
            return true;
        case VK_MULTIPLY:
            out = ho::InputKey::INPUT_KEY_NUMPAD_MULTIPLY;
            return true;
        case VK_ADD:
            out = ho::InputKey::INPUT_KEY_NUMPAD_ADD;
            return true;
        case VK_SEPARATOR:
            out = ho::InputKey::INPUT_KEY_NUMPAD_SEPARATOR;
            return true;
        case VK_SUBTRACT:
            out = ho::InputKey::INPUT_KEY_NUMPAD_SUBTRACT;
            return true;
        case VK_DECIMAL:
            out = ho::InputKey::INPUT_KEY_NUMPAD_DECIMAL;
            return true;
        case VK_DIVIDE:
            out = ho::InputKey::INPUT_KEY_NUMPAD_DIVIDE;
            return true;

        // System
        case VK_LSHIFT:
            out = ho::InputKey::INPUT_KEY_LEFT_SHIFT;
            return true;
        case VK_RSHIFT:
            out = ho::InputKey::INPUT_KEY_RIGHT_SHIFT;
            return true;
        case VK_LCONTROL:
            out = ho::InputKey::INPUT_KEY_LEFT_CONTROL;
            return true;
        case VK_RCONTROL:
            out = ho::InputKey::INPUT_KEY_RIGHT_CONTROL;
            return true;
        case VK_LMENU:
            out = ho::InputKey::INPUT_KEY_LEFT_ALT;
            return true;
        case VK_RMENU:
            out = ho::InputKey::INPUT_KEY_RIGHT_ALT;
            return true;
        case VK_LWIN:
            out = ho::InputKey::INPUT_KEY_LEFT_SUPER;
            return true;
        case VK_RWIN:
            out = ho::InputKey::INPUT_KEY_RIGHT_SUPER;
            return true;
        case VK_APPS:
            out = ho::InputKey::INPUT_KEY_MENU;
            return true;

        // Browser / Media
        case VK_BROWSER_BACK:
            out = ho::InputKey::INPUT_KEY_BROWSER_BACK;
            return true;
        case VK_BROWSER_FORWARD:
            out = ho::InputKey::INPUT_KEY_BROWSER_FORWARD;
            return true;
        case VK_BROWSER_REFRESH:
            out = ho::InputKey::INPUT_KEY_BROWSER_REFRESH;
            return true;
        case VK_BROWSER_STOP:
            out = ho::InputKey::INPUT_KEY_BROWSER_STOP;
            return true;
        case VK_BROWSER_SEARCH:
            out = ho::InputKey::INPUT_KEY_BROWSER_SEARCH;
            return true;
        case VK_BROWSER_FAVORITES:
            out = ho::InputKey::INPUT_KEY_BROWSER_FAVORITES;
            return true;
        case VK_BROWSER_HOME:
            out = ho::InputKey::INPUT_KEY_BROWSER_HOME;
            return true;

        case VK_VOLUME_MUTE:
            out = ho::InputKey::INPUT_KEY_VOLUME_MUTE;
            return true;
        case VK_VOLUME_DOWN:
            out = ho::InputKey::INPUT_KEY_VOLUME_DOWN;
            return true;
        case VK_VOLUME_UP:
            out = ho::InputKey::INPUT_KEY_VOLUME_UP;
            return true;
        case VK_MEDIA_NEXT_TRACK:
            out = ho::InputKey::INPUT_KEY_MEDIA_NEXT_TRACK;
            return true;
        case VK_MEDIA_PREV_TRACK:
            out = ho::InputKey::INPUT_KEY_MEDIA_PREVIOUS_TRACK;
            return true;
        case VK_MEDIA_STOP:
            out = ho::InputKey::INPUT_KEY_MEDIA_STOP;
            return true;
        case VK_MEDIA_PLAY_PAUSE:
            out = ho::InputKey::INPUT_KEY_MEDIA_PLAY_PAUSE;
            return true;

        case VK_LAUNCH_MAIL:
            out = ho::InputKey::INPUT_KEY_LAUNCH_MAIL;
            return true;
        case VK_LAUNCH_MEDIA_SELECT:
            out = ho::InputKey::INPUT_KEY_LAUNCH_MEDIA_SELECT;
            return true;
        case VK_LAUNCH_APP1:
            out = ho::InputKey::INPUT_KEY_LAUNCH_APP_1;
            return true;
        case VK_LAUNCH_APP2:
            out = ho::InputKey::INPUT_KEY_LAUNCH_APP_2;
            return true;

        // OEM
        case VK_OEM_1:
            out = ho::InputKey::INPUT_KEY_OEM_1;
            return true;
        case VK_OEM_2:
            out = ho::InputKey::INPUT_KEY_OEM_2;
            return true;
        case VK_OEM_3:
            out = ho::InputKey::INPUT_KEY_OEM_3;
            return true;
        case VK_OEM_4:
            out = ho::InputKey::INPUT_KEY_OEM_4;
            return true;
        case VK_OEM_5:
            out = ho::InputKey::INPUT_KEY_OEM_5;
            return true;
        case VK_OEM_6:
            out = ho::InputKey::INPUT_KEY_OEM_6;
            return true;
        case VK_OEM_7:
            out = ho::InputKey::INPUT_KEY_OEM_7;
            return true;
        case VK_OEM_8:
            out = ho::InputKey::INPUT_KEY_OEM_8;
            return true;

        default:
            return false;
    }
}
