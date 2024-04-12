"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
var timeoutUDP;

$(document).ready(function() {

	window.setInterval(function() {updateTime()},1000);

	
	$('#terminate').click(function(){
		sendCommandViaUDP("shutdown\n");
	});


	socket.on('commandReply', function(result) {
		hideError(); 
		clearTimeout(timeoutUDP);
		result = result.replace(/[^ -~]+/g, "").trim();
		console.log(result);
		if (result.startsWith('BBG_ALLIN')) {
			const parts = result.split(' ');
			const commandType = parts[1];
			const data = {};
	
			// Loop over each part starting from the third part
			parts.slice(2).forEach(part => {
				// Split each part into key and value using '=' as a delimiter
				const [key, value] = part.split('=');
				// Convert the value to a correct type and add it to the data object
				if (key && value) {
					data[key] = isNaN(value) ? value : parseFloat(value);
				}
			});
	
			
			updateStats(data, commandType);


		} else {
			console.log('Invalid message format');
		}
	});

	socket.on('error', function(errorMsg) {
		// Display the error message to the user
		console.log(errorMsg);
		showError(errorMsg);
	});


});
document.addEventListener('DOMContentLoaded', function() {
    const increaseButton = document.getElementById('volumeUp');

    increaseButton.addEventListener('click', function() {
        sendCommandViaUDP("volume increase\n");
        console.log("Increase volume command sent.");
    });
});
document.addEventListener('DOMContentLoaded', function() {
    const decreaseButton = document.getElementById('volumeDown');

    decreaseButton.addEventListener('click', function() {
        sendCommandViaUDP("volume decrease\n");
        console.log("decrease volume command sent.");
    });
});

document.addEventListener('DOMContentLoaded', function() {
    const radioButtons = document.querySelectorAll('[name="displayMode"]');

    radioButtons.forEach(button => {
        button.addEventListener('click', function() {
            // 根据按钮的 value 发送相应的命令
            switch(button.value) {
                case '0':
                    sendCommandViaUDP("ppl\n");
                    console.log("Set mode to People Number");
                    break;
                case '1':
                    sendCommandViaUDP("temp\n");
                    console.log("Set mode to Temperature");
                    break;
                case '2':
                    sendCommandViaUDP("smile\n");
                    console.log("Set mode to Smile");
                    break;
                default:
                    console.error("Unknown mode");
                    break;
            }
        });
    });
});

function hideError() {
    $('#error-box').hide();
}

function showError(errorMessage) {
    $('#error-text').text(errorMessage);
    $('#error-box').show();

    // Clear any existing timeout to hide the error box to avoid conflicts
    clearTimeout(window.errorHideTimeout);

    // Set a timeout to hide the error box after 10 seconds
    window.errorHideTimeout = setTimeout(function() {
        $('#error-box').hide();
    }, 5000); // 10 seconds
}

function updateTime(){
	sendCommandViaUDP("status\n");
}
function updatePageCount(data) {
    // 更新页面元素
    const pplElement = document.getElementById('room-count-value');
    pplElement.textContent = data.totalcount;
}
function updateTotalCountDisplay(totalcount) {
    const totalCountElement = document.getElementById('count');
    totalCountElement.textContent = totalcount;
}
function updateCounts(data) {
    const transaction = db.transaction(["countData"], "readwrite");
    const store = transaction.objectStore("countData");
    const request = store.get(1); // 
    request.onsuccess = function(event) {
        const countData = event.target.result;
		console.log(countData);

        if (countData) {
            if (data.ppl > countData.lastcount) {
                countData.totalcount += 1; // 增加总计数
            }
            countData.lastcount = data.ppl; // 更新上一次的计数
            store.put(countData); // 保存更新
        }
    };

    request.onerror = function(event) {
        console.error("Error updating counts: ", event.target.errorCode);
    };
}

function updateStats(data, commandType) {

    if (commandType === 'status') {
		console.log(data);
		// Update status based on mode
        const modeMapping = {
			0: 'ppl-display',
			1: 'temperature-display',
			2: 'smile-display'
		};
	
		if (data.mode !== undefined) {
			const modePart = data.mode.match(/\d+/g);
			if (modePart) {	
				let oneDigit = modePart.join('').substring(0, 1);
				oneDigit = parseInt(oneDigit, 10);
				const selectedDisplay = modeMapping[oneDigit];
				console.log(selectedDisplay)
				if (selectedDisplay) {
					document.getElementById(selectedDisplay).checked = true;
				}

			}

			
		}

        // Update temperature value, round to two decimal places
		const temperatureElement = document.getElementById('temperature-value');
        if (data.temp !== undefined) {
            temperatureElement.textContent = parseFloat(data.temp).toFixed(2) + " °C";
        }
		
		const volumeText = document.getElementById('volumeText');
		if (data.volume !== undefined) {
			const numericPart = data.volume.match(/\d+/g);
			if (numericPart) {	
				let firstThreeDigits = numericPart.join('').substring(0, 3);
				firstThreeDigits = parseInt(firstThreeDigits, 10);
				volumeText.value = firstThreeDigits;
			}
		}
		
		// Update people count
		const pplElement = document.getElementById('room-count-value');

		if (data.ppl !== undefined) {
			
			updateCounts({ ppl: data.ppl });
			pplElement.textContent = data.ppl;
			readTotalCount(function(totalcount) {
				document.getElementById('count').textContent = totalcount;
			});		
		}

        // Calculate and update uptime
        if (data.uptime !== undefined) {
            updateUptime(data.uptime);
        }
    } else if (commandType === 'stop') {
        // Reset the UI to indicate the BeatBox has been stopped or set to defaults
        if (modeSpan) modeSpan.textContent = 'None';
        if (volumeText) volumeText.value = '---'; // Or set to a default value
        if (tempoText) tempoText.value = '---'; // Or set to a default value

        // Optionally clear the uptime or set a stopped message
        const statusDiv = document.getElementById('status');
        if (statusDiv) {
            statusDiv.textContent = "BeatBox stopped.";
        }
		clearTimeout(timeoutUDP);

    }
}
function updateUptime(uptimeString) {
    // Remove any non-numeric characters (except decimal point and negative sign)
    uptimeString = uptimeString.replace(/[^\d.-]/g, '');

    // Convert to a number
    let uptime_seconds = parseFloat(uptimeString);

    // Check if the conversion was successful
    if (isNaN(uptime_seconds)) {
        console.error('Invalid uptime value:', uptimeString);
        return; // Exit the function if conversion fails
    }

    let hours = Math.floor(uptime_seconds / 3600);
    let minutes = Math.floor((uptime_seconds % 3600) / 60);
    let seconds = Math.round(uptime_seconds % 60);

    // Format the uptime string
    let formattedUptime = `Device up for: ${hours}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')} (H:M:S)`;

    // Update the status element
    const statusDiv = document.getElementById('board-status');
    if (statusDiv) {
        statusDiv.textContent = formattedUptime;
    }
}

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
	 // Clear any existing command reply timeout to avoid conflicts
	 clearTimeout(timeoutUDP);

	 // Set a new timeout for the command reply
	 timeoutUDP = setTimeout(function() {
		 showError('NodeJS server or BeatBox C/C++ application is not responding.');
	 }, 1000); // 1 second
};