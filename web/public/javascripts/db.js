// db.js
let db;

function initDB() {
    const request = indexedDB.open("PeopleCountDB", 2);

    request.onerror = function(event) {
        console.error("Database error: ", event.target.errorCode);
    };

    request.onupgradeneeded = function(event) {
        const db = event.target.result;
        if (!db.objectStoreNames.contains('countData')) {
            const store = db.createObjectStore("countData", { keyPath: "id", autoIncrement: true });
            store.createIndex("totalcount", "totalcount", { unique: false });
            store.createIndex("lastcount", "lastcount", { unique: false });
        }
    };

    request.onsuccess = function(event) {
        db = event.target.result;
        console.log("Database initialized successfully");
        readInitialCount(); // 读取初始计数
        // resetTotalCount(); // 这里移动了调用位置
    };
}
function resetTotalCount() {
    const transaction = db.transaction(["countData"], "readwrite");
    const store = transaction.objectStore("countData");
    const request = store.get(1);

    request.onsuccess = function(event) {
        const data = event.target.result;
        if (data) {
            data.totalcount = 0;
            store.put(data).onsuccess = function() {
                console.log('Total count has been reset to 0 at initial load.');
                updateTotalCountDisplay(0); // 更新页面显示为 0
            };
        } else {
            console.log("No previous count found, initializing...");
            store.add({ id: 1, totalcount: 0, lastcount: 0 });
            updateTotalCountDisplay(0);
        }
    };

    request.onerror = function(event) {
        console.error("Error accessing the data to reset:", event.target.errorCode);
    };
}
function updateTotalCountDisplay(totalcount) {
    let totalCountElement = document.getElementById('count');
    totalCountElement.textContent = totalcount;
}
function readInitialCount() {
    // Opening the transaction with "readwrite" access
    const transaction = db.transaction(["countData"], "readwrite");
    const store = transaction.objectStore("countData");
    const request = store.get(1); 

    request.onsuccess = function(event) {
        const data = event.target.result;
        if (data) {
            updateTotalCountDisplay(data.totalcount);
        } else {
            console.log("No previous count found, initializing...");
            const initialData = {
                id: 1,
                totalcount: 0,
                lastcount: 0,
            };
            store.add(initialData); // Now this operation is allowed
            updateTotalCountDisplay(0);
        }
    };

    request.onerror = function(event) {
        console.error("Error reading initial count: ", event.target.errorCode);
    };
}
function readTotalCount(callback) {
    const transaction = db.transaction(["countData"], "readonly");
    const store = transaction.objectStore("countData");
    const request = store.get(1); // 假设我们用 ID 1 存储计数数据

    request.onsuccess = function(event) {
        const data = event.target.result;
        if (data && data.totalcount !== undefined) {
            callback(data.totalcount); // 使用回调函数传递 totalcount
        } else {
            console.error("Total count not found in the database.");
            callback(0); // 传递一个默认值或进行错误处理
        }
    };

    request.onerror = function(event) {
        console.error("Error reading total count from database:", event.target.errorCode);
        callback(0); // 在错误情况下传递一个默认值或进行错误处理
    };
}
document.addEventListener('DOMContentLoaded', function() {
    initDB(); // Initialize the database when the page loads

});

// function addOrUpdateCount(newCount) {
//     const transaction = db.transaction(["counts"], "readwrite");
//     const store = transaction.objectStore("counts");
//     const index = store.index("time"); // 假设你用时间戳作为索引
//     const now = Date.now();

//     index.openCursor(null, 'prev').onsuccess = function(event) {
//         const cursor = event.target.result;
//         if (cursor) {
//             // 存在上一个记录，不是第一次访问
//             const lastRecord = cursor.value;
//             const updatedCount = lastRecord.lastCount + (newCount - lastRecord.lastCount);
//             const newRecord = { id: cursor.key, time: now, lastCount: updatedCount, isFirstVisit: false };
//             store.put(newRecord);
//         } else {
//             // 第一次访问
//             const firstRecord = { time: now, lastCount: newCount, isFirstVisit: true };
//             store.add(firstRecord);
//         }
//     };

//     transaction.oncomplete = function() {
//         console.log("Operation completed.");
//     };

//     transaction.onerror = function(event) {
//         console.error("Error adding/updating count: ", event.target.errorCode);
//     };
// }
// function readCounts() {
//     const transaction = db.transaction(["counts"]);
//     const store = transaction.objectStore("counts");
//     const request = store.openCursor();
//     const counts = [];

//     request.onsuccess = function(event) {
//         const cursor = event.target.result;
//         if (cursor) {
//             counts.push(cursor.value);
//             cursor.continue();
//         } else {
//             console.log("All counts retrieved: ", counts);
//         }
//     };

//     request.onerror = function(event) {
//         console.error("Error reading counts: ", event.target.errorCode);
//     };
// }

// Export the readCounts function if using modules
// export { readCounts };
