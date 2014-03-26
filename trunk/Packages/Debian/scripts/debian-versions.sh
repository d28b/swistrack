#!/bin/sh
#DEBIAN_CODES="etch lenny dapper feisty gutsy hardy"
DEBIAN_CODES="hardy"
TIMESTAMP=`date "++%Y.%m.%d.%H.%M.%S"`
SVN_REV=`svnversion | tr ":" "."`
BUILD_ID=$TIMESTAMP"+r"$SVN_REV
gettag() {
    case "$1" in
	etch)
	    VER_ID="~debian4.0"
	    ;;
	lenny)
	    VER_ID="~debian4.1~0.1"
	    ;;
	dapper)
	    VER_ID="~ubuntu6.06"
	    ;;
	feisty)
	    VER_ID="~ubuntu7.04"
	    ;;
	gutsy)
	    VER_ID="~ubuntu7.10"
	    ;;
	hardy)
	    VER_ID="~ubuntu8.04"
	    ;;
	versions)
	    echo "$DEBIAN_CODES"
	    exit 1
	    ;;
	*)
	    echo "error"
	    exit 1
	    ;;
    esac
    echo $BUILD_ID$VER_ID
}
#gettag hardy
