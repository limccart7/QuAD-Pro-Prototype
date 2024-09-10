sudo apt install python3-pip

sudo rm README.md
sudo rm -r Archive

cd webapp
python3 -m venv web_py_env
source web_py_env/bin/activate
pip install redis
pip install flask
pip install flask-socketio