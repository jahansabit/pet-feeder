var SERVER_URL = "http://localhost:5000";

// get url
var url = new URL(window.location.href);

// get api key from url
var api_key = url.searchParams.get("api_key");

// get user id from url
var user_id = url.searchParams.get("user_id");


// function to create task
function fetchTasks(api_key, user_id) {
    const headers = new Headers({
        'Authorization': api_key
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

fetchTasks(api_key, user_id);

// function to create task
function createTask(api_key, user_id, time) {
    const data = {
        time: time
    };

    fetch(`${SERVER_URL}/tasks/${user_id}`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': api_key
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
function editTaskNetwork(api_key, user_id, prev_time, new_time) {
    const data = {
        time: new_time
    };

    fetch(`${SERVER_URL}/tasks/${user_id}/${prev_time}`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': api_key
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
function deleteTaskNetwork(api_key, user_id, time) {
    fetch(`${SERVER_URL}/tasks/${user_id}/${time}`, {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': api_key
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
