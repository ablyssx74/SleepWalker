name			$(NAME)
version			$(VERSION)-1
architecture	$(ARCH)
summary 		"SleepWalker"
description 	"SleepWalker - Shutdown Script Listener for Haiku"
packager		"ablyss <jb@epluribusunix.net>"
vendor			"Haiku Project"
licenses {
	"MIT"
}
copyrights {
	"$(YEAR) ablyss"
}
provides {
	$(NAME) = $(VERSION)-1
}
requires {
	haiku
}	
urls {
	"https://github.com/ablyssx74/"
}

