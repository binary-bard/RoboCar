from flask import Flask, flash, redirect, render_template, request, Response, session
from subprocess import Popen, PIPE
import arduino_mode

app = Flask(__name__, template_folder='../templates')

@app.route("/")
def hello():
	return render_template('setMode.html')

# set the secret key.  keep this really secret:
app.secret_key = b'\x80\xc9\xd5\xb2?\xfeeN\xca#h\x8d\x1c\x01jE}\xb7\nK\xa4\xfd-\xf0'

@app.route("/SetMode", methods=['GET', 'POST'])
def setModes():
  action  = request.values.get("Submit")
  if(action == 'Select'):
    #mode = request.args.get('mode')
    mode = request.form.get('mode')
    session['mode'] = mode
    if mode == "train":
      arduino_mode.set_mode(0)
    elif mode == "steer":
      arduino_mode.set_mode(1)
    elif mode == "test":
      arduino_mode.set_mode(2)
    elif mode == "run":
      arduino_mode.set_mode(3)
    else:
      print('No mode set, setting it to train')
      arduino_mode.set_mode(0)

  elif(action == 'Start'):
    arduino_mode.start_motors()
  elif(action == 'Stop'):
    arduino_mode.stop_motors()
  else:
    print('No appropriate action')

  return redirect('/')
  #return Response(action, mimetype='text/html')


@app.route("/runInitScripts")
def runScripts():
  dirpath  = request.values.get("dir")
  print(dirpath)
  def inner():
    cmd = ["scripts/test.sh"]
    proc = Popen(cmd,stdout=PIPE,stderr=PIPE,stdin=PIPE)
    for line in iter(proc.stdout.readline,''):
      yield line.rstrip() + '<br/>\n'

  return Response(inner(), mimetype='text/html')


@app.route("/train")
def train():
  def inner():
    proc = Popen(
      ["processes/train.py"],             #call something with a lot of output so we can see it
      shell=True,
      stdout=PIPE
    )

    for line in iter(proc.stdout.readline,''):
      yield line.rstrip() + '<br/>\n'

  return Response(inner(), mimetype='text/html')  # text/html is required for most browsers to show th$


if __name__ == "__main__":
  app.run(host='0.0.0.0', port=8080)

