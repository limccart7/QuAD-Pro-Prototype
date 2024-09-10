import redis
import re
from flask import Flask, render_template, request, redirect, url_for
from flask_socketio import SocketIO
import redis.exceptions

# Redis streams configuration
REDIS_HOST = "localhost"
STREAM_NAME = "mystream"
CONSUMER_GROUP = "webGroup"
CONSUMER_NAME = "webConsumer"
INBOUND_KEY = "CDP_WEB"
OUTBOUND_KEY = "WEB_CDP"

# Defines hotspot IP address
IP_ADDRESS = '10.42.0.1'

# Instantiate a Flask app and SocketIO object
app = Flask(__name__, static_url_path='/static')
socket = SocketIO(app)

# Connect to redis server and create consumer group
redis_stream = redis.Redis(host=REDIS_HOST, port=6379)

def redis_init():
    # Create consumer group for web portal
    try:
        redis_stream.xgroup_create(STREAM_NAME, CONSUMER_GROUP, id=0)
    # Handle exception raised when group already exists
    except(redis.exceptions.ResponseError):
        print("Either stream not created, or group already created")
    
    # Check for left over pending messages
    try:
        pend_reply = redis_stream.xpending(STREAM_NAME, CONSUMER_GROUP)
    # Handle exception stream doesn't exist
    except(redis.exceptions.ResponseError):
        print("Stream not created")
        return False

    if(pend_reply == None):
        print("Pending message check error")
        return False
    # Acknowledge any left over pending messages
    elif(pend_reply['pending'] > 0):
        detailed_reply = redis_stream.xpending_range(STREAM_NAME, CONSUMER_GROUP, pend_reply['min'], pend_reply['max'], pend_reply['pending'])
        for message in detailed_reply:
            redis_stream.xack(STREAM_NAME, CONSUMER_GROUP, message['message_id'])
    return True
    
def parse_reply(reply):
    # List to store extracted values
    parsed_messages = []
    
    for message in reply[0][1]:
        message_id, message_data = message[0], message[1]
        
        if INBOUND_KEY.encode() in message_data:
            message_content = message_data[INBOUND_KEY.encode()]
            
            try:
                # Decode message content, replacing invalid characters with '_'
                decoded_content = message_content.decode('utf-8', errors='replace')
                # Replace the Unicode replacement character with '_'
                decoded_content = decoded_content.replace('�', '_')
                
                # Debugging: Print the full message content
                print(f"Decoded message content: {decoded_content}")
                
                # Regex pattern to match and extract values
                match = re.match(r"SDUID:(.*?)_TOPIC:(.*?)_DATA:(.*?)_", decoded_content)
                if match:
                    sduid, topic, data = match.groups()
                    parsed_messages.append((sduid, topic, data))
                else:
                    print(f"Message did not match expected format: {decoded_content}")
            
            except UnicodeDecodeError as e:
                print(f"Unicode decode error: {e}")
        
        # Acknowledge the message as processed
        redis_stream.xack(STREAM_NAME, CONSUMER_GROUP, message_id)
    
    # Process each extracted message
    for sduid, topic, data in parsed_messages:
        message_to_client(sduid, topic, data)
    """
    for message in reply[0][1]:
        if list(message[1].keys())[0] == INBOUND_KEY.encode():
            # Slice string to get components
            message_content = message[1][INBOUND_KEY.encode()]
            decoded_content = message_content.decode('utf-8')
            content_list = decoded_content.split('_')[:-1]
            sduid = content_list[0].split("SDUID:")[1]
            topic = content_list[1].split("TOPIC:")[1]
            data = content_list[2].split("DATA:")[1]
            #sduid = content_list[0][len("SDUID:") : ]
            #topic = content_list[1][len("TOPIC:") : ]
            #data = content_list[2][len("DATA:") : ]
            # Send message out to client side
            message_to_client(sduid, topic, data)
            
        # Ack message regardless of inbound/outbound
        redis_stream.xack(STREAM_NAME, CONSUMER_GROUP, message[0])
    """
def message_to_client(SDUID, topic, data):
    # Format information how client expects to receive it
    formatted_string = f'SDUID:{SDUID}\nTOPIC:{topic}\nDATA:{data}'
    socket.send(formatted_string)

def check_messages():
    reply = redis_stream.xreadgroup(CONSUMER_GROUP, CONSUMER_NAME, {STREAM_NAME:'>'})
    if(len(reply)):
        parse_reply(reply)

# Main route for sending HTML page to the client
@app.route('/')
def index():
    return render_template('index.html')

# POST method that deals with message sent by client
@app.route('/submit', methods=['POST'])
def submit():
    text1 = request.form['DUID']
    text2 = request.form['TOPIC']
    text3 = request.form['DATA']
    text4 = request.form['DUCK_TYPE']

    # Add the message to the Redis stream
    redis_stream.xadd(STREAM_NAME, {OUTBOUND_KEY:f'DUID:{text1}_TOPIC:{text2}_DATA:{text3}_DUCKTYPE:{text4}'})

    # Check for any new messages
    check_messages()

    return redirect(url_for('index'))

# Checks for message upon client side polling message
@socket.on('message')
def poll(message):
    if message == "polling":
        check_messages()

if __name__ == '__main__':
    if(redis_init()):    # Initialize redis, abort program if exceptions are raised
        socket.run(app, host=IP_ADDRESS, allow_unsafe_werkzeug=True)
    else:
        print("Redis error raised, program aborting")
