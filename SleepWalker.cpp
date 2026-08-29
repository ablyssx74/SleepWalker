/*
 * Copyright 2026, Kris Beazley "ablyss" jb@epluribusunix.net
 * AI Assisted
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Roster.h>
#include <Alert.h>
#include <Messenger.h>
#include <string.h>
#include <cstdio>
#include <image.h>
#include <OS.h>
#include <sys/stat.h>
#include <Notification.h>

namespace AppInfo {
	static const char* const APP_NAME = "SleepWalker";
    static const char* const VERSION_STRING = "v1.0.1";

}

// =============================================================================
// Update Checker
// =============================================================================
static int32 BackgroundUpdateChecker(void* data) {
    snooze(5000000); 

    printf("[UpdateChecker] Asynchronous curl update checker running...\n");

    const char* targetUrl = "https://raw.githubusercontent.com/ablyssx74/SleepWalker/refs/heads/main/VERSION";

    BString shellCmdString;
    #if defined(__x86_64__)
        shellCmdString.SetToFormat("curl -sL \"%s\"", targetUrl);
    #else
        shellCmdString.SetToFormat("curl-x86 -sL \"%s\"", targetUrl);
    #endif

    BString remoteVersionStr = "";
    
    FILE* pipeStream = popen(shellCmdString.String(), "r");
    if (pipeStream != nullptr) {
        char buffer[128] = {0};
        if (fgets(buffer, sizeof(buffer), pipeStream) != nullptr) {
            remoteVersionStr = buffer;
        }
        pclose(pipeStream);
    }

    remoteVersionStr.Trim(); 
     printf("[UpdateChecker] Raw text received from GitHub: '%s'\n", remoteVersionStr.String());

    if (remoteVersionStr.Length() > 0) {
    	BString currentAppStr = AppInfo::APP_NAME;
        BString currentVersionStr = AppInfo::VERSION_STRING;
        printf("[UpdateChecker] Local AppInfo text before cleaning: '%s'\n", currentVersionStr.String());

        int32 curMajor = 0, curMinor = 0, curRevision = 0;
        int32 remMajor = 0, remMinor = 0, remRevision = 0;


        if (sscanf(currentVersionStr.String(), "%*[^v]v%d.%d.%d", &curMajor, &curMinor, &curRevision) != 3) {
            sscanf(currentVersionStr.String(), "%*[^0-9]%d.%d.%d", &curMajor, &curMinor, &curRevision);
        }

        if (sscanf(remoteVersionStr.String(), "%*[^v]v%d.%d.%d", &remMajor, &remMinor, &remRevision) != 3) {
            sscanf(remoteVersionStr.String(), "%*[^0-9]%d.%d.%d", &remMajor, &remMinor, &remRevision);
        }

        
            printf("[UpdateChecker] Cleaned local target string: '%d.%d.%d'\n", curMajor, curMinor, curRevision);
        

        int32 currentFlattened = (curMajor * 10000) + (curMinor * 100) + curRevision;
        int32 remoteFlattened  = (remMajor * 10000) + (remMinor * 100) + remRevision;

      
            printf("[UpdateChecker] Calculated values for math match -> Local: %d | Remote: %d\n", 
                   (int)currentFlattened, (int)remoteFlattened);
        


			if (remoteFlattened > currentFlattened) {
			    printf("[UpdateChecker] Update found! Sending notification...\n");
			
			    BNotification updateAlert(B_INFORMATION_NOTIFICATION);
			    updateAlert.SetGroup(currentAppStr);
			    updateAlert.SetTitle("Update Available");
			    
			    BString alertContent;
			    // Added spaces around currentAppStr so it reads: "of GLToogle is available!"
			    alertContent << "A newer version of " << currentAppStr << " is available! (" << remoteVersionStr << ")";
			    updateAlert.SetContent(alertContent.String());
			    
			    updateAlert.Send();
			    printf("[UpdateChecker] Toast notification sent successfully.\n");
			} else {
			    printf("[UpdateChecker] Client binary is up to date.\n");
			}

		    } else {
		        printf("[UpdateChecker] CRITICAL ERR: Raw text data read from pipe buffer was empty!\n");
		    }
    
    return B_OK;
}


const uint32 MSG_QUICK_QUIT = 'qqit';
extern char **environ; 
class ShutdownListener : public BApplication {
public:
    ShutdownListener()
        : BApplication("application/x-vnd.Be-SYS.SleepWalker", B_SINGLE_LAUNCH)
        {
        	
        // =========================================================================
        // AUTOMATED BACKGROUND UPDATE CHECKER THREAD INITIALIZATION
        // =========================================================================
        thread_id updateThread = spawn_thread(BackgroundUpdateChecker, "UpdateCheckerThread", B_NORMAL_PRIORITY, this);
        if (updateThread >= 0) {
            resume_thread(updateThread);
        }
        // =========================================================================
        	
        	
        	
        }
        

      	  
    virtual void MessageReceived(BMessage* msg) {
        if (msg->what == MSG_QUICK_QUIT) {
            Quit(); 
        } else {
            BApplication::MessageReceived(msg);
        }
    }

    virtual void ReadyToRun() {
        const char* dirPath = "/boot/home/config/settings/SleepWalker";
        const char* scriptPath = "/boot/home/config/settings/SleepWalker/sleepwalker.sh";
        const char* launchDir = "/boot/home/config/settings/boot/launch";
        const char* symlinkPath = "/boot/home/config/settings/boot/launch/SleepWalker";
        const char* appPath = "/boot/system/apps/SleepWalker";
        
        if (create_directory(dirPath, 0755) != B_OK) return;

        BEntry entry(scriptPath);
        if (!entry.Exists()) {
            BFile file(scriptPath, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
            if (file.InitCheck() == B_OK) {
				const char* templateText = 
    			"#!/bin/bash\n"
    			"# SleepWalker Shutdown Script Template\n"
    			"notify --icon /system/data/SleepWalker/SleepWalker_64px.png --title 'SleepWalker' 'Shutdown script started'\n"
    			"\n"
    			"# Add your custom script here\n"
    			"\n"
    			"# ---- DONT EDIT BELOW HERE ----\n"
    			"# Check for dynamic reboot/shutown action from SleepWalker app\n"
    			"if [ -f /tmp/sleepwalker_action ]; then\n"
    			"    ACTION=$(cat /tmp/sleepwalker_action)\n"
    			"    rm /tmp/sleepwalker_action\n"
    			"    $ACTION\n"
    			"fi\n";
                file.Write(templateText, strlen(templateText));
                entry.SetPermissions(0755);
                
                BAlert* alert = new BAlert("Template Created", 
                    "A new script template has been created at:\n~/config/settings/SleepWalker/sleepwalker.sh", 
                    "Excellent");
                alert->Go();
            }
        }
        
        BEntry linkEntry(symlinkPath);
        if (!linkEntry.Exists()) {
            if (create_directory(launchDir, 0755) == B_OK) {
                BDirectory launchBDir(launchDir);
                if (launchBDir.CreateSymLink(symlinkPath, appPath, NULL) == B_OK) {
                    BAlert* autoAlert = new BAlert("Auto-start Enabled", 
                        "SleepWalker has been added to your startup folder.", 
                        "Great");
                    autoAlert->Go();
                }
            }
        }        
    }

	virtual bool QuitRequested() {
		
     //------------
	 // If all we want to do is run the shutdown script if nebula is not installed then enable this.
	 // By Default this is off but can be compiled in with `make CHK_GL=ON package`
	  #ifdef CHK_GL
        struct stat mesaBuffer;
        bool hasHardwareDriver = (stat("/boot/system/add-ons/opengl/egl_vendor.d/libEGL_mesa.so", &mesaBuffer) == 0);        
        if (hasHardwareDriver) {
            return true;
        }
      #endif
     //------------  
     
    	const char* args[] = {
        	"/bin/bash",
        	"/boot/home/config/settings/SleepWalker/sleepwalker.sh",
        	NULL
    	};

    	thread_id thread = load_image(2, args, (const char**)environ);
    	if (thread >= B_OK) {
        	resume_thread(thread);
    	}

    	bool isReboot = false;
    	BMessage* msg = CurrentMessage();

    	BAlert* alert = new BAlert("SleepWalker", "SleepWalker needs time to process your scripts.\n\nProceed when you are ready.", 
           "Cancel", "Poweroff", "Reboot", 
            B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
        
        int32 selection = alert->Go();
        if (selection == 0) return false; 
        
        isReboot = (selection == 2);
 
    	BFile actionFile("/tmp/sleepwalker_action", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
    	if (actionFile.InitCheck() == B_OK) {
        	const char* cmd = isReboot ? "shutdown -r\n" : "shutdown\n";
        	actionFile.Write(cmd, strlen(cmd));
        	actionFile.Sync(); 
    	}

    	return true;
	}

};

int main(int argc, char** argv) {
   // const char* sig = "application/x-vnd.SleepWalker";
   const char* sig = "application/x-vnd.Be-SYS.SleepWalker";

    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("SleepWalker - Shutdown Script Listener for Haiku\n\n");
        printf("Usage:\n");
        printf("  SleepWalker          Launch the daemon (Single Instance)\n");
        printf("  SleepWalker -q       Quit the daemon silently (no script)\n");
        printf("  SleepWalker --help   Show this help message\n\n");
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "-q") == 0) {
        BMessenger messenger(sig);
        if (messenger.IsValid()) {
            messenger.SendMessage(MSG_QUICK_QUIT);
        }
        return 0;
    }

    ShutdownListener app;
    app.Run();
    return 0;
}
