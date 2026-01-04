import pika
import json
from config import RABBIT_HOST, EXCHANGE_NAME, EXCHANGE_TYPE

def get_connection():
    return pika.BlockingConnection(pika.ConnectionParameters(host=RABBIT_HOST))

def publish_message(routing_key, data):
    """ Envoie un message et l'affiche dans la console """
    connection = get_connection()
    channel = connection.channel()
    channel.exchange_declare(exchange=EXCHANGE_NAME, exchange_type=EXCHANGE_TYPE)
    
    message_body = json.dumps(data)
    
    channel.basic_publish(
        exchange=EXCHANGE_NAME,
        routing_key=routing_key,
        body=message_body
    )
    
    # --- AJOUT DU PRINT ICI ---
    print(f" [ENVOI] Clé: '{routing_key}' | Data: {data}")
    # --------------------------

    connection.close()