import json
import os

from flask import Flask, request, render_template, send_from_directory, jsonify, session
from flask_session import Session

SHEEP_STATE_FILE = 'static/sheep_state.json'

app = Flask(__name__)
SESSION_TYPE = 'filesystem'
app.config.from_object(__name__)
Session(app)


# http://192.168.0.112/sheep?speed=<speed>&dir=<dir>
#
# where
#  <speed> : -100 to 100
#  <dir>   : |speed| + |dir| < 100
#
# or
# http://192.168.0.112/motor?m1=<speedM1>&m2=<speedM2>
#


def get_sheep() -> dict:

    # with open(SHEEP_STATE_FILE) as json_file:
    #     try:
    #         sheep_state = json.load(json_file)
    #     except json.decoder.JSONDecodeError as e:
    #         print("Error decoding " + SHEEP_STATE_FILE)
    #         for line in json_file:
    #             print(line)
    #         print(e)
    #         return {"m1": 0, "m2": 0}

    try:
        sheep_state = session["sheep_state"]
    except KeyError:
        sheep_state = {"m1": 0, "m2": 0}
    return sheep_state


def get_m1() -> str:
    sheep_state = get_sheep()
    return sheep_state['m1']


def get_m2():
    sheep_state = get_sheep()
    return sheep_state['m2']


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                               'favicon.ico', mimetype='image/vnd.microsoft.icon')


@app.route("/", methods=['GET'])
def index():
    session["sheep_state"] = {"m1": 0, "m2": 0}
    return render_template('index.html')


@app.route("/motor", methods=['GET'])
def motor():
    sheep_state = get_sheep()
    sheep_state['m1'] = request.args.get('m1')
    sheep_state['m2'] = request.args.get('m2')
    with open(SHEEP_STATE_FILE, 'w') as outfile:
        json.dump(sheep_state, outfile)
    return


@app.route("/sheep/state", methods=['GET'])  # decorator
def get_sheep_state():
    return jsonify(get_sheep())


@app.route("/sheep/move", methods=['GET'])  # decorator
def sheep_move():
    cmdSpeed = int(request.args.get('speed'))
    cmdDir = int(request.args.get('dir'))
    sheep_state = get_sheep()
    sheep_state['m1'] = cmdSpeed + cmdDir
    sheep_state['m2'] = cmdSpeed - cmdDir
    with open(SHEEP_STATE_FILE, 'w') as outfile:
        json.dump(sheep_state, outfile)
    return jsonify(get_sheep())


if __name__ == '__main__':
    app.debug = True
    app.run()
