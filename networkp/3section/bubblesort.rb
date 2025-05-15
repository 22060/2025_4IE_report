a = []

for i in 0..9 do
    a[i] = rand(99)
end
print "ソート前:"
print a, "\n"
for i in 0..8 do
    for j in 0..(8 - i) do
        if a[j] > a[j + 1]
            t = a[j]
            a[j] = a[j + 1]
            a[j + 1] = t
        end
    end
end
print "ソート後:"
print a, "\n"