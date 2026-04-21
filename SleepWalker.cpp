/*
 * Copyright 2026, Kris Beazley "ablyss" jb@epluribusunix.net
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Alert.h>
#include <Roster.h>
#include <Messenger.h>
#include <StringList.h>
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

    // Standard shutdown:
    virtual bool QuitRequested() {
        const char* scriptPath = "/boot/home/config/settings/SleepWalker/sleepwalker.sh";
        BEntry entry(scriptPath);
        if (entry.Exists()) {
            BStringList args;
            args.Add("Terminal");
            args.Add(scriptPath);

            int32 argc = args.CountStrings();
            char* argv[argc];
            for (int32 i = 0; i < argc; i++) argv[i] = (char*)args.StringAt(i).String();

            be_roster->Launch("application/x-vnd.Haiku-Terminal", argc, argv);
        }
        return true; 
    }
};
int main(int argc, char** argv) {
    const char* sig = "application/x-vnd.SleepWalker";

    // 1. Handle Help Flag
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("SleepWalker - Shutdown Script Listener for Haiku\n\n");
        printf("Usage:\n");
        printf("  SleepWalker          Launch the daemon (Single Instance)\n");
        printf("  SleepWalker -q       Quit the daemon silently (no script)\n");
        printf("  SleepWalker --help   Show this help message\n\n");
        printf("Script Location: ~/config/settings/SleepWalker/sleepwalker.sh\n");
        return 0;
    }

    // 2. Handle -q (Silent Quit)
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

    // 5. Normal Launch attempt
    ShutdownListener app;

    app.Run();
    return 0;
}

/*
int main(int argc, char** argv) {
    const char* sig = "application/x-vnd.SleepWalker";

    // 1. Handle -q (Silent Quit)
    if (argc > 1 && strcmp(argv[1], "-q") == 0) {
        BMessenger messenger(sig);
        if (messenger.IsValid()) {
            messenger.SendMessage(MSG_QUICK_QUIT);
            printf("SleepWalker shutting down silently.\n");
        } else {
            printf("SleepWalker is not running.\n");
        }
        return 0;
    }

    // 2. Handle -status
    if (argc > 1 && strcmp(argv[1], "-status") == 0) {
        if (be_roster->IsRunning(sig)) {
            printf("SleepWalker is currently ACTIVE.\n");
        } else {
            printf("SleepWalker is NOT running.\n");
        }
        return 0;
    }

    // 3. Normal Launch attempt
    ShutdownListener app;
    if (app.InitCheck() != B_OK) {
        // If we get here with no arguments, it means a launch was 
        // attempted while it was already active.
        if (argc == 1) {
            printf("SleepWalker is already running.\n");
        } else {
            fprintf(stderr, "Error: Could not launch SleepWalker.\n");
        }
        return 1;
    }
    
    app.Run();
    return 0;
}
*/
