# DeliveryCar_Public
### Description
My friend and I aim to make a remote control car with camera. Due to budget limit, we use esp32s and esp32-cam as our car's control system. This project is updating, so there could still be problems.
### this repo include?
1. controlling web code(flask)
2. In car board code(esp32s)
3. Car camera code(esp32-cam)
4. websocket server
### How to use
1. Change Wifi setting in both code for board(.ino file) and burn the code in your board.
2. Get your localIP as your websocket host and use a port as you like. (There could be other alternatives but I don't know) 
3. Change the websocket setting in "InCarBoard,CarCam,web.py".
4. Install the necessary packages by use "pip install pillow websockets flask asyncio"
5. Run server by use "py server.py"
6. Run web by use "py web.py"
7. Refresh the web and click any button to register you to server. 
