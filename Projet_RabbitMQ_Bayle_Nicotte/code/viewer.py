import pygame
import json
import threading
import pika
from assets.loader import *
from config import *

current_state = {"player_x": 400, "aliens": [], "bullets": [], "alien_speed": 5, "alien_moove": (0, 0)}
game_status = "Game"

def on_state_received(ch, method, properties, body):
    global current_state
    current_state = json.loads(body)
    
    # --- AJOUT DU PRINT ICI ---
    # On affiche juste X pour ne pas spammer tout l'écran
    print(f" [RECU] Mise à jour graphismes | Joueur X: {current_state['player_x']}")
    # --------------------------

def on_lifecycle_received(ch, method, properties, body):
    global game_status, current_state
    msg = json.loads(body)
    
    print(f" [RECU] STATUS PARTIE : {msg['status']}")
 
    game_status = msg['status']

def start_state_consumer():
    connection = pika.BlockingConnection(pika.ConnectionParameters(host=RABBIT_HOST))
    channel = connection.channel()
    channel.exchange_declare(exchange=EXCHANGE_NAME, exchange_type=EXCHANGE_TYPE)
    
    result = channel.queue_declare(queue='', exclusive=True)
    queue_name = result.method.queue
    channel.queue_bind(exchange=EXCHANGE_NAME, queue=queue_name, routing_key=KEY_STATE)
    channel.queue_bind(exchange=EXCHANGE_NAME, queue=queue_name, routing_key=KEY_LIFECYCLE)
    
    def master_callback(ch, method, properties, body):
        if method.routing_key == KEY_LIFECYCLE:
            on_lifecycle_received(ch, method, properties, body)
        elif method.routing_key == KEY_STATE:
            on_state_received(ch, method, properties, body)

    channel.basic_consume(queue=queue_name, on_message_callback=master_callback, auto_ack=True)
    channel.start_consuming()

    

threading.Thread(target=start_state_consumer, daemon=True).start()

clock = pygame.time.Clock()

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill((0, 0, 0))

    if game_status == "Game":

        screen.blit(player_ship, (current_state['player_x'] - 30, SCREEN_HEIGHT - 80))
        
        for alien in current_state['aliens']:
            if alien["type"] == "green":
                screen.blit(alien_green, (alien['x'] + current_state["alien_moove"][0], alien['y'] + current_state["alien_moove"][1]))

            elif alien["type"] == "red":
                screen.blit(alien_red, (alien['x'] + + current_state["alien_moove"][0], alien['y']+ current_state["alien_moove"][1]))
            
            elif alien["type"] == "yellow":
                screen.blit(alien_yellow, (alien['x'] + + current_state["alien_moove"][0], alien['y']+ current_state["alien_moove"][1]))

        for bullet in current_state['bullets']:
            pygame.draw.rect(screen, (255, 255, 0), (bullet['x'], bullet['y'], 4, 10))
    
    elif game_status == "Win":
        screen.blit(win_txt, (SCREEN_WIDTH//2 - win_txt.get_width()//2, SCREEN_HEIGHT//2 - win_txt.get_height()//2))
        screen.blit(game_txt, (SCREEN_WIDTH//2 - game_txt.get_width()//2, SCREEN_HEIGHT//2 + 50))
    
    elif game_status == "Lose":
        screen.blit(lose_txt, (SCREEN_WIDTH//2 - lose_txt.get_width()//2, SCREEN_HEIGHT//2 - lose_txt.get_height()//2))
        screen.blit(game_txt, (SCREEN_WIDTH//2 - game_txt.get_width()//2, SCREEN_HEIGHT//2 + 50))

    elif game_status == "Quit":
        running = False
        break

    pygame.display.flip()
    clock.tick(60)