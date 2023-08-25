var SERVER_URL = "http://localhost:5000";

// get current url
var current_url = window.location.href;
var pathname = window.location.pathname;

console.log(current_url);
console.log(pathname);

// get user id from local storage. if user_id is null, redirect to login page. base64 decode user_id.
var user_id = localStorage.getItem("user_id");
user_id = atob(user_id);

// get password from local storage. if password is null, redirect to login page. base64 decode password.
var password = localStorage.getItem("auth");
password = atob(password);

async function startup_tasks() {
    // if current url pathname doesn't start with /login and check if user is logged in. if not, redirect to login page.
    if (!pathname.startsWith("/login")) {

        if (password == null || user_id == null) {
            window.location.href = "login";
        } else {
            // check if user is logged in
            var login_status = await login(user_id, password);
            if (login_status != "true") {
                window.location.href = "login";
            }
        }
    }

    if (pathname == "/") {
        fetchTasks(password, user_id);
    }
}

startup_tasks();

// function to create task
function fetchTasks(password, user_id) {
    const headers = new Headers({
        'Authorization': password
    });

    fetch(`${SERVER_URL}/tasks/${user_id}`, {
        method: 'GET',
        headers: headers
    }) // Change the URL to your server endpoint
        .then(response => response.json())
        .then(data => {
            const taskList = document.getElementById('taskList');
            taskList.innerHTML = ''; // Clear existing tasks
            data.tasks.forEach(task => {
                const listItem = document.createElement('li');
                listItem.className = 'task-list-item';
                listItem.innerHTML = `<h4>${convertTime24to12(task.time)}</h4> <button class="btn" onclick="editTask(this)"><i class="material-icons left">edit</i>Edit</button> <button class="btn red" onclick="deleteTask(this)"><i class="material-icons left">delete</i>Delete</button>`;
                taskList.appendChild(listItem);
            });
        })
        .catch(error => console.error('Error fetching tasks:', error));
}

// function to create task
function createTask(password, user_id, time) {
    const data = {
        time: time
    };

    fetch(`${SERVER_URL}/tasks/${user_id}`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': password
        },
        body: JSON.stringify(data),
    }).then(response => {
        return response.json();
    }).then(data => {
        console.log(data);
    }).catch(err => {
        console.log(err);
    });
}

// function to edit task
function editTaskNetwork(password, user_id, prev_time, new_time) {
    const data = {
        time: new_time
    };

    fetch(`${SERVER_URL}/tasks/${user_id}/${prev_time}`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': password
        },
        body: JSON.stringify(data),
    }).then(response => {
        return response.json();
    }).then(data => {
        console.log(data);
    }).catch(err => {
        console.log(err);
    });
}

// function to delete task
function deleteTaskNetwork(password, user_id, time) {
    fetch(`${SERVER_URL}/tasks/${user_id}/${time}`, {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': password
        }
    }).then(response => {
        return response.json();
    }).then(data => {
        console.log(data);
    }).catch(err => {
        console.log(err);
    });
}

function convertTime12to24(time12h) {
    var [time, modifier] = time12h.split(' ');

    var [hours, minutes] = time.split(':');

    if (hours === '12') {
        hours = '00';
    }

    if (modifier === 'PM') {
        hours = (parseInt(hours, 10) + 12).toString();
    }

    hours = hours.padStart(2, '0'); // Add preceding zero if needed
    minutes = minutes.padStart(2, '0'); // Add preceding zero if needed

    return `${hours}:${minutes}`;
}

function convertTime24to12(time24h) {
    var [hours, minutes] = time24h.split(':');

    var modifier = hours >= 12 ? 'PM' : 'AM';

    hours = (hours % 12 || 12).toString().padStart(2, '0'); // Adjust hours
    minutes = minutes.padStart(2, '0'); // Add preceding zero if needed

    return `${hours}:${minutes} ${modifier}`;
}

async function login(user_id, password) {
    const response = await fetch('/authenticate', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            user_id: user_id,
            password: password
        })
    });

    console.log(response);

    if (response.ok) {
        const data = await response.json();
        return "true"
    } else {
        const errorData = await response.json();
        return "Error: " + errorData.error
    }
}
