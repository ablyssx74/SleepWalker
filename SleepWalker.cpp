/*
 * Copyright 2026, Kris Beazley "ablyss" jb@epluribusunix.net
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

const uint32 MSG_QUICK_QUIT = 'qqit';

class ShutdownListener : public BApplication {
public:
    ShutdownListener()
        : BApplication("application/x-vnd.SleepWalker", B_SINGLE_LAUNCH) {}

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
        
        if (create_directory(dirPath, 0755) != B_OK) return;

        BEntry entry(scriptPath);
        if (!entry.Exists()) {
            BFile file(scriptPath, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
            if (file.InitCheck() == B_OK) {
                     const char* templateText = 
                    "#!/bin/bash\n"
                    "# SleepWalker Shutdown Script Template\n"
                    "echo 'Executing shutdown tasks...'\n"
                    "notify --icon /system/data/SleepWalker/SleepWalker_64px.png --title 'SleepWalker' 'Shutdown script started'\n"
                    "# Add your commands below:\n\n";
                file.Write(templateText, strlen(templateText));
                entry.SetPermissions(0755);
                
                 BAlert* alert = new BAlert("Template Created", 
                    "A new script template has been created at:\n~/config/settings/SleepWalker/sleepwalker.sh", 
                    "Excellent");
                alert->Go();
            }
        }
    }

    virtual bool QuitRequested() {    	
        system("/boot/system/apps/Terminal /boot/home/config/settings/SleepWalker/sleepwalker.sh");       
        return true; 
    }
};
int main(int argc, char** argv) {
    const char* sig = "application/x-vnd.SleepWalker";

    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("SleepWalker - Shutdown Script Listener for Haiku\n\n");
        printf("Usage:\n");
        printf("  SleepWalker          Launch the daemon (Single Instance)\n");
        printf("  SleepWalker -q       Quit the daemon silently (no script)\n");
        printf("  SleepWalker --help   Show this help message\n\n");
        printf("Script Location: ~/config/settings/SleepWalker/sleepwalker.sh\n");
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "-q") == 0) {
        BMessenger messenger(sig);
        if (messenger.IsValid()) {
            messenger.SendMessage(MSG_QUICK_QUIT);
            printf("SleepWalker shutting down.\n");
        } else {
            printf("SleepWalker is not running.\n");
        }
        return 0;
    }

    ShutdownListener app;

    app.Run();
    return 0;
}

