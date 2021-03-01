# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.AncientGreece.Debug:
/Users/alex/Code/AncientGreece/project_files/Debug/AncientGreece:\
	/usr/local/lib/liblua.dylib
	/bin/rm -f /Users/alex/Code/AncientGreece/project_files/Debug/AncientGreece


PostBuild.AncientGreece.Release:
/Users/alex/Code/AncientGreece/project_files/Release/AncientGreece:\
	/usr/local/lib/liblua.dylib
	/bin/rm -f /Users/alex/Code/AncientGreece/project_files/Release/AncientGreece


PostBuild.AncientGreece.MinSizeRel:
/Users/alex/Code/AncientGreece/project_files/MinSizeRel/AncientGreece:\
	/usr/local/lib/liblua.dylib
	/bin/rm -f /Users/alex/Code/AncientGreece/project_files/MinSizeRel/AncientGreece


PostBuild.AncientGreece.RelWithDebInfo:
/Users/alex/Code/AncientGreece/project_files/RelWithDebInfo/AncientGreece:\
	/usr/local/lib/liblua.dylib
	/bin/rm -f /Users/alex/Code/AncientGreece/project_files/RelWithDebInfo/AncientGreece




# For each target create a dummy ruleso the target does not have to exist
/usr/local/lib/liblua.dylib:
