require 'sinatra'
require 'active_record'

set :environment, :production

ActiveRecord::Base.configurations = YAML.load_file('database.yml')
ActiveRecord::Base.establish_connection(:development)

class Datedata < ActiveRecord::Base
  self.table_name = 'datedata'
end
def getday(num) #数字を曜日に変換
    case num
    when 0
        return "土"
    when 1
        return "日"
    when 2
        return "月"
    when 3
        return "火"
    when 4
        return "水"
    when 5
        return "木"
    when 6
        return "金"
    else
        return "不明"
    end
end
get '/kanrie/:e' do #管理ページのエラー表示
    @error = params[:e]
    @t = "<a href=\"/kanri\">管理ページへ戻る</a>"
    erb :kanri
end
get '/kanri' do #管理ページ
    @t = "<table border>" #テーブル作成
    @t = @t + "<tr><th>id</th><th>月</th><th>日付</th><th>第何週</th><th>曜日</th><th>名前</th><th>色</th><th>変更</th></tr>"
    Datedata.all.each do |a| #データをテーブルに挿入
        @t = @t + "<tr>"
        @t = @t + "<td>#{a.id}</td>"
        @t = @t + "<td>#{a.month}</td>"
        @t = @t + "<td>#{(a.ischangedata==0) ? (a.date) : "-"}</td>"
        @t = @t + "<td>#{(a.ischangedata!=0) ? (a.ischangedata) : "-"}</td>"
        @t = @t + "<td>#{(a.ischangedata!=0) ? getday(a.date): "-"}</td>"
        @t = @t + "<td>#{a.name}</td>"
        @t = @t + "<td>##{a.color.to_s}</td>"
        @t = @t + "<form method=\"post\" action=\"/del\">"
        @t = @t + "<td><input type=\"submit\" value=\"Delete\"></td>"
        @t = @t + "<input type=\"hidden\" name=\"id\" value=\"#{a.id}\">"
        @t = @t + "<input type=\"hidden\" name=\"_method\" value=\"delete\">"
        @t = @t + "</form>"
        @t = @t + "</tr>\n"
    end
    #form作成
    @t = @t + "<tr>"
    @t = @t + "<form method=\"post\" action=\"/new\">"
    @t = @t + "<td><input type=\"radio\" id=\"fixed\" name=\"days\" checked value=\"0\">
              <label for=\"fixed\">日付固定</label><div></div>
              <input type=\"radio\" id=\"fix\" name=\"days\" value=\"1\">
              <label for=\"fix\">曜日固定</label>
              </td>"
    @t = @t + "<td><input type=\"number\" name=\"month\" placeholder=\"月\"></td>"
    @t = @t + "<td><input type=\"number\" name=\"date\" placeholder=\"日付\"></td>"
    @t = @t + "<td><input type=\"number\" name=\"ischangedata\" placeholder=\"第何週\"></td>"
    @t = @t + "<td><select name=\"day\">"
    @t = @t + "<option value=\"-1\">指定なし</option>"
    @t = @t + "<option value=\"0\">土</option>"
    @t = @t + "<option value=\"1\">日</option>"
    @t = @t + "<option value=\"2\">月</option>"
    @t = @t + "<option value=\"3\">火</option>"
    @t = @t + "<option value=\"4\">水</option>"
    @t = @t + "<option value=\"5\">木</option>"
    @t = @t + "<option value=\"6\">金</option>"
    @t = @t + "</select></td>"
    @t = @t + "<td><input type=\"text\" name=\"name\" placeholder=\"名前\"></td>"
    @t = @t + "<td><input type=\"color\" name=\"color\" value=\"#e600ff\"></td>"
    @t = @t + "<td><input type=\"submit\" valuew=\"Submit\" ></td>"
    @t = @t + "</form>"
    @t = @t + "</table>"
    #説明文
    @t = @t + "<p>日付固定の場合は月と日付を指定．曜日固定の場合は第何週と曜日を記入</p>"
    erb :kanri
end
def deleteerstr(str) #文字列から不正な文字を削除
    str = str.gsub(/<[^>]*>/, '')
    str = str.gsub("\"", '')
    str = str.gsub("'", '')
    str = str.gsub(";", '')
    str = str.gsub("`", '')
    str = str.gsub("(", '')
    str = str.gsub(")", '')
    str = str.gsub("=", '')
    str = str.gsub("!", '')
    str = str.gsub(">", '')
    str = str.gsub("<", '')
    str = str.gsub("&", '')
    str = str.gsub("%", '')
    return str
end

post '/new' do
    if params[:days] == "0" #日付固定か曜日固定の判別
        #入力エラー判定
        if params[:month].to_i < 1 || params[:month].to_i > 12
            @error = "月の値が不正です．"
            redirect '/kanrie/'+@error
        elsif params[:date].to_i < 1 || params[:date].to_i > 31
            @error = "日付の値が不正です．"
            redirect '/kanrie/'+@error
        elsif deleteerstr(params[:name]).empty?
            @error = "名前が入力されていません．"
            redirect '/kanrie/'+@error
        elsif deleteerstr(params[:color]).empty? || params[:color].length != 7 || params[:color][0] != '#'
            @error = "色の値が不正です．"
            redirect '/kanrie/'+@error
        end
        
        #データベースに保存
        s = Datedata.new
        s.month = params[:month].to_i
        s.date = params[:date].to_i
        s.ischangedata = 0
        s.name = deleteerstr(params[:name])
        s.color = deleteerstr(params[:color].gsub('#', ''))
    elsif params[:days] == "1"
        #入力エラー判定
        if params[:month].to_i < 1 || params[:month].to_i > 12
            @error = "月の値が不正です．"
            redirect '/kanrie/'+@error
        
        elsif params[:ischangedata].to_i < 0 || params[:ischangedata].to_i > 5
            @error = "第何週の値が不正です．"
            redirect '/kanrie/'+@error
        elsif params[:day].to_i < -1 || params[:day].to_i > 6
            @error = "曜日の値が不正です．"
            redirect '/kanrie/'+@error
            elsif deleteerstr(params[:name]).empty?
            @error = "名前が入力されていません．"
            redirect '/kanrie/'+@error
        elsif deleteerstr(params[:color]).empty? || params[:color].length != 7 || params[:color][0] != '#'
            @error = "色の値が不正です．"
            redirect '/kanrie/'+@error
        end
        #データベースに保存
        s = Datedata.new
        s.month = params[:month].to_i
        s.date = -1
        s.ischangedata = params[:ischangedata].to_i
        s.name = deleteerstr(params[:name])
        s.color = deleteerstr(params[:color].gsub('#', ''))
        s.date = params[:day].to_i
    else
        @error = "日付の指定方法が不正です．"
        redirect '/kanrie/'+@error
    end
    s.save
  redirect '/kanri'
end

delete '/del' do
  s = Datedata.find(deleteerstr(params[:id])) #IDを取得してデータを削除
  s.destroy
  redirect '/kanri'
end

get '/' do #今日の日付を取得して，その月のカレンダーを表示
    year = Time.now.year
    month = Time.now.month
    redirect '/' + year.to_s + '/' + month.to_s
end
get '/:y' do #年だけ指定された場合，その年の現在の月のカレンダーを表示
    redirect '/' + params[:y] + '/' + Time.now.month.to_s
end
get '/:y/:m' do #年と月が指定された場合，その月のカレンダーを表示
    @year = params[:y].to_i
    @month = params[:m].to_i
    if @month < 1 || @month > 12
        redirect '/'
    elsif  @year < 0 || @year > 9999
        redirect '/'
    else
        hol = Datedata.where(month: @month) #指定された月の祝日データを取得
        #前月，翌月，前年，翌年のURLリンク作成用の変数を計算
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
        once = 0
        issun = 0
        lasthol = 0
        isnexthol = 0
        6.times do |p|
            @t = @t + "<tr>"
            7.times do |q|
                if p == 0 && q < h
                    @t = @t + "<td></td>"
                elsif d <= l
                    if hol.empty? #祝日データがない場合
                        
                        if d == Time.now.day && @year == Time.now.year && @month == Time.now.month
                            @t = @t + "<td align=\"right\"><p id=\"today\" style=\"background: black;color:#a8f8ff\">#{d}</p></td>"
                        elsif (h+d) % 7 == 1
                            @t = @t + "<td align=\"right\"><font color=\"red\">#{d}</font></td>"
                        elsif (h+d) % 7 == 0
                            @t = @t + "<td align=\"right\"><font color=\"blue\">#{d}</font></td>"
                        else
                            @t = @t + "<td align=\"right\">#{d}</td>"
                        end
                    else
                        # 祝日データがある場合
                        hol.each do |c| #祝日の数だけfor文を回し，日付に合致するか確認
                            if c.ischangedata !=0 #曜日固定の場合
                                if (d) / 7 + (d%7==0?0:1) == c.ischangedata && (h+d) % 7 == c.date&& once == 0
                                    #入れ子要素によりcursorがある場合に曜日の名前が出てくるようにしている
                                    @t = @t + "<td align=\"right\"><font class=\"tooltip\"><p style=\"color:##{c.color.to_s};\">#{d}</p><span class=\"tooltip-text\">#{c.name}</span></font></td>"
                                    once = 1
                                end
                            else
                                if d  == c.date && once == 0
                                    @t = @t + "<td align=\"right\"><font class=\"tooltip\"><p style=\"color:##{c.color.to_s};\">#{d}</p><span class=\"tooltip-text\">#{c.name}</span></font></td>"
                                    once = 1
                                end
                            end
                        end
                        if once == 0
                            if d == Time.now.day && @year == Time.now.year && @month == Time.now.month
                                @t = @t + "<td align=\"right\"><p id=\"today\" style=\"background: black;color:#a8f8ff\">#{d}</p></td>"
                            elsif (h+d) % 7 == 1
                                @t = @t + "<td align=\"right\"><font color=\"red\">#{d}</font></td>"
                                if isnexthol == 1
                                    isnexthol = 0
                                end
                            elsif (h+d) % 7 == 0
                                @t = @t + "<td align=\"right\"><font color=\"blue\">#{d}</font></td>"
                            elsif issun == 1 #振替休日
                                    @t = @t + "<td align=\"right\"><font class=\"tooltip\"><font color=\"red\">#{d}</font><span class=\"tooltip-text\">振替休日</span></font></td>"
                                issun = 0
                            elsif isnexthol == 1
                                @t = @t + "<td align=\"right\"><font color=\"red\">#{d}</font></td>"
                                isnexthol = 0
                            else
                                @t = @t + "<td align=\"right\">#{d}</td>"
                            end
                        else
                            #祝日の日が日曜日だった場合，振替休日の作成
                            if (h+d) % 7 == 1
                                issun = 1
                            end
                            #祝日の日の次の日が平日であり，その次の日が祝日だった場合，間の平日が休日になる．
                            hol.each do |c|
                                if c.ischangedata !=0
                                    if (d+2) / 7 + (d+2%7==0?0:1) == c.ischangedata && (h+d+2) % 7 == c.date
                                        isnexthol = 1
                                    end
                                else
                                    if d+2  == c.date
                                        isnexthol = 1
                                    end
                                end
                            end
                            if isnexthol == 1
                                hol.each do |c|
                                    if c.ischangedata !=0
                                        if (d+1) / 7 + (d+1%7==0?0:1) == c.ischangedata && (h+d+1) % 7 == c.date
                                            isnexthol = 0
                                        end
                                    else
                                        if d+1  == c.date
                                            isnexthol = 0
                                        end
                                    end
                                end
                            end
                        end
                        once = 0
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