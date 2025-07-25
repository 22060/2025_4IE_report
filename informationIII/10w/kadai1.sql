SELECT
    student.code,
    student.name,
    student.age,
    gender.gender,
    course.course,
    place.place,
    subject.subject,
    scores.score,
    result.result,
    teacher.teacher_id,
    teacher.name AS teacher_name,
    scores.date
FROM
    scores
    inner JOIN student ON scores.code = student.code
    inner JOIN course ON student.icourse_d = course.id
    inner JOIN place ON course.place_id = place.id
    inner JOIN gender ON student.gender_id = gender.id
    inner JOIN subject ON scores.id = subject.id
    inner JOIN result ON scores.score between result.min and result.max
    inner JOIN teacher ON subject.teacher_id = teacher.teacher_id