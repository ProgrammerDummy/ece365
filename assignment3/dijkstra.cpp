/*
Matthew Jeong ECE365 Assignment 3
This is my implementation of Dijkstra's shortest path finding algorithm using the custom binary heap class and hash table class.
The program takes in a txt file of vertices and the edges connecting them with non-negative weights and 
takes a starting node to which the shortest paths to all other vertices are found and written to in an output txt file.
The program also records the time to apply the algorithm in seconds.

*/

#include<chrono>

#include "graph.h"

int main(void) {

    graph myGraph;

    std::string graph_filename;
    std::string start_vertex;
            
    std::cout << "Enter name of graph file: ";

    std::cin >> graph_filename;

    myGraph.load_graph(graph_filename);

    std::cout << "Enter name of starting vertex: ";

    std::cin >> start_vertex;

    if(!myGraph.valid_vertex(start_vertex)) {
        std::cout << "invalid vertex given" << std::endl;
    }
    
    std::clock_t dijkstra_starttime = clock();

    myGraph.run_dijkstra(start_vertex);

    std::clock_t dijkstra_endtime = clock();

    double dijkstra_total_time = static_cast<double>(dijkstra_endtime - dijkstra_starttime)/CLOCKS_PER_SEC;

    std::cout << "Total time (in seconds) to apply Dijkstra's algorithm: " << dijkstra_total_time << std::endl;

    std::cout << "Enter name of output file: ";

    std::string output_filename;

    std::cin >> output_filename;

   myGraph.write_output(output_filename);

}