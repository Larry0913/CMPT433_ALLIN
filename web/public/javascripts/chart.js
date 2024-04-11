"use strict";

// Generate random data for testing
const weeklyData = Array.from({ length: 7 }, () => Math.floor(Math.random() * 100));
const hourlyData = Array.from({ length: 24 }, () => Math.floor(Math.random() * 100));

// Get the context of the canvas elements we want to select
var weeklyCtx = document.getElementById("weekly-chart").getContext("2d");
var hourlyCtx = document.getElementById("hourly-chart").getContext("2d");

// Create the charts
var weeklyChart = new Chart(weeklyCtx, {
    type: 'line',
    data: {
        labels: ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'],
        datasets: [{
            label: 'Weekly Average Count',
            data: weeklyData,
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            yAxes: [{
                ticks: {
                    beginAtZero: true
                }
            }]
        },
        onHover: function(event, elements) {
            if (elements.length > 0) {
                this.canvas.style.cursor = 'pointer';
            } else {
                this.canvas.style.cursor = 'default';
            }
        },
        onClick: function(event, elements) {
            if (elements.length > 0) {
                var day = this.data.labels[elements[0].index];
                showHourlyChart(day);
            }
        }
    }
});

var hourlyChart = new Chart(hourlyCtx, {
    type: 'line',
    data: {
        labels: Array.from({ length: 24 }, (_, i) => i + ':00'),
        datasets: [{
            label: 'Hourly Average Count',
            data: hourlyData,
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            yAxes: [{
                ticks: {
                    beginAtZero: true
                }
            }]
        }
    }
});

// Function to show the hourly chart for a specific day
function showHourlyChart(day) {
    // Update the title of the hourly chart
    document.getElementById('hourly-title').textContent = 'Hourly Average Count (' + day + ')';

    // Hide the weekly chart and show the hourly chart
    hourlyChart.canvas.style.display = 'block';
}

// Show the weekly chart by default
weeklyChart.canvas.style.display = 'block';
hourlyChart.canvas.style.display = 'none';

document.getElementById('week-picker').addEventListener('change', function() {
    // Generate new random data
    var newData = Array.from({ length: 7 }, () => Math.floor(Math.random() * 100));

    // Update the chart data
    weeklyChart.data.datasets[0].data = newData;
    weeklyChart.update();
});
// Get the current date
var currentDate = new Date();

// Get the current week number
var currentWeekNumber = getWeekNumber(currentDate);

// Set the value of the <input type="week"> element
document.getElementById('week-picker').value = currentWeekNumber;

function getWeekNumber(d) {
    // Copy date so don't modify original
    d = new Date(Date.UTC(d.getFullYear(), d.getMonth(), d.getDate()));
    // Set to nearest Thursday: current date + 4 - current day number
    // Make Sunday's day number 7
    d.setUTCDate(d.getUTCDate() + 4 - (d.getUTCDay() || 7));
    // Get first day of year
    var yearStart = new Date(Date.UTC(d.getUTCFullYear(), 0, 1));
    // Calculate full weeks to nearest Thursday
    var weekNo = Math.ceil((((d - yearStart) / 86400000) + 1) / 7);
    // Return array of year and week number
    return d.getUTCFullYear() + '-W' + String(weekNo).padStart(2, '0');
}
