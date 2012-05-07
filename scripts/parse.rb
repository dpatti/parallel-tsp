File.open(ARGV[0]) do |f|
    data = f.read()
    cols = [ARGV[0]]
    cols << data.match(/Best solution was (\d+)/){|m| m.captures}[0].to_i
    cols << data.match(/Ant size:\s+(\d+)/){|m| m.captures}[0].to_i
    cols << data.match(/Ant transfers:\s+(\d+)/){|m| m.captures}[0].to_i
    cols << data.match(/Total time:\s+([0-9.]+)s/){|m| m.captures}[0].to_f
    cols << data.match(/Compute time:\s+([0-9.]+)s/){|m| m.captures}[0].to_f
    cols << data.match(/Transfer time:\s+([0-9.]+)s/){|m| m.captures}[0].to_f
    cols << data.match(/ACO time:\s+([0-9.]+)s/){|m| m.captures}[0].to_f
    cols << data.match(/Sync time:\s+([0-9.]+)s/){|m| m.captures}[0].to_f
    puts cols.join("\t")
end
