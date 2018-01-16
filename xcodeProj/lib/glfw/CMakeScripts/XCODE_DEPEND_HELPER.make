# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.glfw.Debug:
/Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3_d.a:
	/bin/rm -f /Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3_d.a


PostBuild.glfw.Release:
/Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a:
	/bin/rm -f /Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a


PostBuild.glfw.MinSizeRel:
/Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a:
	/bin/rm -f /Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a


PostBuild.glfw.RelWithDebInfo:
/Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a:
	/bin/rm -f /Users/yuelei/Desktop/openGitProj/opengl-9th-examples-master/lib/libglfw3.a




# For each target create a dummy ruleso the target does not have to exist
