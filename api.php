<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");
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

// Handle OPTIONS request for CORS
if ($_SERVER["REQUEST_METHOD"] == "OPTIONS") {
    http_response_code(200);
    exit();
}

// Get the JSON input
$inputJSON = file_get_contents("php://input");
file_put_contents("log.txt", "Received: " . $inputJSON . "\n", FILE_APPEND);

$data = json_decode($inputJSON, true);

// Validate JSON input
if (isset($data["name"]) && isset($data["value"])) {
    $name = $conn->real_escape_string($data["name"]);
    $value = floatval($data["value"]);

    // Insert into database
    $sql = "INSERT INTO sensordata (name, value) VALUES ('$name', '$value')";

    if ($conn->query($sql) === TRUE) {
        echo json_encode(["status" => "success", "message" => "Data added successfully"]);
    } else {
        file_put_contents("log.txt", "Database error: " . $conn->error . "\n", FILE_APPEND);
        echo json_encode(["status" => "error", "message" => "Database error: " . $conn->error]);
    }
} else {
    echo json_encode(["status" => "error", "message" => "Invalid input format"]);
}

$conn->close();
?>
