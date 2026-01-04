import pygame
from config import *
from rabbit_utils import publish_message

pygame.init()
screen = pygame.display.set_mode((200, 100))
pygame.display.set_caption("Manette")

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
        # Envoi des inputs via RabbitMQ
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                publish_message(KEY_INPUT, {"action": "LEFT"})
            elif event.key == pygame.K_RIGHT:
                publish_message(KEY_INPUT, {"action": "RIGHT"})
            elif event.key == pygame.K_SPACE:
                publish_message(KEY_INPUT, {"action": "SHOOT"})
            elif event.key == pygame.K_RETURN:
                publish_message(KEY_INPUT, {"action": "REPLAY"})
            elif event.key == pygame.K_ESCAPE:
                publish_message(KEY_INPUT, {"action": "QUIT"})

pygame.quit()