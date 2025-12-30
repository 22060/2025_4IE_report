#212 古城隆人
require 'sinatra'
require 'active_record'
require 'digest/sha2'
require 'cgi'
require 'mimemagic'
# セッション管理の設定
set :sessions,
  expire_after: 7200,
  secret: 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghij'

ActiveRecord::Base.configurations = YAML.load_file('database.yml')
ActiveRecord::Base.establish_connection(:development)


# データベースのモデル定義
class BBSdata < ActiveRecord::Base
  self.table_name = 'bbsdata'
end

class Account < ActiveRecord::Base
  self.table_name = 'account'
end

set :environment, :production

#ログイン認証を行う．成功したら1，パスワード不一致で0，ユーザ名が存在しない場合2を返す
def checkLogin(trial_username, trial_password)
  r = 0

  begin
    a = Account.find(trial_username)
    db_username = a.id
    db_salt = a.salt
    db_hashed = a.hashed
    trial_hashed = Digest::SHA2.hexdigest(trial_password + db_salt) # パスワードのハッシュ値を計算
    if trial_hashed == db_hashed
      r = 1
    end
  rescue => e
    r = 2
  end
  return r
end
  
# ルーティングの定義
get '/' do
  redirect '/login'
end
#ページ表示
get '/login' do
  erb :login
end
#ログアウトページを表示させ，セッションを全部削除
get '/logout' do
  session.clear
  erb :logout
end
#ログインが失敗した場合のページ表示
get '/loginfailure' do
  erb :loginfailure
end
#次の10件を表示させるためにセッションを変更
get '/next' do
  page = session[:page]
  if page == nil
    page = 2
  else
    if page < BBSdata.all.count / 10 + 1 # 最大ページ数を超えないようにする
      page = page.to_i + 1
    end
  end
    session[:page] = page
  redirect '/contents'
end

#前の10件を表示させるためにセッションを変更
get '/pre' do
  page = session[:page]
  if page == nil
    page = 1
  else
    if page > 1 # 1ページ未満にならないようにする
      page = page.to_i - 1
    end
  end
    session[:page] = page
  redirect '/contents'
end

get '/contents' do
  @u = session[:username]
  page = session[:page] # 現在のページ番号をセッションから取得
  i = 0
  if page == nil
    page = 1
    session[:page] = page
  else
    page = page.to_i
  end
  if @u == nil
    redirect '/badrequest' # ログインしていない場合はエラー画面へ
  end

  a = BBSdata.all 
  if a.count == 0
    @t = "<tr><td>No entries in this BBS.</td></tr>"
  else
    @t = ""
    a.reverse_each  do |b|
      if i >= (page - 1) * 10 && i < page * 10
        
        @t = @t + "<tr>"
        @t = @t + "<td>#{b.id}</td>"
        c = Account.find(b.userid)
        @t = @t + "<td><img src='#{c.iconfile}' width=64 height=64></td>"
        @t = @t + "<td>#{b.userid}</td>"
        @t = @t + "<td>#{Time.at(b.writedate)}</td>"
        if b.userid == @u
          @t = @t + "<td><form action=\"/delete\" method=\"post\">"
          @t = @t + "<input type=\"hidden\" name=\"id\" value=\"#{b.id}\">"
          @t = @t + "<input type=\"hidden\" name=\"_method\" value=\"delete\">"
          @t = @t + "<input type=\"submit\" value=\"DELETE\"></form></td>"
        else
          @t = @t + "<td></td>"
        end
        @t = @t + "</tr>"
        if b.imgfile != ""
          if MimeMagic.by_path(b.imgfile) != nil && MimeMagic.by_path(b.imgfile).type.start_with?('image/')
            @t = @t + "<tr><td colspan='4'>#{b.entry}<br><a href ='#{b.imgfile}'><img src='#{b.imgfile}' style='max-width: 400px;'></a></td></tr>\n"
          else
            @t = @t + "<tr><td colspan='4'>#{b.entry}<br><a href ='#{b.imgfile}'>Download File</a></td></tr>\n"
          end
        else
          @t = @t + "<tr><td colspan='4'>#{b.entry}</td></tr>\n"
        end
      end
      i = i + 1
    end
    if i > page * 10 # 次のページが存在するなら「次のページ」リンクを表示
      @t = @t + "<tr><td colspan='4'><a href=\"/next\">Next Page</a></td></tr>\n"
    end
    if page > 1 # 2ページ目以降なら「前のページ」リンクを表示
      @t = "<tr><td colspan='4'><a href=\"/pre\">Pre Page</a></td></tr>" + @t
    end
  end

  erb :contents
end

#ログイン認証用エンドポイント
post '/auth' do
  user = params[:uname]
  pass = params[:pass]

  r = checkLogin(user, pass) # ログイン認証を実行

  if r == 1
    session[:username] = user
    redirect '/contents'
  end

  redirect '/loginfailure'
end
#新規投稿用エンドポイント
post '/new' do
  maxid = 0
  a = BBSdata.all
  a.each do |b|
    if b.id > maxid
      maxid = b.id
    end
  end
  s = BBSdata.new
  s.id = maxid + 1
  s.userid = session[:username]
  s.entry = CGI.escapeHTML(params[:entry]) #サニタイズを実行
  s.writedate = Time.now.to_i

  p = params[:file]
  if p != nil
    ext = File.extname(params[:file][:filename])
    puts params[:file][:filename] + Time.now.to_s + (maxid + 1).to_s
    fname = Digest::SHA2.hexdigest(params[:file][:filename] + Time.now.to_s + (maxid + 1).to_s)
    fname = "#{fname}#{ext}"
    save_path = "./public/#{fname}"
    File.open(save_path, 'wb') do |f|
      g = p[:tempfile]
      f.write g.read
    end
    s.imgfile = "/#{fname}"
  else
    s.imgfile = ""
  end
  s.save
  session[:page] = 1 # 新規投稿したら1ページ目に戻る
  redirect '/contents'
end

delete '/delete' do
  begin
    s = BBSdata.find(params[:id])
    f = s.imgfile
    if f != ""
      File.delete("./public/#{f}") if File.exist?("./public/#{f}")
    end
    s.destroy
    redirect '/contents'
  rescue => e
    puts "Error occurred while deleting: #{e.message}"
    redirect '/contents'
  end
end