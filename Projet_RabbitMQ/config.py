# Configuration de RabbitMQ
RABBIT_HOST = 'localhost'
EXCHANGE_NAME = 'space_invaders_topic'
EXCHANGE_TYPE = 'topic'

# Clés de routage (Routing Keys)
KEY_INPUT = 'game.input'       # Pour les commandes (gauche, droite, tir)
KEY_STATE = 'game.state'       # Pour l'état du monde (positions X, Y)
KEY_EVENT = 'game.events'      # Pour les explosions/sons
KEY_LIFECYCLE = 'game.lifecycle' # Pour Win/Lose

# Config Jeu
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600