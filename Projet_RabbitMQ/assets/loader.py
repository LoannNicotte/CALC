import pygame
from config import *

pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

player_ship = pygame.image.load("assets/player.png").convert_alpha()
alien_green = pygame.image.load("assets/green.png").convert_alpha()
alien_red = pygame.image.load("assets/red.png").convert_alpha()
alien_yellow = pygame.image.load("assets/yellow.png").convert_alpha()

win_txt = pygame.font.SysFont(None, 74).render("YOU WIN!", True, (0, 255, 0))
lose_txt = pygame.font.SysFont(None, 74).render("GAME OVER", True, (255, 0, 0))
game_txt = pygame.font.SysFont(None, 36).render("ENTER to replay, ESC to quit !", True, (255, 255, 255))
