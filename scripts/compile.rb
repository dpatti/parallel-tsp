File.open(ARGV[0]) do |f|
    runs = []
    headers = f.gets.split("\t")
    f.lines.each do |line|
        runs << Hash[*headers.zip(line.split("\t")).flatten]
    end

    #System	Cores	Graph	Ants	Method	Best Tour	Ant Byte Size	Transfers	Total	Compute	Transfer	ACO	Sync
    graphs = []

    # Graph 0 - Bluegene with x:cores, y:time, series:[1024, 4096, 8192, 11264]
    g = {}
    runs.each do |run|
        if run['System'] == 'Bluegene' and run['VN?'] == 'no' and run['Ants'] == '512'
            g[run['Cores']] ||= {}
            g[run['Cores']][run['Graph'].to_i] = run['Total']
        end
    end
    puts g
    graphs << g

    # Graph 1 - Kratos with x:cores, y:time, series:[clustering, distance, round_robin] where graph_size=4096
    g = {}
    runs.each do |run|
        if run['System'] == 'Kratos' and run['Graph'] == "4096"
            g[run['Cores']] ||= {}
            g[run['Cores']][run['Method']] = run['Total']
        end
    end
    graphs << g

    # Graph 2 - Kratos with x:cores, y:time, series:[clustering, distance, round_robin] where graph_size=8192
    g = {}
    runs.each do |run|
        if run['System'] == 'Kratos' and run['Method'] == "distance"
            g[run['Cores']] ||= {}
            g[run['Cores']][run['Graph'].to_i] = run['Total']
        end
    end
    graphs << g

    # Graph 3 - Bluegene with x:cores, y:[transfer_time, compute_time]
    g = {}
    runs.each do |run|
        if run['System'] == 'Bluegene' and run['VN?'] == 'yes' and run['Graph'] == '4096'
            g[run['Cores']] = {
                'transfer_time' => run['Transfer'],
                'compute_time' => run['Compute'],
            }
        end
    end
    graphs << g

    # Graph 4 - Kratos with x:cores, y:[transfer_time, compute_time]
    g = {}
    runs.each do |run|
        if run['System'] == 'Kratos' and run['Graph'] == '4096'
            g[run['Cores']] = {
                'transfer_time' => run['Transfer'],
                'compute_time' => run['Compute'],
            }
        end
    end
    graphs << g

    # Graph 5 - Bluegene with x:cores, y:time, series[VN, no_VN]
    g = {}
    runs.each do |run|
        if run['System'] == 'Bluegene' and run['Graph'] == '4096'
            g[run['Cores']] ||= {}
            g[run['Cores']][run['VN?'] == 'yes' ? 0 : 1] = run['Total']
        end
    end
    graphs << g

    graphs.each_with_index do |g, i|
        File.open(ARGV[1] + "graph%d.dat" % i, "w") do |f|
            headers = g.values.map{|i| i.keys}.flatten.uniq.sort
            g.sort_by{|a,b| a.to_i}.each do |cores, graph|
                f << cores + "\t" + headers.map{|h| graph[h] || ""}.join("\t") + "\n"
            end
        end
    end
end
