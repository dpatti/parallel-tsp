File.open("results.dat") do |f|
    runs = []
    headers = f.gets.split("\t")
    f.lines.each do |line|
        runs << Hash[*headers.zip(line.split("\t")).flatten]
    end

    #System	Cores	Graph	Ants	Method	Best Tour	Ant Byte Size	Transfers	Total	Compute	Transfer	ACO	Sync
    # Graph 1 - Bluegene with x:cores, y:time, series:[1024, 4096, 8192, 11264]
    graph1 = {}
    runs.each do |run|
        if run['System'] == 'Bluegene'
            graph1[run['Cores']] ||= {}
            graph1[run['Cores']][run['Graph']] = run['Total']
        end
    end
    # Graph 2 - Kratos with x:cores, y:time, series:[clustering, distance, round_robin] where graph_size=4096
    graph2 = {}
    runs.each do |run|
        if run['System'] == 'Kratos' and run['Graph'] == "4096"
            graph2[run['Cores']] ||= {}
            graph2[run['Cores']][run['Method']] = run['Total']
        end
    end
    # Graph 3 - Kratos with x:cores, y:time, series:[clustering, distance, round_robin] where graph_size=8192
    graph3 = {}
    runs.each do |run|
        if run['System'] == 'Kratos' and run['Method'] == "distance"
            graph3[run['Cores']] ||= {}
            graph3[run['Cores']][run['Graph']] = run['Total']
        end
    end
    # Graph 4 - Bluegene with x:cores, y:[transfer_time, compute_time]
    graph4 = {}
    runs.each do |run|
        if run['System'] == 'Bluegene'
            graph4[run['Cores']] = {
                'transfer_time' => run['Transfer'],
                'compute_time' => run['Compute'],
            }
        end
    end
    [graph1, graph2, graph3, graph4].each_with_index do |g, i|
        File.open("graph%d.dat" % i, "w") do |f|
            g.each do |cores, graph|
                f << cores + "\t" + graph.to_a.sort.map{|i,v| v}.join("\t") + "\n"
            end
        end
    end
end
