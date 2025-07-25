CREATE DATABASE IF NOT EXISTS kadai;

USE kadai;

DROP TABLE IF EXISTS scores;

DROP TABLE IF EXISTS student;

DROP TABLE IF EXISTS course;

DROP TABLE IF EXISTS subject;

DROP TABLE IF EXISTS place;

DROP TABLE IF EXISTS teacher;

DROP TABLE IF EXISTS gender;

DROP TABLE IF EXISTS result;

CREATE TABLE
    result (
        id INT NOT NULL PRIMARY KEY,
        max INT,
        min INT,
        result VARCHAR(2) NOT NULL
    );

CREATE TABLE
    gender (id INT NOT NULL PRIMARY KEY, gender CHAR(4));

CREATE TABLE
    teacher (
        teacher_id CHAR(4) NOT NULL PRIMARY KEY,
        name VARCHAR(40) NOT NULL
    );

CREATE TABLE
    place (
        id INT NOT NULL PRIMARY KEY,
        place VARCHAR(40) NOT NULL
    );

CREATE TABLE
    subject (
        id INT NOT NULL PRIMARY KEY,
        subject VARCHAR(40) NOT NULL,
        teacher_id CHAR(4) NOT NULL,
        FOREIGN KEY (teacher_id) REFERENCES teacher (teacher_id)
    );

CREATE TABLE
    course (
        id INT NOT NULL PRIMARY KEY,
        course VARCHAR(40) NOT NULL,
        place_id INT,
        FOREIGN KEY (place_id) REFERENCES place (id)
    );

CREATE TABLE
    student (
        code CHAR(5) NOT NULL PRIMARY KEY,
        name VARCHAR(40) NOT NULL,
        age INT,
        icourse_d INT,
        gender_id INT,
        FOREIGN KEY (icourse_d) REFERENCES course (id),
        FOREIGN KEY (gender_id) REFERENCES gender (id)
    );

CREATE TABLE
    scores (
        id INT NOT NULL,
        code CHAR(5) NOT NULL,
        score INT NOT NULL,
        date DATE NOT NULL,
        PRIMARY KEY (id, code),
        FOREIGN KEY (id) REFERENCES subject (id),
        FOREIGN KEY (code) REFERENCES student (code)
    );

INSERT INTO
    result (id, max, min, result)
VALUES
    (1, 59, 0, '不可'),
    (2, 69, 60, '可'),
    (3, 79, 70, '良'),
    (4, 100, 80, '優');

INSERT INTO
    gender (id, gender)
VALUES
    (1, '男'),
    (2, '女');

INSERT INTO
    teacher (teacher_id, name)
VALUES
    ('G002', '長松 次郎'),
    ('G001', '長松 遥'),
    ('G005', '高田 真由'),
    ('E012', '三浦 裕'),
    ('M301', '上条 雄三'),
    ('M553', '西岡 剛');

INSERT INTO
    place (id, place)
VALUES
    (1, 'C棟'),
    (2, 'B棟'),
    (3, 'A棟');

INSERT INTO
    subject (id, subject, teacher_id)
VALUES
    (1, '国語', 'G002'),
    (2, '物理', 'G001'),
    (3, '現代社会', 'G005'),
    (4, '電気工学', 'E012'),
    (5, '信号処理', 'E012'),
    (6, '機械力学', 'M301'),
    (7, '計測工学', 'M553');

INSERT INTO
    course (id, course, place_id)
VALUES
    (1, '都市デザイン系', 1),
    (2, '情報エレクトロニクス系', 2),
    (3, '都市デザイン系', 3);

INSERT INTO
    student (code, name, age, icourse_d, gender_id)
VALUES
    ('26401', '山川 一郎', 18, 1, 1),
    ('26205', '谷泉 太郎', 19, 2, 1),
    ('26132', '海土 花子', 20, 3, 2);

INSERT INTO
    scores (id, code, score, date)
VALUES
    (1, '26401', 65, '2025-06-10'),
    (2, '26401', 42, '2025-06-11'),
    (3, '26401', 79, '2025-06-14'),
    (2, '26205', 76, '2025-06-11'),
    (4, '26205', 66, '2025-06-12'),
    (5, '26205', 90, '2025-06-12'),
    (2, '26132', 87, '2025-06-10'),
    (6, '26132', 78, '2025-06-11'),
    (7, '26132', 100, '2025-06-12');