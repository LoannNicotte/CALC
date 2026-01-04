@echo off
echo ==========================================
echo Lancement de Rabbit Invaders Distribué...
echo ==========================================

:: 1. Lancer le Moteur (Engine) dans une nouvelle fenêtre
:: "cmd /k" permet de garder la fenêtre ouverte si ça plante (pour lire l'erreur)
start "1 - Cerveau (Moteur)" cmd /k python engine.py

:: Petite pause de 2 secondes pour laisser le temps à RabbitMQ de connecter le moteur
timeout /t 2 /nobreak >nul

:: 2. Lancer l'Affichage (Viewer)
start "2 - Ecran (Viewer)" cmd /k python viewer.py

:: 3. Lancer la Manette (Controller)
start "3 - Manette (Controller)" cmd /k python controller.py

echo.
echo Tout est lance ! Regarde les autres fenetres.
echo Tu peux fermer cette fenetre ci.
pause