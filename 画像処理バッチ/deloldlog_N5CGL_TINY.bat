cd c:\N5CGL_TINY\exe\log
for /f "skip=500" %%A in ('dir *.log /b /o-d') do del %%A