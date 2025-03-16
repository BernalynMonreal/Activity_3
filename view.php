<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET");
header("Content-Type: application/json; charset=UTF-8");

// Database credentials
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "sensor";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die(json_encode(["status" => "error", "message" => "Database connection failed: " . $conn->connect_error]));
}

// Fetch sensor data
$sql = "SELECT * FROM sensordata ORDER BY id DESC";
$result = $conn->query($sql);

$records = [];

if ($result) {
    while ($row = $result->fetch_assoc()) {
        $records[] = $row;
    }
    if (!empty($records)) {
        echo json_encode(["status" => "success", "data" => $records]);
    } else {
        echo json_encode(["status" => "error", "message" => "No records found"]);
    }
} else {
    echo json_encode(["status" => "error", "message" => "Database query failed: " . $conn->error]);
}

// Close connection
$conn->close();
?>