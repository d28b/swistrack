
.PHONY: all
all:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C 3rdParty/camshift_wrapper
	$(MAKE) -C 3rdParty/cvblobslib_v6p1
	$(MAKE) -C code/core
	$(MAKE) -C code/gui

.PHONY: batch
batch:
	$(MAKE) -C 3rdParty/libtsai
	$(MAKE) -C 3rdParty/camshift_wrapper
	$(MAKE) -C 3rdParty/cvblobslib_v6p1
	$(MAKE) -C code/core
	$(MAKE) -C code/batch

.PHONY: clean
clean:
	$(MAKE) -C 3rdParty/camshift_wrapper clean
	$(MAKE) -C 3rdParty/libtsai clean
	$(MAKE) -C 3rdParty/cvblobslib_v6p1 clean
	$(MAKE) -C code/core clean
	$(MAKE) -C code/gui clean
	$(MAKE) -C code/batch clean
