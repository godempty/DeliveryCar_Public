import asyncio
import websockets
from io import BytesIO
CamWS=None
WebWS=None
from PIL import Image, UnidentifiedImageError
async def main():
    global server
    server = await websockets.serve(handle_connection, '0.0.0.0', 3001) #change port here
    await server.wait_closed()

def is_valid_image(image_bytes):
    try:
        Image.open(BytesIO(image_bytes))
        # print("image OK")
        return True
    except UnidentifiedImageError:
        print("image invalid")
        return False

async def handle_connection(websocket, path):
    global WebWS
    global CamWS
    while True:
        try:
            message = await websocket.recv()
            if len(message) > 3000:
                  if is_valid_image(message):
                        CamWS = websocket
                        if WebWS!=None and list(server.websockets).__contains__(WebWS):
                            await WebWS.send(message)
            elif message[0:3]=='dc:':
                WebWS = websocket
                candidates = list(server.websockets)
                if WebWS!=None and candidates.__contains__(WebWS):
                    candidates.remove(WebWS)
                if CamWS != None and candidates.__contains__(CamWS):
                    candidates.remove(CamWS)
                print(f'broadcast to', candidates)
                websockets.broadcast(candidates,message)
            else:
                print(message)
                    
                    
        except websockets.exceptions.ConnectionClosed:
            break


asyncio.run(main())