h = 170
w = 70
h = h / 100.0
bmi = w/h/h

case bmi
when 40.0..100.0
    puts "肥満（4度）"
when 35.0..40.0
    puts "肥満（3度）"
when 30.0..35.0
    puts "肥満（2度）"
when 25.0..30.0
    puts "肥満（1度）"
when 18.5..25.0
    puts "普通体重"
when 0..18.5
    puts "低体重"
end