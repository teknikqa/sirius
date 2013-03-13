set name=Sirius.zip
del output\%name%
cd ..\release
wzzip ..\setup\output\%name% Sirius.exe
cd ..\help
wzzip ..\setup\output\%name% Sirius.html
cd ..\setup
wzzip output\%name% License.rtf Sirius.rtf
set name=
pause
