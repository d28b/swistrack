
clean:
	$(MAKE) clean

build:
	$(MAKE) all

FIND_GROUP = `grep '^root:' /etc/passwd | awk -F: '{ print $$4 }'`
INSTALL_FOLDER=debian/swistrack/usr/bin
install:
	./mkinstalldirs $(INSTALL_FOLDER)
	./install-sh -c -m 0755 -o root -g ${FIND_GROUP} SwisTrackEnvironment/SwisTrack $(INSTALL_FOLDER)
	rm SwisTrackEnvironment/SwisTrack

