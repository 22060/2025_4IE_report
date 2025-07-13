require 'sinatra'
set :environment, :production
get '/' do
    year = Time.now.year
    month = Time.now.month
    redirect '/' + year.to_s + '/' + month.to_s
end
get '/:y' do
    redirect '/' + params[:y] + '/' + Time.now.month.to_s
end
get '/:y/:m' do
    @year = params[:y].to_i
    @month = params[:m].to_i
    if @month < 1 || @month > 12
        redirect '/'
    elsif  @year < 0 || @year > 9999
        redirect '/'
    else
        @y1 = @year
        @m1 = @month - 1
        if @m1 == 0
            @y1 -= 1
            @m1 = 12
        end
        @y2 = @year
        @m2 = @month + 1
        if @m2 == 13
            @y2 += 1
            @m2 = 1
        end
        @nyear = @year + 1
        @pyear = @year - 1
        
        @t = "<table border>"
        @t = @t + "<tr><th>Sun</th><th>Mon</th><th>Tue</th><th>Wed</th>"
        @t = @t + "<th>Thu</th><th>Fri</th><th>Sat</th></tr>"

        l = getLastDay(@year, @month)
        h = zeller(@year, @month, 1)

        d = 1

        6.times do |p|
            @t = @t + "<tr>"
            7.times do |q|
                if p == 0 && q < h
                    @t = @t + "<td></td>"
                elsif d <= l
                    
                    if d == Time.now.day && @year == Time.now.year && @month == Time.now.month
                        @t = @t + "<td align=\"right\"><p id=\"today\" style=\"background: black;color:#a8f8ff\">#{d}</p></td>"
                    elsif (h+d) % 7 == 0
                        @t = @t + "<td align=\"right\"><font color=\"blue\">#{d}</font></td>"
                    elsif (h+d) % 7 == 1
                        @t = @t + "<td align=\"right\"><font color=\"red\">#{d}</font></td>"
                    else
                        @t = @t + "<td align=\"right\">#{d}</td>"
                    end
                    d+= 1
                else
                    @t = @t + "<td></td>"
                end
            end
            @t = @t + "</tr>"
            if d > l
                break
            end
        end

        @t = @t + "</table>"

        erb :moncal
    end
end

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