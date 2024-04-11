from flask import Flask, render_template
app = Flask(__name__)

@app.route('/')
def index():
    return render_template("./main.html")
app.run(host='0.0.0.0', debug=True  , threaded=True)