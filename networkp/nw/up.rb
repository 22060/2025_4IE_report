require 'sinatra'
require 'active_record'
require 'digest/sha2'
require 'cgi'

set :environment, :production

set :sessions,
  expire_after: 7200,
  secret: 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghij'
ActiveRecord::Base.configurations = YAML.load_file('database.yml')
ActiveRecord::Base.establish_connection(:development)

class Account < ActiveRecord::Base
  self.table_name = 'account'
end

class Filesdata < ActiveRecord::Base
  self.table_name = 'filesdata'
end

class Image
  attr_accessor :images_info, :images_path
  
  def initialize(info, path)
    @images_info = info
    @images_path = path
  end
end
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

get '/' do
  @u = session[:username]
  if @u == nil
    redirect '/badrequest'
  end
  @error = session.delete(:error)
  # images_name = Dir.glob("public/files/*")
  images_name = Filesdata.all
  puts images_name
  @images = []
  @t = "<table border='1'><tr><th>ユーザー名</th><th>ファイル名</th><th>ファイルサイズ</th><th>最終更新日時</th><th>削除</th></tr>"
  images_name.each do |a|
    path = a.filename
    info = " " + File.size(File.join("public/files/", path)).to_s + " bytes " + File.mtime(File.join("public/files/", path)).to_s
    @t += "<tr><td>#{a.userid}</td>"
    @t += "<td><a href='/files/#{path}'>#{path}</a></td><td>#{File.size(File.join("public/files/", path))} bytes</td><td>#{File.mtime(File.join("public/files/", path))}</td>"
    if( a.userid == @u )
      @t += "<td><form method='post' action='/delete'><input type='hidden' name='filename' value='#{path}'><input type='submit' value='削除'></form></td></tr>"
    else
      @t += "<td>--</td></tr>"
    end
  end
  @t += "</table>"
  erb :index
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
get '/badrequest' do
  erb :badrequest
end

post '/upload' do
  s = params[:file]
  u = session[:username]
  puts u
  if u == nil
    puts "no login"
    session[:error] = 'You must be logged in to upload files'
  end
  if s != nil
    save_path = "./public/files/#{params[:file][:filename]}"
    File.open(save_path, 'wb') do |f|
      g = params[:file][:tempfile]
      f.write g.read
    end
    begin 
      f = Filesdata.new
      f.userid = u
      f.filename = params[:file][:filename]
      f.uploadate = Time.now.to_i
      f.save
    rescue => e
      puts e.message
    end
  else
    session[:error] = 'No file selected'
  end
  redirect '/'
end

#ログイン認証用エンドポイント
post '/auth' do
  user = params[:uname]
  pass = params[:pass]

  r = checkLogin(user, pass) # ログイン認証を実行

  if r == 1
    session[:username] = user
    redirect '/'
  end

  redirect '/loginfailure'
end

post '/delete' do
  u = session[:username]
  if u == nil
    redirect '/badrequest'
  end
  a = Filesdata.find_by(filename: params[:filename])
  if a != nil
    if a.userid == u
      a.destroy
    else
      session[:error] = 'You are not authorized to delete this file'
      redirect '/'
    end
  else
    redirect '/badrequest'
  end
  filename = params[:filename]
  file_path = "./public/files/#{filename}"
  if File.exist?(file_path)
    File.delete(file_path)
  end
  redirect '/'
end