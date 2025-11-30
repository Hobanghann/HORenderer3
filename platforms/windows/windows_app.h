#pragma once

#define UNICODE
#define _UNICODE

#define NOMINMAX
#include <windows.h>

#include <array>
#include <list>
#include <memory>
#include <string>

#include "windows_key.h"

class Window {
   public:
    Window(HINSTANCE hOwner, const std::wstring& name, uint32_t width, uint32_t height, WNDPROC msg_handler);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    ~Window();

    HWND handle() const;
    const std::wstring& name() const;
    uint32_t width() const;
    uint32_t height() const;
    uint32_t client_width() const;
    uint32_t client_height() const;

    void Resize(uint32_t width, uint32_t height);
    void ResizeClient(uint32_t client_width, uint32_t client_height);

    void Show(int nCmdShow = SW_SHOW);

    void ShowMessageBox(const std::wstring& title, const std::wstring& text, UINT type = MB_ICONERROR);

    uint32_t* CreateCPUBackBuffer(uint32_t buffer_width, uint32_t buffer_height);

    void CopyCPUBuffer(const void* external_memory, uint32_t external_width, uint32_t external_height);
    void SwapCPUBuffer(uint32_t index);

    void PrintText(const std::wstring& text, uint32_t x, uint32_t y);

    void PrintTextInTitle(const std::wstring& text);

   private:
    static const uint32_t MAX_BACK_BUFFER_COUNT = 2;

    HWND handle_;
    std::wstring name_;
    uint32_t width_;
    uint32_t height_;
    uint32_t client_width_;
    uint32_t client_height_;
    HDC front_dc_;
    struct BackBuffer {
        HDC dc;
        HBITMAP bitmap;
        HBITMAP default_bitmap;
        BITMAPINFO info;
    };
    BackBuffer back_buffers_[MAX_BACK_BUFFER_COUNT] = {};
    uint32_t back_buffer_count_;

    BITMAPINFO default_bmi_;
};

class WindowsApp {
   public:
    // TODO : define following member
    //
    // static void CreateApp(...) {
    //    assert(app_ == nullptr);
    //    app_ = new ~App(...)
    // }
    // static ~App& GetApp() {
    //    assert(app_ != nullptr);
    //    return *app_;
    // }

    virtual void Initialize() = 0;

    virtual void Run() = 0;

    virtual void Quit() = 0;

    void CreateDebugConsole();

    LRESULT MsgHandler(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

   protected:
    WindowsApp() = delete;
    WindowsApp(const WindowsApp&) = delete;
    WindowsApp(HINSTANCE hInstance);
    WindowsApp& operator=(const WindowsApp&) = delete;
    virtual ~WindowsApp() = default;

    void HandleMessages(MSG& Message);

    virtual void OnKEYDOWN(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnKEYUP(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnMOUSEDOWN(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnMOUSEUP(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnMOUSEMOVE(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnMOUSEWHEEL(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnSETFOCUS(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnKILLFOCUS(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnSize(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnENTERSIZEMOVE(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnEXITSIZEMOVE(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnACTIVE(WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnINACTIVE(WPARAM wParam, LPARAM lParam) = 0;

    HINSTANCE hInstance_;
    std::list<std::unique_ptr<Window>> windows_;
    // TODO : define following member
    //
    // static ~App* app_;
};

// TODO: define following function in drived class's source file
//
// HRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam,
//                          LPARAM lParam) {
//   return GetApp().MsgHandler(hWnd, iMessage, wParam, lParam);
// }