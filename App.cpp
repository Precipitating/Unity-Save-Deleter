#include "App.h"
#include "MainFrame.h"
#include "wx/wx.h"
#include "Constants.h"

wxIMPLEMENT_APP(App);

// INITIALIZES AND RUNS THE GUI
bool App::OnInit()
{
    // library handles loose pointers
    MainFrame* mainFrame = new MainFrame(CONSTANT::APP_NAME);
    mainFrame->SetClientSize(CONSTANT::RESOLUTION.first, CONSTANT::RESOLUTION.second);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}
