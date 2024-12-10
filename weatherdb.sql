--creates a fake database of weather data for accessing from the server

CREATE DATABASE weather_db;

USE weather_db;

CREATE TABLE weather(
        ID INT AUTO_INCREMENT PRIMARY KEY,
        city VARCHAR(50) NOT NULL,
        temperature VARCHAR(50) NOT NULL,
        humidity VARCHAR(50) NOT NULL,
        description VARCHAR(50) NOT NULL
);

INSERT INTO weather (city, temperature, description) VALUES
        ('Vancouver', '74', '60%', 'cloudy'),
        ('Victoria', '50', '55%', 'Sunny');
