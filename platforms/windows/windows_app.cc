#include "windows_app.h"

#include <commctrl.h>
#include <windowsx.h>

#include <cassert>
#include <cstdint>

Window::Window(HINSTANCE hOwner, const std::wstring& name, uint32_t width, uint32_t height, WNDPROC msg_handler)
    : name_(name), width_(width), height_(height), client_width_(0), client_height_(0), back_buffer_count_(0) {
    WNDCLASSEXW stWndClass{};
    stWndClass.cbSize = sizeof(WNDCLASSEXW);
    stWndClass.style = CS_OWNDC;
    stWndClass.lpfnWndProc = msg_handler;
    stWndClass.hInstance = hOwner;
    stWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    stWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    stWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    stWndClass.lpszClassName = name_.c_str();
    stWndClass.hIconSm = (HICON)LoadImage(hOwner, MAKEINTRESOURCE(5), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
                                          GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    if (!RegisterClassExW(&stWndClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"RegisterClassExW failed.", L"Error", MB_ICONERROR);
        return;
    }

    RECT stClientSize = {0, 0, width_, height_};
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&stClientSize, dwStyle, FALSE);
    handle_ = CreateWindowW(name_.c_str(), name_.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                            stClientSize.right - stClientSize.left, stClientSize.bottom - stClientSize.top, nullptr,
                            nullptr, hOwner, nullptr);

    if (!handle_) {
        MessageBoxW(nullptr, L"CreateWindowW failed.", L"Error", MB_ICONERROR);
        return;
    }

    front_dc_ = GetDC(handle_);

    if (!front_dc_) {
        MessageBoxW(handle_, L"GetDC failed.", L"Error", MB_ICONERROR);
        return;
    }

    // set text's background color as transparent.
    SetBkMode(front_dc_, TRANSPARENT);

    RECT stClientRect{};
    GetClientRect(handle_, &stClientRect);
    client_width_ = stClientRect.right - stClientRect.left;
    client_height_ = stClientRect.bottom - stClientRect.top;

    default_bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    default_bmi_.bmiHeader.biWidth = client_width_;
    default_bmi_.bmiHeader.biHeight = -client_height_;
    default_bmi_.bmiHeader.biPlanes = 1;
    default_bmi_.bmiHeader.biBitCount = 32;
    default_bmi_.bmiHeader.biCompression = BI_RGB;

    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

Window::~Window() {
    for (uint32_t i = 0; i < back_buffer_count_; i++) {
        SelectObject(back_buffers_[i].dc, back_buffers_[i].default_bitmap);
        DeleteObject(back_buffers_[i].bitmap);
        DeleteDC(back_buffers_[i].dc);
    }
    ReleaseDC(handle_, front_dc_);
}

HWND Window::handle() const { return handle_; }
const std::wstring& Window::name() const { return name_; }
uint32_t Window::width() const { return width_; }
uint32_t Window::height() const { return height_; }
uint32_t Window::client_width() const { return client_width_; }
uint32_t Window::client_height() const { return client_height_; }

void Window::Resize(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;

    RECT rc;
    GetClientRect(handle_, &rc);
    client_width_ = rc.right - rc.left;
    client_height_ = rc.bottom - rc.top;
}
void Window::ResizeClient(uint32_t client_width, uint32_t client_height) {
    client_width_ = client_width;
    client_height_ = client_height;

    RECT wr;
    GetWindowRect(handle_, &wr);
    width_ = wr.right - wr.left;
    height_ = wr.bottom - wr.top;
}

void Window::Show(int nCmdShow) { ShowWindow(handle_, nCmdShow); }

void Window::ShowMessageBox(const std::wstring& title, const std::wstring& text, UINT type) {
    MessageBoxW(handle_, text.c_str(), text.c_str(), type);
}

uint32_t* Window::CreateCPUBackBuffer(uint32_t buffer_width, uint32_t buffer_height) {
    if (back_buffer_count_ >= MAX_BACK_BUFFER_COUNT) {
        return nullptr;
    }

    // set back buffer properties
    back_buffers_[back_buffer_count_].info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    back_buffers_[back_buffer_count_].info.bmiHeader.biWidth = buffer_width;
    back_buffers_[back_buffer_count_].info.bmiHeader.biHeight = -buffer_height;
    back_buffers_[back_buffer_count_].info.bmiHeader.biPlanes = 1;
    back_buffers_[back_buffer_count_].info.bmiHeader.biBitCount = 32;
    back_buffers_[back_buffer_count_].info.bmiHeader.biCompression = BI_RGB;

    // create dc for back buffer
    back_buffers_[back_buffer_count_].dc = CreateCompatibleDC(front_dc_);

    if (!back_buffers_[back_buffer_count_].dc) {
        MessageBoxW(nullptr, L"CreateCompatibleDC failed.", L"Error", MB_ICONERROR);
        return nullptr;
    }

    uint32_t* color_buffer = nullptr;
    // create bitmap for back buffer
    back_buffers_[back_buffer_count_].bitmap =
        CreateDIBSection(back_buffers_[back_buffer_count_].dc, &back_buffers_[back_buffer_count_].info, DIB_RGB_COLORS,
                         (void**)&color_buffer, NULL, 0);

    if (!back_buffers_[back_buffer_count_].bitmap || !color_buffer) {
        MessageBoxW(nullptr, L"CreateDIBSection failed.", L"Error", MB_ICONERROR);
        return nullptr;
    }

    // binding back buffer dc to bitmap
    back_buffers_[back_buffer_count_].default_bitmap =
        (HBITMAP)SelectObject(back_buffers_[back_buffer_count_].dc, back_buffers_[back_buffer_count_].bitmap);

    back_buffer_count_++;

    // return raw pointer of back buffer
    return color_buffer;
}

void Window::CopyCPUBuffer(const void* external_memory, uint32_t external_width, uint32_t external_height) {
    if (external_memory == nullptr) {
        return;
    }

    BITMAPINFO bmi = default_bmi_;
    bmi.bmiHeader.biWidth = external_width;
    bmi.bmiHeader.biHeight = -static_cast<LONG>(external_height);

    if (!StretchDIBits(front_dc_, 0, 0, client_width_, client_height_, 0, 0, external_width, external_height,
                       external_memory, &bmi, DIB_RGB_COLORS, SRCCOPY)) {
        MessageBoxW(nullptr, L"StretchDIBits failed.", L"Error", MB_ICONERROR);
    }
}

void Window::SwapCPUBuffer(uint32_t index) {
    if (index >= back_buffer_count_) {
        MessageBoxW(nullptr, L"SwapCPUBuffer failed.", L"Error", MB_ICONERROR);
        return;
    }
    if (!back_buffers_[index].dc) {
        MessageBoxW(nullptr, L"SwapCPUBuffer failed.", L"Error", MB_ICONERROR);
        return;
    }
    if (!BitBlt(front_dc_, 0, 0, client_width_, client_height_, back_buffers_[index].dc, 0, 0, SRCCOPY)) {
        MessageBoxW(nullptr, L"BitBlit failed.", L"Error", MB_ICONERROR);
    }
}

void Window::PrintText(const std::wstring& text, uint32_t x, uint32_t y) {
    if (!TextOutW(front_dc_, x, y, text.c_str(), text.size())) {
        MessageBoxW(nullptr, L"TextOutW failed.", L"Error", MB_ICONERROR);
    }
}

void Window::PrintTextInTitle(const std::wstring& text) {
    std::wstring title = name_ + L" | " + text;
    SetWindowTextW(handle_, title.c_str());
}

WindowsApp::WindowsApp(HINSTANCE hInstance) : hInstance_(hInstance) {}

void WindowsApp::CreateDebugConsole() {
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    printf("Debug Console Initialized!\n");
}

void WindowsApp::HandleMessages(MSG& Message) {
    while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
}

LRESULT WindowsApp::MsgHandler(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
        case WM_KEYDOWN:
            OnKEYDOWN(wParam, lParam);
            return 0;

        case WM_KEYUP:
            OnKEYUP(wParam, lParam);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            OnMOUSEDOWN(wParam, lParam);
            return 0;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            OnMOUSEUP(wParam, lParam);
            return 0;

        case WM_MOUSEMOVE:
            OnMOUSEMOVE(wParam, lParam);
            return 0;

        case WM_MOUSEWHEEL:
            OnMOUSEWHEEL(wParam, lParam);
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                OnINACTIVE(wParam, lParam);
            } else {
                OnACTIVE(wParam, lParam);
            }
            return 0;

        case WM_SETFOCUS:
            OnSETFOCUS(wParam, lParam);
            return 0;
        case WM_KILLFOCUS:
            OnKILLFOCUS(wParam, lParam);
            return 0;

        case WM_SIZE:
            OnSize(wParam, lParam);
            return 0;
        case WM_ENTERSIZEMOVE:
            OnENTERSIZEMOVE(wParam, lParam);
            return 0;
        case WM_EXITSIZEMOVE:
            OnEXITSIZEMOVE(wParam, lParam);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
