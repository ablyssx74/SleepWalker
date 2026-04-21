name			$(NAME)
version			$(VERSION)-1
architecture	$(ARCH)
summary 		"A daemon to launch Terminal and run a script during shutdown or reboot.",
description 	"A daemon to launch Terminal and run a script during shutdown or reboot."
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

