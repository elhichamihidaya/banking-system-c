@echo off
echo Compilation du projet BANKFLOW...
gcc src/*.c -o bin/bankflow.exe
if %ERRORLEVEL% EQU 0 (
    echo Compilation reussie ! Lancement de l'application...
    cd bin
    bankflow.exe
    cd ..
) else (
    echo Erreur lors de la compilation.
)
pause
