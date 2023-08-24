import sqlite3
import pytz
from datetime import datetime
from flask import Flask, jsonify, request, current_app, g
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# Connect to the SQLite database
conn = sqlite3.connect('tasks.db')
cursor = conn.cursor()

# Create the tasks table if it doesn't exist
cursor.execute('''
    CREATE TABLE IF NOT EXISTS tasks (
        id INTEGER PRIMARY KEY,
        user_id INTEGER,
        time TEXT
    )
''')

conn.commit()

# Connect to the SQLite database
def get_db():
    if 'db' not in g:
        g.db = sqlite3.connect('tasks.db')
        g.db.row_factory = sqlite3.Row
    return g.db

# Close the database connection at the end of the request
@app.teardown_appcontext
def close_db(error):
    db = g.pop('db', None)
    if db is not None:
        db.close()

# Define a list of valid API keys (for demonstration purposes)
VALID_API_KEYS = ["your-api-key-1", "your-api-key-2"]

# Custom decorator for API key validation
def validate_api_key(api_key):
    return api_key in VALID_API_KEYS

# Convert UTC to Bangladesh Time
def utc_to_bangladesh_time(utc_time):
    utc = pytz.utc.localize(utc_time)
    bst = utc.astimezone(pytz.timezone("Asia/Dhaka"))
    return bst

# Endpoint to get all tasks for a specific user
@app.route('/tasks/<int:user_id>', methods=['GET'])
def get_tasks(user_id):
    api_key = request.headers.get("Authorization")

    if not validate_api_key(api_key):
        return jsonify({"error": "Unauthorized"}), 401
    
    db = get_db()
    cursor = db.cursor()
    cursor.execute('SELECT id, time FROM tasks WHERE user_id = ?', (user_id,))
    user_tasks = cursor.fetchall()

    tasks_with_bst = [{"id": task[0], "time": task[1]} for task in user_tasks]

    return jsonify({"tasks": tasks_with_bst})

# Endpoint to create a new task for a specific user
@app.route('/tasks/<int:user_id>', methods=['POST'])
def create_task(user_id):
    api_key = request.headers.get("Authorization")

    if not validate_api_key(api_key):
        return jsonify({"error": "Unauthorized"}), 401

    db = get_db()
    cursor = db.cursor()


    user_provided_time = request.json.get("time")
    if not user_provided_time:
        return jsonify({"error": "Time must be provided"}), 400

    try:
        user_time = datetime.strptime(user_provided_time, '%H:%M')
    except ValueError:
        return jsonify({"error": "Invalid time format"}), 400

    user_time_formatted = user_time.strftime('%H:%M')
    
    cursor.execute('INSERT INTO tasks (user_id, time) VALUES (?, ?)', (user_id, user_time_formatted))
    db.commit()

    return jsonify({"message": "Task created"}), 201

# Endpoint to update an existing task with user-provided time
@app.route('/tasks/<int:user_id>/<string:time_str>', methods=['PUT'])
def update_task(user_id, time_str):
    api_key = request.headers.get("Authorization")

    if not validate_api_key(api_key):
        return jsonify({"error": "Unauthorized"}), 401

    user_provided_time = request.json.get("time")
    if not user_provided_time:
        return jsonify({"error": "Time must be provided"}), 400

    try:
        user_time = datetime.strptime(user_provided_time, '%H:%M')
    except ValueError:
        return jsonify({"error": "Invalid time format"}), 400

    user_time_formatted = user_time.strftime('%H:%M')
    
    db = get_db()
    cursor = db.cursor()

    print(user_time_formatted, user_id, time_str)

    # update the task with the new time and check if it was successful
    cursor.execute('UPDATE tasks SET time = ? WHERE user_id = ? AND time = ?', (user_time_formatted, user_id, time_str))
    db.commit()
    if cursor.rowcount == 0:
        return jsonify({"error": "Task not found"}), 404

    return jsonify({"message": "Task updated"})

# Endpoint to delete a task
@app.route('/tasks/<int:user_id>/<string:time_str>', methods=['DELETE'])
def delete_task(user_id, time_str):
    api_key = request.headers.get("Authorization")

    if not validate_api_key(api_key):
        return jsonify({"error": "Unauthorized"}), 401

    db = get_db()
    cursor = db.cursor()

    cursor.execute('DELETE FROM tasks WHERE user_id = ? AND time = ?', (user_id, time_str))
    db.commit()

    return jsonify({"message": "Task deleted"})

# Endpoint to check if it is time to send a notification
@app.route('/tasks/check/<int:user_id>', methods=['GET'])
def check_tasks(user_id):
    api_key = request.headers.get("Authorization")

    if not validate_api_key(api_key):
        return jsonify({"error": "Unauthorized"}), 401

    db = get_db()
    cursor = db.cursor()

    cursor.execute('SELECT id, user_id, time FROM tasks WHERE user_id = ?', (user_id,))
    all_tasks = cursor.fetchall()

    # get current hour and minute in Bangladesh time
    current_time = utc_to_bangladesh_time(datetime.utcnow()).strftime('%H:%M')

    print("current_time:", current_time)

    dispense_now = False

    for task in all_tasks:
        task_time = task[2]
        # print("task_id:", task[0])
        # print("user_id:", task[1])
        # print("task_time:", task_time)
        if task_time == current_time:
            dispense_now = True
            break

    return jsonify({"dispense_now": dispense_now})


if __name__ == '__main__':
    app.run(debug=True)
