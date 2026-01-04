# 🐰 Rabbit Invaders - Space Invaders Distribué

Ce projet est une réimplémentation du célèbre jeu **Space Invaders** utilisant une architecture distribuée basée sur le middleware **RabbitMQ**.

L'objectif est de démontrer les capacités de découplage, de tolérance aux pannes et de communication asynchrone offertes par RabbitMQ.

## 📋 Prérequis

* **Python 3.x**
* **RabbitMQ** (Serveur installé localement ou via Docker)
* Les bibliothèques Python suivantes :
    pip install pika pygame

## 🚀 Installation & Lancement

### 1. Démarrer RabbitMQ
Si vous utilisez Docker (recommandé), lancez la commande suivante :

```bash
docker run -it --rm --name rabbitmq -p 5672:5672 -p 15672:15672 rabbitmq:3-management
```

Si vous l'avez installé sur Windows, assurez-vous que le service "RabbitMQ" est démarré.

### 2. Lancer le jeu
Un script automatique est fourni pour lancer les 3 composants (Moteur, Affichage, Manette) simultanément sous Windows.

Double-cliquez simplement sur : `lancer_jeu.bat`

*Si vous devez les lancer manuellement (ou sous Linux/Mac), ouvrez 3 terminaux et exécutez :*
1.  `python components/engine.py` (Le Cerveau)
2.  `python components/viewer.py` (L'Écran)
3.  `python components/controller.py` (La Manette)

## 🎮 Comment Jouer

Le jeu se contrôle via la petite fenêtre titrée **"Manette"**.

* **⬅️ Flèche Gauche** : Déplacer le vaisseau à gauche.
* **➡️ Flèche Droite** : Déplacer le vaisseau à droite.
* **Espace** : Tirer.
* **Entrée** : Rejouer (si la partie est finie).
* **Echap** : Quitter.

## 🛠️ Architecture Technique

Le jeu est découpé en 3 micro-services autonomes qui communiquent via un Exchange RabbitMQ de type `Topic` :

1.  **Engine (Moteur)** : Gère la physique, les collisions et l'état du monde. Il ne possède pas d'interface graphique.
2.  **Viewer (Affichage)** : Affiche graphiquement l'état du jeu reçu. S'il plante, le jeu continue de tourner en arrière-plan.
3.  **Controller (Manette)** : Capture les touches clavier et envoie des commandes de déplacement.