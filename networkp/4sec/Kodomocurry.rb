require './curry'

class Kodomocurry < Curry
    def initialize(a = 3, b = 200)
        setKarasa(a)
        setRyou(b)
        @omake = 1
    end
    def openOmake
        if @omake == 1
            puts "Omake open!"
            @omake = 0
        else
            puts "mounaiyo!"
        end
    end
    def setKarasa(a)
        if a > 10
            puts "[ERROR] Karasa is too high!"
            @karasa = 5
        else
            @karasa = a
        end
    end
end