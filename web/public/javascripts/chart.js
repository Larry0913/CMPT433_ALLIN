"use strict";

// Generate random data for testing
// 更新初始数据生成，以适应逻辑上的限制
const weeklyData = Array.from({ length: 7 }, () => Math.floor( 50 + Math.random() * 100));

// 更新小时数据，确保小时数据的总和理论上不会超过日数据的最大值
const maxHourlyTotal = 100 / 3;  // 假设每日总数最高100，平均分给三个小时
const hourlyData = Array.from({ length: 24 }, () => Math.floor(Math.random() * maxHourlyTotal));

// 更新分钟数据，确保分钟数据的总和理论上不会超过小时数据的最大值
const maxMinuteTotal = maxHourlyTotal / 4;  // 假设每小时总数最高33.33，平均分给四个十五分钟
const minuteData = Array.from({ length: 60 }, () => Math.floor(Math.random() * maxMinuteTotal));


// Get the context of the canvas elements we want to select
var weeklyCtx = document.getElementById("weekly-chart").getContext("2d");
var hourlyCtx = document.getElementById("hourly-chart").getContext("2d");
var minuteCtx = document.getElementById("minute-chart").getContext("2d");
function generateRandomData(length, maxTotal) {
    let array = new Array(length).fill(0);
    let total = 0;
    for (let i = 0; i < length; i++) {
        let max = Math.floor((maxTotal - total) / (length - i));
        let value = Math.floor(Math.random() * (max + 1));
        array[i] = value;
        total += value;
    }
    return array;
}


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
                var dayIndex = elements[0].index;
                var day = this.data.labels[dayIndex];
                var dayValue = this.data.datasets[0].data[dayIndex]; // 获取该天的计数
        
                // 生成符合条件的小时数据
                var hourlyData = generateRandomData(24, dayValue);
                hourlyChart.data.datasets[0].data = hourlyData;
                hourlyChart.update();
        
                showHourlyChart(day);
                document.getElementById('minute-average').style.display = 'none'; // Show the minute chart

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
            backgroundColor: 'rgba(54, 162, 235, 0.2)', // Updated to light blue
            borderColor: 'rgba(54, 162, 235, 1)', // Updated to solid blue
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
                var hourIndex = elements[0].index;
                var hour = this.data.labels[hourIndex];
                var hourValue = this.data.datasets[0].data[hourIndex]; // 获取该小时的计数
        
                // 生成符合条件的分钟数据
                var minuteData = generateRandomData(60, hourValue);
                minuteChart.data.datasets[0].data = minuteData;
                minuteChart.update();
        
                showMinuteChart(hour);
            }
        }
        
    }
});

var minuteChart = new Chart(minuteCtx, {
    type: 'line',
    data: {
        labels: Array.from({ length: 60 }, (_, i) => `${i} min`),
        datasets: [{
            label: 'Minute Max Count',
            data: minuteData,
            backgroundColor: 'rgba(153, 102, 255, 0.2)',
            borderColor: 'rgba(153, 102, 255, 1)',
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
function showMinuteChart(hour) {
    document.getElementById('minute-average').style.display = 'block'; // Show the minute chart
    document.getElementById('minute-title').textContent = 'Minute Max Count (' + hour + ' Hour)';
}

// Show the weekly chart by default
weeklyChart.canvas.style.display = 'block';
hourlyChart.canvas.style.display = 'none';


document.getElementById('week-picker').addEventListener('change', function() {
    // Generate new random data
    var newWeeklyData = generateRandomData(50, 200); // 假设一周的总数不超过100
    weeklyChart.data.datasets[0].data = newWeeklyData;
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
