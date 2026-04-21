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
#include <StringList.h>
#include <string.h>

class ShutdownListener : public BApplication {
public:
    ShutdownListener()
        : BApplication("application/x-vnd.SleepWalker") {}

    virtual void ReadyToRun() {
        const char* dirPath = "/boot/home/config/settings/SleepWalker";
        const char* scriptPath = "/boot/home/config/settings/SleepWalker/sleepwalker.sh";
        
        // 1. Ensure directory exists
        if (create_directory(dirPath, 0755) != B_OK) {
            BAlert* alert = new BAlert("Error", "Could not create directory /SleepWalker", "OK");
            alert->Go();
            return;
        }

        // 2. Ensure script exists, or create a template
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
                
                // 3. Make the script executable
                entry.SetPermissions(0755);

                BAlert* alert = new BAlert("Template Created", 
                    "A new script template has been created at:\n~/config/settings/SleepWalker/sleepwalker.sh", 
                    "Excellent");
                alert->Go();
            }
        }
    }

    virtual bool QuitRequested() {
        const char* scriptPath = "/boot/home/config/settings/SleepWalker/sleepwalker.sh";
        const char* terminalSignature = "application/x-vnd.Haiku-Terminal";
        
        BEntry entry(scriptPath);
        if (entry.Exists()) {
            BStringList args;
            args.Add("Terminal");
            args.Add(scriptPath);

            int32 argc = args.CountStrings();
            char* argv[argc];
            for (int32 i = 0; i < argc; i++) {
                argv[i] = (char*)args.StringAt(i).String();
            }

            be_roster->Launch(terminalSignature, argc, argv);
        }
        
        return true; 
    }
};

int main() {
    ShutdownListener app;
    app.Run();
    return 0;
}
