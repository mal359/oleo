# Check to see if we're running under EMX, without using
# AC_CANONICAL_*.  If so, set output variable EXEEXT to ".exe".
# Otherwise set it to "".

dnl AM_EMX()
dnl This lines were done with a quick look to the cygwin neighbour
dnl
AC_DEFUN(AM_EMX,  
[AC_CACHE_CHECK(for EMX environment, am_cv_emx,
[cat > conftest.$ac_ext << 'EOF' 
int main () { 
/* Nothing.  */
return 0; }
EOF
if AC_TRY_EVAL(ac_link -Zexe) && test -s conftest.exe; then
   am_cv_emx=yes
   x_includes=$X11ROOT/XFree86/include
   x_libraries=$X11ROOT/XFree86/lib
else
   am_cv_emx=no
fi
rm -f conftest*])
EXEEXT=
test "$am_cv_emx" = yes && EXEEXT=.exe
AC_SUBST(EXEEXT)])
