def calc(h,w)
    w / h / h
end
print "身長(m):"
height = gets.chomp.to_f
print "体重(kg):"
weight = gets.chomp.to_f
bmi = calc(height, weight)
puts "BMI:#{bmi}"
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