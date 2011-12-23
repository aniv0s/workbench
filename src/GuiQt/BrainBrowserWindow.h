
#ifndef __BRAIN_BROWSER_WINDOW_H__
#define __BRAIN_BROWSER_WINDOW_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdint.h>

#include <QImage>
#include <QMainWindow>

#include "AString.h"
#include "BrainBrowserWindowScreenModeEnum.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"

class QAction;
class QActionGroup;
class QMenu;

namespace caret {
    class BrainBrowserWindowToolBar;
    class BrainBrowserWindowToolBox;
    class BrainOpenGLWidget;
    class BrowserTabContent;
    class MapScalarDataColorMappingEditorDialog;

    
    /**
     * The brain browser window is the viewer for
     * brain models.  It may contain multiple tabs
     * with each tab displaying brain models.
     */ 
    class BrainBrowserWindow : public QMainWindow, public EventListenerInterface  {
        Q_OBJECT
        
    public:
        virtual ~BrainBrowserWindow();
        
        void receiveEvent(Event* event);
        
        BrowserTabContent* getBrowserTabContent();
        
        QMenu* createPopupMenu();
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        
        int32_t getBrowserWindowIndex() const;

        QImage captureImageOfGraphicsArea(const int32_t imageSizeX,
                                          const int32_t imageSizeY);
        
        BrainBrowserWindowScreenModeEnum::Enum getScreenMode() const;
        
    protected:
        void closeEvent(QCloseEvent* event);
        
    private slots:        
        void processDisplayControl();
        void processNewWindow();
        void processDataFileOpen();
        void processDataFileOpenFromSpecFile();
        void processManageSaveLoadedFiles();
        void processCaptureImage();
        void processEditPreferences();
        void processCloseSpecFile();
        void processExitProgram();
        void processViewScreenActionGroupSelection(QAction*);
        void processMoveAllTabsToOneWindow();
        void processMoveToolBoxToLeft();
        void processMoveToolBoxToRight();
        void processMoveToolBoxToTop();
        void processMoveToolBoxToBottom();
        void processMoveToolBoxToFloat();
        
        void processMoveSelectedTabToWindowMenuAboutToBeDisplayed();
        void processMoveSelectedTabToWindowMenuSelection(QAction*);
        
        void processRecentSpecFileMenuAboutToBeDisplayed();
        void processRecentSpecFileMenuSelection(QAction*);
        
        void shrinkToolbox();
        
        void processFileMenuAboutToShow();
        void processViewMenuAboutToShow();
        
    private:
        // Contains status of components such as enter/exit full screen
        struct WindowComponentStatus {
            bool isToolBoxDisplayed;
            bool isToolBarDisplayed;
        };
        
        BrainBrowserWindow(const int browserWindowIndex,
                           BrowserTabContent* browserTabContent,
                           QWidget* parent = 0,
                           Qt::WindowFlags flags = 0);
        
        BrainBrowserWindow(const BrainBrowserWindow&);
        BrainBrowserWindow& operator=(const BrainBrowserWindow&);
        
        void createActions();
        void createMenus();
        
        QMenu* createMenuFile();
        QMenu* createMenuView();
        QMenu* createMenuViewMoveToolBox();
        QMenu* createMenuData();
        QMenu* createMenuSurface();
        QMenu* createMenuVolume();
        QMenu* createMenuWindow();
        QMenu* createMenuHelp();
        
        void moveToolBox(Qt::DockWidgetArea area);
        
        void restoreWindowComponentStatus(const WindowComponentStatus& wcs);
        void saveWindowComponentStatus(WindowComponentStatus& wcs);
        
        void openSpecFile(const AString& specFileName) throw (DataFileException);
        
        /** Index of this window */
        int32_t browserWindowIndex;
        
        BrainOpenGLWidget* openGLWidget;
        
        BrainBrowserWindowToolBar* toolbar;
        
        QAction* newWindowAction;
        
        QAction* newTabAction;
        
        QAction* openFileAction;
        
        QAction* openFileViaSpecFileAction;
        
        QAction* manageFilesAction;
        
        QAction* closeSpecFileAction;
        
        QAction* closeTabAction;
        
        QAction* closeWindowAction;
        
        QAction* captureImageAction;
        
        QAction* preferencesAction;
        
        QAction* exitProgramAction;
        
        QAction* showToolBarAction;
        
        QAction* displayControlAction;
        
        QActionGroup* viewScreenActionGroup;
        QAction* viewScreenNormalAction;
        QAction* viewScreenFullAction;
        QAction* viewScreenMontageTabsAction;
        QAction* viewScreenFullMontageTabsAction;        
        
        QAction* viewMenuShowToolBoxAction;
        
        QAction* nextTabAction;
        
        QAction* previousTabAction;
        
        QAction* renameSelectedTabAction;
        
        QAction* moveTabsInWindowToNewWindowsAction;
        
        QAction* moveTabsFromAllWindowsToOneWindowAction;
        
        QAction* bringAllToFrontAction;
        
        QAction* helpOnlineAction;
        
        QAction* searchHelpOnlineAction;
        
        QMenu* moveSelectedTabToWindowMenu;
        
        QMenu* recentSpecFileMenu;
        
        BrainBrowserWindowToolBox* toolBox;
        
        AString previousOpenFileNameFilter;
        
        BrainBrowserWindowScreenModeEnum::Enum screenMode;
        
        WindowComponentStatus normalWindowComponentStatus;
                
        /* Editor for scalar color mapping. */
        MapScalarDataColorMappingEditorDialog* scalarDataColorMappingEditor;
        
        friend class GuiManager;
    };
}
#endif // __BRAIN_BROWSER_WINDOW_H__

