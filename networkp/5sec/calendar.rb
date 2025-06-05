#22060 古城隆人

def leap(year)
    loop do
        y = year
        break if (y == 0)
        if y % 4 == 0 && y % 100 != 0 || y % 400 == 0
            return true
        else
            return false
        end
    end
end
def getLastDay(year, month)
    if month == 2
        if leap(year)
            return 29
        else
            return 28
        end
    elsif month == 4 || month == 6 || month == 9 || month == 11
        return 30
    else
        return 31
    end
end
def zeller(year, month, day)
    if month <= 2
        month += 12
        year -= 1
    end
    k = (year % 100).to_i
    j = (year / 100).to_i
    f = year + (year / 4).to_i - (year / 100).to_i + (year / 400).to_i + ((13 * month + 8).to_i / 5).to_i + day
    return f % 7
end

# main
if ARGV.length == 0
    year = Time.now.year
    month = Time.now.month
elsif ARGV.length == 1
    if(ARGV[0].length == 4)
        year = ARGV[0].to_i
    elsif ARGV[0].length == 6
        year = ARGV[0][0..3].to_i
        month = ARGV[0][4..5].to_i
    else
        puts "Usage: ruby calendar.rb YYYYMM or ruby calendar.rb <year> <month>"
        exit
    end
elsif ARGV.length == 2
    year = ARGV[0].to_i
    month = ARGV[1].to_i
else
    puts "Usage: ruby calendar.rb YYYYMM or ruby calendar.rb <year> <month>"
    exit
end

if year < 0 || month < 1 || month > 12
    puts "[ERROR]Invalid input"
    exit
end

puts "西暦#{year}年#{month}月"
puts "Sun Mon Tue Wed Thu Fri Sat"
g = getLastDay(year, month)
w = zeller(year, month, 1)
c = 0
while c < w
    print "    "
    c += 1
end
e = 1
while e <= g
    print " %2d " % e
    e = e + 1
    c = c + 1
    if c % 7 == 0
        puts "\n"
    end
end
puts "\n"
