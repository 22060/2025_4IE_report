x = 1
i = 1
until i  >= 101
    x = i
    print x,":"
    until x == 1 do
        if (x % 2) == 0
            x = x / 2
        else
            x = (3 * x) + 1
        end
        print x, " "
    end
    print "\n"
    i += 1
end