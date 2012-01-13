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
/*LICENSE_END*/


#include <QApplication>
#include <QGLFormat>
#include <QGLPixelBuffer>
#include <QSplashScreen>
#include <QThread>

#include <cstdlib>
#include <iostream>

#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "EventBrowserWindowNew.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ProgramParameters.h"
#include "SessionManager.h"
#include "SystemUtilities.h"
#include "WuQtUtilities.h"
#include "FileInformation.h"

static bool caretLoggerIsValid = false;

using namespace caret;
using namespace std;

/*
 * From http://developer.qt.nokia.com/wiki/How_to_create_a_splash_screen_with_an_induced_delay
 */ 
class Sleeper : public QThread
{
public:
    static void sleep(unsigned long secs) {
        QThread::sleep(secs);
    }
};

/**
 * Handles message produced by Qt.
 */
static void
messageHandlerForQt(QtMsgType type, const char* msg)
{
    const AString backtrace = SystemUtilities::getBackTrace();
    
    const AString message = (AString(msg) + "\n" + backtrace);
    
    if (caretLoggerIsValid) {
        switch (type) {
            case QtDebugMsg:
                if (CaretLogger::getLogger()->isInfo()) {
                    CaretLogInfo(message);
                }
                else {
                    std::cerr << "Qt Debug: " << message.toCharArray() << std::endl;
                }
                break;
            case QtWarningMsg:
                if (CaretLogger::getLogger()->isWarning()) {
                    CaretLogWarning(message);
                }
                else {
                    std::cerr << "Qt Warning: " << message.toCharArray() << std::endl;
                }
                break;
            case QtCriticalMsg:
                if (CaretLogger::getLogger()->isSevere()) {
                    CaretLogSevere(message);
                }
                else {
                    std::cerr << "Qt Critical: " << message.toCharArray() << std::endl;
                }
                break;
            case QtFatalMsg:
                if (CaretLogger::getLogger()->isSevere()) {
                    CaretLogSevere(message);
                }
                else {
                    std::cerr << "Qt Fatal: " << message.toCharArray() << std::endl;
                }
                std::abort();
                break;
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
                std::cerr << "Qt Debug: " << message.toCharArray() << std::endl;
                break;
            case QtWarningMsg:
                std::cerr << "Qt Warning: " << message.toCharArray() << std::endl;
                break;
            case QtCriticalMsg:
                std::cerr << "Qt Critical: " << message.toCharArray() << std::endl;
                break;
            case QtFatalMsg:
                std::cerr << "Qt Fatal: " << message.toCharArray() << std::endl;
                std::abort();
                break;
        }
    }
}

//struct for communicating stuff back to main from parseCommandLine
struct ProgramState
{
    vector<AString> fileList;
    int specLoadType;
    ProgramState();
};

//declare the functions associated with command line
void printHelp(const AString& progName);
void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState);

int 
main(int argc, char* argv[])
{
    int result;
    {
        /*
        * Handle uncaught exceptions
        */
        SystemUtilities::setHandlersForUnexpected();
        
        qInstallMsgHandler(messageHandlerForQt);
        QApplication app(argc, argv);
        QApplication::setApplicationName("Connectome Workbench");
        QApplication::setApplicationVersion("0");
        QApplication::setOrganizationDomain("brainvis.wustl.edu");
        QApplication::setOrganizationName("Van Essen Lab");
        
        /*
        * Make sure OpenGL is available.
        */
        if (!QGLFormat::hasOpenGL()) { 
            qWarning( "This system has no OpenGL support. Exiting." );
            return -1;
        }
            
        /*
        * Setup OpenGL
        */
        QGLFormat glfmt;
        glfmt.setRedBufferSize(8);
        glfmt.setGreenBufferSize(8);
        glfmt.setBlueBufferSize(8);
        glfmt.setDoubleBuffer(true);
        glfmt.setDirectRendering(true);
        QGLFormat::setDefaultFormat(glfmt);
        
        
        /*
        * Create the session manager.
        */
        SessionManager::createSessionManager();
        caretLoggerIsValid = true;

        /*
        * Parameters for the program.
        */
        ProgramParameters* parameters = new ProgramParameters(argc, argv);

        /*
        * Log the command parameters.
        */
        CaretLogFine("Running: "
                    + AString(argv[0])
                    + " "
                    + parameters->getAllParametersInString());
        
        //begin parsing command line
        ProgramState myState;
        FileInformation progInfo(argv[0]);
        AString progName = progInfo.getFileName();
        parseCommandLine(progName, parameters, myState);
        
        /*
        * Splash Screen
        */
        QPixmap splashPixmap;
        QSplashScreen splashScreen;
        if (WuQtUtilities::loadPixmap(":/splash_hcp.png", splashPixmap)) {
            splashScreen.setPixmap(splashPixmap);
            splashScreen.showMessage("Starting Workbench...");
            splashScreen.show();
            app.processEvents();
            Sleeper::sleep(2);
        }
        
        //begin using parsed command line
        bool haveSpec = false;
        bool haveFiles = false;
        for (int i = 0; i < (int)myState.fileList.size(); ++i)
        {
            if (myState.fileList[i].endsWith(".spec"))
            {
                haveSpec = true;
            } else {
                haveFiles = true;
            }
        }
        //error to have both
        if (haveFiles && haveSpec)
        {
            cerr << "error, cannot specify both spec files and data files on the command line" << endl;
            printHelp(progName);
            return -1;
        }
        if (haveFiles)
        {
        }
        if (haveSpec)
        {
            if (myState.fileList.size() > 1)
            {
                cerr << "error, cannot specify more than one spec file" << endl;
                printHelp(progName);
                return -1;
            }
            switch (myState.specLoadType)
            {
                case 0://load none
                    break;
                case 1://use spec dialog
                    break;
                case 2://load all
                    break;
                default:
                    CaretAssert(false);
            }
        }
        
        /*
        * Create the GUI Manager.
        */
        GuiManager::createGuiManager();
        
        /*
        * Create and display a main window.
        * If not done as pointer, the event object is listed as an
        * object that was not deleted by CaretObject::printListOfObjectsNotDeleted
        * since it does not go out of scope.
        */
        /*EventBrowserWindowNew* newBrowserWindow = new EventBrowserWindowNew(NULL, NULL);
        EventManager::get()->sendEvent(newBrowserWindow->getPointer());
        delete newBrowserWindow;//*/
        
        //TSC: do this with explicit scoping instead of new, so that it does go out of scope.
        //Better idea would be to have EVERYTHING except CaretObject::printListOfObjectsNotDeleted(true); in a single explicit scope
        {
            EventBrowserWindowNew newBrowserWindow(NULL, NULL);
            EventManager::get()->sendEvent(newBrowserWindow.getPointer());
        }
        //GuiManager::get()->newBrainBrowserWindow(NULL);
        
        if (QGLPixelBuffer::hasOpenGLPbuffers()) {
            CaretLogConfig("OpenGL PBuffers are supported");
        }
        else {
            CaretLogConfig("OpenGL PBuffers are NOT supported");
        }
        
        /*
        * Start the app which will launch the main window.
        */
        splashScreen.close();
        result = app.exec();
        
        /*
        * Hiding the window removes it from the event loop on Windows, which is necessary to
        * prevent paint events from causing assertion errors when the Window is destroyed
        * Although this is a Window's only bug, it's probably good practice to do on all platforms
        */
        //theMainWindow->hide();
        
        /*
        * Delete the GUI Manager.
        */
        GuiManager::deleteGuiManager();
        
        /*
        * Delete the session manager.
        */
        SessionManager::deleteSessionManager();
        
        CaretHttpManager::deleteHttpManager();
        
        delete parameters;
    }
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    
    return result;
}

void printHelp(const AString& progName)
{
    cout
    << "Usage: " << progName << " [options] [files]" << endl
    << endl
    << "    [files], if present, can be a single spec file, or multiple data files" << endl
    << endl
    << "Options:" << endl
    << "    -help" << endl
    << "        display this usage text" << endl
    << endl
    << "    -style <style-name>" << endl
    << "        change the window style to the specified style" << endl
    << "        the following styles should always be valid:" << endl
    << "        motif" << endl
    << "        windows" << endl
    << "        platinum" << endl
    << endl
    << "        other styles may be available" << endl
    << endl
    << "    -spec-load-all" << endl
    << "        load all files in the given spec file, don't show spec file dialog" << endl
    << endl
    << "    -spec-load-none" << endl
    << "        don't load any files from the spec file, or show spec file dialog" << endl
    << endl;
}

void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState)
{
    while (myParams->hasNext())
    {
        AString thisParam = myParams->nextString("option");
        if (thisParam[0] == '-')
        {
            if (thisParam == "-style")
            {
                myParams->nextString("style");//discard, QApplication handles this
            } else if (thisParam == "-help") {
                printHelp(progName);
                exit(0);
            } else if (thisParam == "-spec-load-all") {
                myState.specLoadType = 2;
            } else if (thisParam == "-spec-load-none") {
                myState.specLoadType = 0;
            } else {
                cerr << "unrecognized option \"" << thisParam << "\"" << endl;
                printHelp(progName);
                exit(-1);
            }
        } else {
            myState.fileList.push_back(thisParam);
        }
    }
}

ProgramState::ProgramState()
{
    specLoadType = 1;//0: none, 1: use spec window, 2: all
}
