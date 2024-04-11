const ws = new WebSocket(""); // use the same host port as esp do in the format ("ws://host:port")
var l_motor_p; // <p>
var r_motor_p;
var angle_p;
var image;
var server_s; // status
var cam_s;
var esp_s;
window.onload = () => {
  l_motor_p = document.getElementById("l_motor");
  r_motor_p = document.getElementById("r_motor");
  angle_p = document.getElementById("angle");
  image = document.getElementById("img");
  l_motor_p.innerHTML = "狀態列：左側馬達：0";
  r_motor_p.innerHTML = " | 右側馬達：0 ";
  angle_p.innerHTML = " | 車頭角度(正為右)：0";
};

ws.onmessage = (msg) => {
  tmp = "";
  console.log(msg);
  for (var i = 0; i < 3; i++) {
    tmp += msg[i];
  }
  if (msg.data.size > 3000) {
    var blob = new Blob([msg.data], { type: "image / jpeg" });
    var fr = new FileReader();
    fr.onload = () => {
      image.src = fr.result;
    };
    fr.readAsDataURL(blob);
  } else {
    console.log("unknown msg\n");
    console.log(msg);
  }
};

ws.onopen = () => {
  ws.send("web connected");
  console.log("websocket connected");
};

ws.onclose = () => {
  console.log("websocket disconnected");
};
let rpm = 150;

function stop() {
  let ret = "dc:0,0,0,0";
  try {
    ws.send(ret);
    console.log("sent " + ret);
  } catch (e) {
    console.log(e);
  }
}

function sendAct(op, val) {
  var l_motor, r_motor, angle; // type: (int | int | int) => range: [(-1,0,1) | (-1,0,1) | (-45~45)]
  if (op == "L") {
    // Left
    l_motor = 1;
    r_motor = 1;
    angle = -45;
  } else if (op == "R") {
    // Right
    l_motor = 1;
    r_motor = 1;
    angle = 45;
  } else if (op == "F") {
    // Forward
    l_motor = 1;
    r_motor = 1;
    angle = 0;
  } else if (op == "B") {
    // Backward
    l_motor = -1;
    r_motor = -1;
    angle = 0;
  } else if (op == "S") {
    // Speed
    rpm = val;
  } else {
    l_motor = 0;
    r_motor = 0;
  }
  let ret =
    "dc:" +
    l_motor.toString() +
    "," +
    r_motor.toString() +
    "," +
    angle.toString() +
    "," +
    rpm.toString();
  try {
    ws.send(ret);
    console.log("sent " + ret);
  } catch (e) {
    console.log(e);
  }
}
