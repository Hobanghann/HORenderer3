#include "renderer_app.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
                   _In_ int nShowCmd) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    RendererApp::CreateApp(hInstance);

    RendererApp::GetApp().Initialize();
    RendererApp::GetApp().Run();
    RendererApp::GetApp().Quit();
}