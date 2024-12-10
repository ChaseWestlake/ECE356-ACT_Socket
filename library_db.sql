--Creates the database, then adds various fake books to the library
CREATE DATABASE library_db;

USE library_db;

CREATE TABLE books (

        id INT PRIMARY KEY AUTO_INCREMENT,
        title VARCHAR(50) NOT NULL,
        author VARCHAR(50) NOT NULL,
        year INT NOT NULL
);

INSERT INTO books (title, author, year) VALUES
('Star wars 2', 'George Lucas', 1980),
('Lord of The rings', 'JRR Tolkien', 1948),
('Batman comic', 'Kevin Conroy', 2020),
('Star Wars 44', 'George Frank', 1981),
('American Gods', 'Neil Gaiman', 2001);
