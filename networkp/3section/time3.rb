#22060 古城隆人
if ARGV.length != 2
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "command must be usage format\n"
    exit
end
if ARGV[0].length != 8
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 must be YYYYMMDD format\n"
    exit
end
if ARGV[1].length != 8
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 must be YYYYMMDD format\n"
    exit
end
if ARGV[0] !~ /^[0-9]+$/
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 must be number\n"
    exit
end
if ARGV[1] !~ /^[0-9]+$/
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 must be number\n"
    exit
end
if(ARGV[0][0..3].to_i < 1970)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 must be more than 19700101\n"
    exit
end
if(ARGV[1][0..3].to_i < 1970)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 must be more than 19700101\n"
    exit
end
if(ARGV[0][4..5].to_i < 1 || ARGV[0][4..5].to_i > 12)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 month must be 1-12\n"
    exit
end
if(ARGV[1][4..5].to_i < 1 || ARGV[1][4..5].to_i > 12)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 month must be 1-12\n"
    exit
end
if(ARGV[0][6..7].to_i < 1 || ARGV[0][6..7].to_i > 31)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 day must be 1-31\n"
    exit
end
if(ARGV[1][6..7].to_i < 1 || ARGV[1][6..7].to_i > 31)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 day must be 1-31\n"
    exit
end
begin 
    a = Time.local(ARGV[0][0..3], ARGV[0][4..5], ARGV[0][6..7])
    b = Time.local(ARGV[1][0..3], ARGV[1][4..5], ARGV[1][6..7])
rescue => e # 例外オブジェクトを代入した変数。
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    exit
end
if(a.to_i < 0)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 must be more than 19700101\n"
    exit
end
if(b.to_i < 0)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg2 must be more than 19700101\n"
    exit
end

diff = ((b - a) / 86400).to_i

if(diff < 0)
    print "usage: ruby time3.rb YYYYMMDD YYYYMMDD\n"
    print "arg1 must be less than arg2\n"
    exit
end
print diff, "\n"
