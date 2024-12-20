#include "dispatchers.hpp"

PHLWINDOW get_circle_next_window (std::string arg) {
	bool next_ready = false;
	PHLWINDOW pTempClient =  g_pCompositor->m_pLastWindow.lock();
    for (auto &w : g_pCompositor->m_vWindows)
    {
		PHLWINDOW pWindow = w;
        if (pTempClient->m_pWorkspace->m_iID !=pWindow->m_pWorkspace->m_iID || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->isFullscreen())
            continue;
		if (next_ready)
			return 	pWindow;
		if (pWindow == pTempClient)
			next_ready = true;
    }

    for (auto &w : g_pCompositor->m_vWindows)
    {
		PHLWINDOW pWindow = w;
        if (pTempClient->m_pWorkspace->m_iID !=pWindow->m_pWorkspace->m_iID || !g_pCompositor->isWorkspaceSpecial(pWindow->m_pWorkspace->m_iID) || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->isFullscreen())
            continue;
		return pWindow;
    }
	return nullptr;
}

void warpcursor_and_focus_to_window(PHLWINDOW pWindow) {
	g_pCompositor->focusWindow(pWindow);
	g_pCompositor->warpCursorTo(pWindow->middle());
	g_pInputManager->m_pForcedFocus = pWindow;
    g_pInputManager->simulateMouseMovement();
    g_pInputManager->m_pForcedFocus = PHLWINDOWREF{};
}

void dispatch_circle(std::string arg)
{
	PHLWINDOW pWindow;
	pWindow = get_circle_next_window(arg);
	if(pWindow){
		warpcursor_and_focus_to_window(pWindow);
	}
}


void minimize_window(std::string arg)
{
	PHLWINDOW pWindow =  g_pCompositor->m_pLastWindow.lock();
	auto pNode = g_hych_Hide->getNodeFromWindow(pWindow);
  	if(pNode && !pNode->isMinimized) {
  	  g_hych_Hide->hideWindowToSpecial(pWindow);
	  hych_log(LOG,"shortcut key toggle minimize window:{}",pWindow);
  	}
}

void restore_minimize_window(std::string arg)
{

	if(g_pCompositor->m_pLastMonitor->activeSpecialWorkspace->m_iID != 0) {
		auto pTargetWindow = g_pCompositor->m_pLastWindow.lock();

		if(!pTargetWindow) {
			return;
		}

		if(!g_pCompositor->isWorkspaceSpecial(pTargetWindow->m_pWorkspace->m_iID)) {
			g_hych_Hide->leaveSpecialWorkspace();
			hych_log(LOG,"special workspace view,do noting");
			return;
		}
		g_hych_Hide->leaveSpecialWorkspace();
		g_hych_Hide->restoreWindowFromSpecial(pTargetWindow);
		hych_log(LOG,"special workspace view,shortcut key toggle restore window:{}",g_pCompositor->m_pLastWindow.lock());
		return;
	}

    for (auto &nd : g_hych_Hide->m_lHideNodesData)
    {
        if(!nd.isMinimized) {
            continue;
        }
		g_hych_Hide->restoreWindowFromSpecial(nd.pWindow);
		hych_log(LOG,"normal workspace view,shortcut key toggle restore window:{}",nd.pWindow);
        break;
	}
}

void focusOneWindowInSpecialWorkspace() {

    for (auto &w : g_pCompositor->m_vWindows) {
        PHLWINDOW pWindow = w;
        if ( g_pCompositor->isWorkspaceSpecial(pWindow->m_pWorkspace->m_iID)) {
            g_pCompositor->focusWindow(pWindow);
            return;
        }
    }
    g_pCompositor->focusWindow(nullptr);
}

void toggle_restore_window(std::string arg)
{
	if(g_pCompositor->m_pLastMonitor->activeSpecialWorkspace->m_iID == 0) {
		g_pKeybindManager->toggleSpecialWorkspace("");
		focusOneWindowInSpecialWorkspace();
	} else if(g_hych_enable_alt_release_exit && !g_pCompositor->m_pLastMonitor->activeSpecialWorkspace->m_iID == 0) {
		dispatch_circle("");
	} else {
		restore_minimize_window("");
		g_hych_Hide->leaveSpecialWorkspace();
	}
}

void registerDispatchers()
{
	HyprlandAPI::addDispatcher(PHANDLE, "hych:minimize", minimize_window);
	HyprlandAPI::addDispatcher(PHANDLE, "hych:restore_minimize", restore_minimize_window);
	HyprlandAPI::addDispatcher(PHANDLE, "hych:toggle_restore_window", toggle_restore_window);

}
