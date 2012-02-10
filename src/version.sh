#/bin/sh

svnversion=`LC_ALL=C svn info | awk '/^Revision:/ {print $2}'`
svndate=`LC_ALL=C svn info | awk '/^Last Changed Date:/ {print $4}'`
gendate=`LC_ALL=C date`
cat <<EOF > version.h

// Do not edit!
//
// generated from $0 on $gendate

#define DENGON_SVNVERSION "$svnversion"
#define DENGON_SVNDATE "$svndate"

EOF

