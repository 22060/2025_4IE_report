require './curry'
require'./Kodomocurry'
if(ARGV.length != 2)
    puts "Usage: ruby main.rb <karasa> <ryou>"
    exit
end
karasa = ARGV[0].to_i
ryou = ARGV[1].to_i
kodomo = Kodomocurry.new(karasa,ryou)
omake_test = Kodomocurry.new
puts "kodomo"
puts "Karasa: #{kodomo.getKarasa}"
puts "Ryou: #{kodomo.getRyou}"
kodomo.openOmake
kodomo.openOmake
omake_test.openOmake
omake_test.openOmake