result = 0
count = 0
i = 1
until count > 19 do
    i += 1
    for j in 2..100 do
        break if i == j
        if i % j == 0
            result = 1
            break
        end
    end
    if result == 0
        puts i
        count += 1
    end
    result = 0
end