@echo off
title rename for visual studio(pap)
color 02
cls
echo select your need change model.
echo (1)all (2)billing (3)sharememory (4)login (5)world (6)server
set /p select=your select?
echo revert to standard?
set revert=-1
set /p revert=(yes/no)?
set all="billing sharememory login world server"
if 1 == %select% echo "php ../php/vcbuild.php" %all% %revert%
if 2 == %select% php ../php/vcbuild.php "billing" %revert%
if 3 == %select% php ../php/vcbuild.php "sharememory" %revert%
if 4 == %select% php ../php/vcbuild.php "login" %revert%
if 5 == %select% php ../php/vcbuild.php "world" %revert%
if 6 == %select% php ../php/vcbuild.php "server" %revert%

pause