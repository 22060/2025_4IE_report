class Curry
    def initialize(a = 3, b = 300)
        setKarasa(a)
        setRyou(b)
    end
    def setKarasa(a)
        @karasa = a
    end
    def setRyou(b)
        @ryou = b
    end
    def getKarasa
        return @karasa
    end
    def getRyou
        return @ryou
    end
end
