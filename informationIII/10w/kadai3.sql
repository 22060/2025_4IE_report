SELECT
    student.name,
    subject.subject,
    teacher.name AS teacher_name
FROM
    scores
    inner JOIN student ON scores.code = student.code
    inner JOIN course ON student.icourse_d = course.id
    inner JOIN place ON course.place_id = place.id
    inner JOIN gender ON student.gender_id = gender.id
    inner JOIN subject ON scores.id = subject.id
    inner JOIN result ON scores.score between result.min and result.max
    inner JOIN teacher ON subject.teacher_id = teacher.teacher_id
WHERE
    teacher.name LIKE '%高田%'
    OR teacher.name LIKE '%三浦%';