"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
var timeoutUDP;

$(document).ready(function() {

	window.setInterval(function() {updateTime()},1000);

	$('#base').click(function(){
		sendCommandViaUDP("play base\n");
	});
	$('#snare').click(function(){
		sendCommandViaUDP("play snare\n");
	});
	$('#hihat').click(function(){
		sendCommandViaUDP("play hiHat\n");
	});


	$('#modeRock').click(function(){
		sendCommandViaUDP("play rock\n");
	});
	$('#modeNone').click(function(){
		sendCommandViaUDP("play none\n");
	});
	$('#modeCustom').click(function(){
		sendCommandViaUDP("play customize\n");
	});
	
	$('#volumeDown').click(function(){
		sendCommandViaUDP("volume decrease\n");
	});
	$('#volumeUp').click(function(){
		sendCommandViaUDP("volume increases\n");
	});
	$('#tempoDown').click(function(){
		sendCommandViaUDP("tempo decrease\n");
	});
	$('#tempoUp').click(function(){
		sendCommandViaUDP("tempo increase\n");
	});
	$('#terminate').click(function(){
		sendCommandViaUDP("stop\n");
	});

	socket.on('commandReply', function(result) {
		hideError(); 
		clearTimeout(timeoutUDP);
		result = result.replace(/[^ -~]+/g, "").trim();
		console.log(result);
		if (result.startsWith('bbg_allin')) {
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

function updateStats(data, commandType) {
    // Update the mode
    const modeSpan = document.getElementById('mode');
    // Update the volume
    const volumeText = document.getElementById('volumeText');
    // Update the tempo
    const tempoText = document.getElementById('tempoText'); // Fixed typo here from 'temppoText' to 'tempoText'

    if (commandType === 'status') {
        // Mapping mode numbers to mode names, assuming mode 0=None, 1=Rock, 2=Custom
        const modeNames = {
            0: 'None',
            1: 'Rock',
            2: 'Custom'
        };

        if (modeSpan && data.mode !== undefined) {
            modeSpan.textContent = modeNames[data.mode] || 'Unknown';
        }

        if (volumeText && data.volume !== undefined) {
            volumeText.value = data.volume.toString();
        }

        if (tempoText && data.bpm !== undefined) {
            tempoText.value = data.bpm.toString();
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
function updateUptime(uptime_seconds) {
	let hours = Math.floor(uptime_seconds / 3600);
    let minutes = Math.floor((uptime_seconds % 3600) / 60);
	let seconds = Math.round(uptime_seconds % 60); 

    // Format the uptime string
    let uptimeString = `Device up for: ${hours}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')} (H:M:S)`;

    // Update the status element
    const statusDiv = document.getElementById('status');
    if (statusDiv) {
        statusDiv.textContent = uptimeString;
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