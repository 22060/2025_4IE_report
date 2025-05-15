loop do
    y = gets.chomp.to_i
    break if (y == 0)
 
    if y % 4 == 0 && y % 100 != 0 || y % 400 == 0
        puts "Leap year"
    else
        puts "Not a leap year"
    end
end