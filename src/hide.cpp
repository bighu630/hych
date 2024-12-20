#include "hide.hpp"

SHideNodeData *Hide::getNodeFromWindow(PHLWINDOW pWindow)
{
    for (auto &nd : m_lHideNodesData)
    {
        if (nd.pWindow == pWindow)
            return &nd;
    }

    return nullptr;
}

void Hide::refocusToSourceWorkspaceAfterMove(int workspaceID) {
    for (auto &w : g_pCompositor->m_vWindows)
    {
	    PHLWINDOW pWindow = w;
        if ((!g_pCompositor->isWorkspaceSpecial(pWindow->m_pWorkspace->m_iID)) || pWindow->m_pWorkspace->m_iID != workspaceID || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->isFullscreen())
            continue;
        g_pCompositor->focusWindow(pWindow);
        return;
    }
	g_pCompositor->focusWindow(nullptr);
}

void Hide::moveWindowToSpecialWorkspace(PHLWINDOW pWindow) {
    std::string workspaceName = "";

    // TODO: 魔法数+1
    const int WORKSPACEID = 86;

    if (WORKSPACEID == INT_MAX) {
        Debug::log(ERR, "Error in moveActiveToWorkspaceSilent, invalid value");
        return;
    }

    if (WORKSPACEID == pWindow->m_pWorkspace->m_iID)
        return;

    g_pHyprRenderer->damageWindow(pWindow);

    auto       pWorkspace = g_pCompositor->getWorkspaceByID(WORKSPACEID);

    if (pWorkspace) {
        g_pCompositor->moveWindowToWorkspaceSafe(pWindow, pWorkspace);
    } else {
        pWorkspace = g_pCompositor->createNewWorkspace(WORKSPACEID, pWindow->m_pMonitor->ID, workspaceName);
        g_pCompositor->moveWindowToWorkspaceSafe(pWindow, pWorkspace);
    }
}

void Hide::leaveSpecialWorkspace() {
	const auto pMonitor = g_pCompositor->m_pLastMonitor;
	pMonitor->setSpecialWorkspace(nullptr);

}

void Hide::hideWindowToSpecial(PHLWINDOW pWindow) {

    auto pNode = getNodeFromWindow(pWindow);
    int workspaceID = pWindow->m_pWorkspace->m_iID;
    if(!pNode)
        return;

    pNode->isMinimized = true;

    moveWindowToSpecialWorkspace(pWindow);
    refocusToSourceWorkspaceAfterMove(workspaceID);
    wlr_foreign_toplevel_handle_v1_set_activated(pWindow->m_phForeignToplevel, false);
}


void Hide::restoreWindowFromSpecial(PHLWINDOW pWindow) {

    PHLWORKSPACE pWorkspace;
    auto pNode = getNodeFromWindow(pWindow);

    if(!pNode)
        return;

    pNode->isMinimized = false;

    if(g_hych_restore_to_old_workspace) {
        pWorkspace = g_pCompositor->getWorkspaceByID(pNode->hibk_workspaceID);
        if (!pWorkspace){
            hych_log(LOG,"source workspace no exist");
            pWorkspace = g_pCompositor->createNewWorkspace(pNode->hibk_workspaceID, pNode->pWindow->m_pMonitor->ID,pNode->hibk_workspaceName);
        }
    } else {
        pWorkspace =  g_pCompositor->getWorkspaceByID(g_pCompositor->m_pLastMonitor->activeWorkspace);
    }

    auto pMonitor = g_pCompositor->getMonitorFromID(pWorkspace->m_pMonitor->ID);
    g_pCompositor->moveWindowToWorkspaceSafe(pWindow, pWorkspace);
    pMonitor->changeWorkspace(pWorkspace);


    wlr_foreign_toplevel_handle_v1_set_minimized(pWindow->m_phForeignToplevel, false);
    g_pCompositor->focusWindow(pWindow);
}
