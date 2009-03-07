PKG_NAME=swistrack
PKG_DIR=swistrack
SCRIPTS_DIR = $(PKG_DIR)/Packages/Debian/scripts
SCRIPTS_DIR2 = scripts
# the gpg key to use to sign the package:
GPG_KEY_ID=6FF7AEA2

all: help

help:
	@echo "Read the Makefile to see what targets are available"

# FIXME: replace with 'upload to PPA'
#upload_package: sbuild_package
#	ssh -t root@csik "reprepro -Vb $(REPREPRO_ROOT) includedeb biophys $(PKG_WORKING_DIR_BASE)/build/new/*.deb"

# debian/control is only automatically generated from
# debian/control.in if such a file already exsist. Thus for generating
# it the first time, use this target:
generate_control:
	DEB_AUTO_UPDATE_DEBIAN_CONTROL=yes fakeroot debian/rules clean

debianized-tree:
	mkdir -p swistrack
	rsync -r --exclude ".svn" debian/ swistrack/debian
	rsync -r --exclude ".svn" ../../Code/ swistrack/Code
	rsync -r --exclude ".svn" ../../3rdParty/ swistrack/3rdParty
	rsync -r --exclude ".svn" ../../Examples/ swistrack/Examples
	rsync -r --exclude ".svn" ../../SwisTrackEnvironment/ swistrack/SwisTrackEnvironment
	cp ../../Makefile swistrack
	cp ../../Makefile.inc swistrack
	cp mkinstalldirs swistrack
	cp install-sh swistrack

# Build source package:
# pass --no-tgz-check arg to debuild so that it will shut up about not finding orig.tar.gz
source_package: debianized-tree
	cd swistrack && debuild --no-tgz-check -S -k$(GPG_KEY_ID)

# Build without using schroot-ed build system:
build_package_locally: debianized-tree
	cd swistrack && debuild --no-tgz-check --lintian-opts -i -I -E
#cd ../../../ && $(SCRIPTS_DIR)/rotate_build_results "$(PKG_NAME)_"

# Build using schroot-ed build system:
sbuild_package: source_package
	$(SCRIPTS_DIR2)/da $(SCRIPTS_DIR2)/sbuildhack $(PKG_NAME)_*.dsc
	$(SCRIPTS_DIR2)/rotate_build_results "$(PKG_NAME)_"
# FIXME
#lintian -i -I -E $(PKG_WORKING_DIR_BASE)/build/new/*.deb

source_package_for_ppa: debianized-tree
	cd swistrack && debuild --no-tgz-check -S -sa -k$(GPG_KEY_ID)

upload_to_ppa: source_package_for_ppa
	dput my-ppa swistrack_4.0-*changes
#	cd swistrack && debuild --no-tgz-check -S -sa -k$(GPG_KEY_ID)

clean:
	echo 'clean'
	-rm -rf swistrack
	-rm swistrack_4.0-*

old-clean:
	-rm debian/files debian/$(PKG_NAME).debhelper.log debian/*.debhelper
	-rm debian/$(PKG_NAME).substvars
	-rm -rf debian/$(PKG_NAME) debian/check_file_copies debian/transform_file_copies
	-rm *cdbs-config_list
	-rm -rf SwisTrack html/
	-rm debian/stamp-makefile-build
	cd ../../ && $(MAKE) clean