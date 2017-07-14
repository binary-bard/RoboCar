from flask import Flask, flash, redirect, render_template, request, Response, session
from subprocess import Popen, PIPE
#from scripts import arduino_mode

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
    print(mode)

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
  #arduino_mode.setup()
  app.run(host='0.0.0.0', port=8080)
