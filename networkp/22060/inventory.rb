require 'sinatra'
require 'json'
require 'active_record'
require 'sinatra/cross_origin'
require 'yaml'


set :environment, :production

register Sinatra::CrossOrigin
enable :cross_origin

helpers do
  def read_body_utf8
    raw = request.body.read
    str = raw.to_s
    str.force_encoding(Encoding::UTF_8)
    str.scrub
  end
end

ActiveRecord::Base.configurations = YAML.load_file('database.yml')
ActiveRecord::Base.establish_connection :development

class Materials < ActiveRecord::Base
self.table_name = 'materials'
end

class Products < ActiveRecord::Base
self.table_name = 'products'
end

puts "Inventory Management System Started."
puts Materials.all.to_json
puts Products.all.to_json

#materials API 材料情報についてDBから取得して返す
get '/materials' do
  r = []
  Materials.all.each do |a|
    d = {
      id: a.id,
      name: a.name,
      unit_value: a.unit_value,
      unit_amount: a.unit_amount,
      unit: a.unit,
      price: a.price
    }
    r.push(d)
  end
  r.to_json
end
# products API 製品情報についてDBから取得して返す
get '/products' do
  r = []
  Products.all.each do |p|
    d = {
      id: p.id,
      name: p.name,
      unit_value: p.unit_value,
      unit: p.unit,
      material: p.material,
      price: p.price
    }
    r.push(d)
  end
  r.to_json
end
# edit product API 製品情報の編集を行う
post '/edit_product' do
  # {id:int,material:json}
  pdata = JSON.parse(request.body.read)
  # body = read_body_utf8
  # puts body
  prod = Products.find_by(id: pdata["id"])
  if prod
    prod.material = pdata["material"].to_json
    prod.price = pdata["price"].to_i
    prod.save
    { status: "success", message: "Product updated." }.to_json
  else
    { status: "error", message: "Product not found." }.to_json
  end

end
# arrival API 在庫追加処理を行う
post '/Arrival' do
  adata = JSON.parse(read_body_utf8)
  mat = Materials.find_by(id: adata["id"])
  if mat
    mat.unit_amount += adata["quantity"].to_i
    mat.save
    { status: "success", message: "Inventory updated." }.to_json
  else
    { status: "error", message: "Material not found." }.to_json
  end
end