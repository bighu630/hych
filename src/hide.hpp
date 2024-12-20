#pragma once
#include "globals.hpp"

struct SHideNodeData
{
  PHLWINDOW pWindow = nullptr;

  bool isMinimized = false;

  int hibk_workspaceID = -1;
  std::string hibk_workspaceName = "";

  bool operator==(const SHideNodeData &rhs) const
  {
    return pWindow == rhs.pWindow;
  }
};

class Hide {
public:
  SHideNodeData *getNodeFromWindow(PHLWINDOW pWindow);
  void hideWindowToSpecial(PHLWINDOW pWindow);
  void restoreWindowFromSpecial(PHLWINDOW pWindow);
  void refocusToSourceWorkspaceAfterMove(int workspaceID);
  void moveWindowToSpecialWorkspace(PHLWINDOW pWindow);
  void leaveSpecialWorkspace();
  std::list<SHideNodeData> m_lHideNodesData;

};


inline std::unique_ptr<Hide> g_hych_Hide;
