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

const uint32 MSG_QUICK_QUIT = 'qqit';
extern char **environ; 
class ShutdownListener : public BApplication {
public:
    ShutdownListener()
        : BApplication("application/x-vnd.Be-SYS.SleepWalker", B_SINGLE_LAUNCH)
        {}
        
		  
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
