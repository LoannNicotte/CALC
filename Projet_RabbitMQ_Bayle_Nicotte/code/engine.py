import time
import threading
import json
import pygame
import pika
import random as rnd
from config import *
from rabbit_utils import publish_message

world_state = {}

def initialize_world():
    global world_state

    world_state = {
        "player_x": 400,
        "aliens": [], 
        "bullets": [],
        "alien_moove": (0, 0)
    }

    for x in [100, 200, 300, 400, 500, 600]:
        for y in [50, 150, 250]:
            if rnd.random() < 0.5:
                alien_type = rnd.choice(["green", "red", "yellow"])
                health = {"green": 1, "red": 3, "yellow": 2}[alien_type]
                world_state["aliens"].append({"type": alien_type, "x": x, "y": y, "health": health})

initialize_world()
game_status = "Game"

def on_input_received(ch, method, properties, body):
    global world_state, game_status
    msg = json.loads(body)
    
    # --- AJOUT DU PRINT ICI ---
    print(f" [RECU] Commande Manette : {msg}")
    # --------------------------

    if msg['action'] == 'LEFT':
        world_state['player_x'] -= 10
    elif msg['action'] == 'RIGHT':
        world_state['player_x'] += 10
    elif msg['action'] == 'SHOOT':
        world_state['bullets'].append({"x": world_state['player_x'], "y": SCREEN_HEIGHT - 100})

    if game_status != "Game":
        if msg['action'] == 'REPLAY':
            initialize_world()
            game_status = "Game"
            publish_message(KEY_LIFECYCLE, {"status": "Game"})

        elif msg['action'] == 'QUIT':
            print(" [MOTEUR] Le joueur a quitté la partie.")
            publish_message(KEY_LIFECYCLE, {"status": "Quit"})
            pygame.quit()
            exit()

def start_input_consumer():
    connection = pika.BlockingConnection(pika.ConnectionParameters(host=RABBIT_HOST))
    channel = connection.channel()
    channel.exchange_declare(exchange=EXCHANGE_NAME, exchange_type=EXCHANGE_TYPE)
    
    result = channel.queue_declare(queue='', exclusive=True)
    queue_name = result.method.queue
    channel.queue_bind(exchange=EXCHANGE_NAME, queue=queue_name, routing_key=KEY_INPUT)
    
    channel.basic_consume(queue=queue_name, on_message_callback=on_input_received, auto_ack=True)
    print(" [*] Moteur prêt. En attente de messages...")
    channel.start_consuming()

threading.Thread(target=start_input_consumer, daemon=True).start()

initialize_world()
last_alien_move_time = time.time()
alien_direction = -1  # 1 pour droite, -1 pour gauche
alien_speed = (5, 10)

time.sleep(2)  # Attente pour s'assurer que le viewer est prêt
while True:
    if game_status == "Game":
        # Mise à jour des positions des aliens
        if time.time() - last_alien_move_time > 0.5:
            dx, dy = world_state["alien_moove"]

            if dx == 0 or dx == 50:
                alien_direction *= -1
                dy += alien_speed[1]
            
            dx += alien_speed[0]*alien_direction
            
            world_state["alien_moove"] = (dx, dy)
            last_alien_move_time = time.time()  

        # Mise à jour des positions des balles
        for bullet in world_state['bullets']:
            bullet['y'] -= 10
            if bullet['y'] < -20:
                world_state['bullets'].remove(bullet)

            #collision avec les aliens
            rect_bullet = pygame.Rect(bullet['x'], bullet['y'], 4, 10)
            for alien in world_state['aliens']:
                rect_alien = pygame.Rect(alien['x'] + world_state["alien_moove"][0], alien['y'] + world_state["alien_moove"][1], 40, 30)
                if rect_bullet.colliderect(rect_alien):
                    alien['health'] -= 1
                    if alien['health'] <= 0:
                        world_state['aliens'].remove(alien)
                    if bullet in world_state['bullets']:
                        world_state['bullets'].remove(bullet)
                    break

        # Vérification de la condition de victoire
        for alien in world_state['aliens']:
            if alien['y'] + world_state["alien_moove"][1] > 450:
                game_status = "Lose"
                print(" [MOTEUR] Le joueur a perdu !")
                publish_message(KEY_LIFECYCLE, {"status": "Lose"})
                break

        # CONDITION DE VICTOIRE
        if len(world_state['aliens']) == 0:
            print(" [MOTEUR] Victoire !")
            publish_message(KEY_LIFECYCLE, {"status": "Win"})
            game_status = "Win"


        # Envoi de l'état du monde 
        publish_message(KEY_STATE, world_state)
        # 60 FPS
        time.sleep(0.01666)