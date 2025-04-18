def tasu(a,b)
    c = a + b
    # return c
end

puts tasu(1,2)

def cofee(taste = "normal",sugar = "use",milk = "use")
    puts "#{taste}, #{sugar}, #{milk}"
end
cofee()
cofee("strong")
cofee("weak","no use")
cofee("normal","use","low-fat")