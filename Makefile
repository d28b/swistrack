.PHONY: all
all:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C Code/core
	$(MAKE) -C Code/gui

.PHONY: osxbundle
osxbundle:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C Code/core
	$(MAKE) -C Code/gui osxbundle

.PHONY: cygwin
cygwin:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C Code/core
	$(MAKE) -C Code/gui cygwin

.PHONY: batch
batch:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C Code/core
	$(MAKE) -C Code/batch

.PHONY: clean
clean:
	$(MAKE) -C 3rdParty/libtsai clean
	$(MAKE) -C Code/core clean
	$(MAKE) -C Code/gui clean
	$(MAKE) -C Code/batch clean
