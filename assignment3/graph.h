/*
Matthew Jeong ECE365 Assignment 3 graph.h 

This file holds the implementation of my graph class as well as several functions to complete the tasks
given in the assignment document.
*/

#ifndef GRAPH_H
#define GRAPH_H

#include "heap.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

class graph {
    public:

        ~graph() {
            for(vertex *ptr : appearance) {
                delete ptr;
            }
        }

        void print_loaded_data() {
            //just a simple function to help me confirm things went right, will be deleted later
            for(vertex* ptr : appearance) {
                std::cout << "vertex id: " << ptr->id << std::endl;
                std::cout << "now showing adjacency list:" << std::endl;

                for(edge dum : ptr->adj_list) {
                    std::cout << dum.dest->id << " " << dum.weight << std::endl;
                }

                std::cout << std::endl << std::endl;
            }

        }

        void load_graph(const std::string& graph_filename) {
        
            std::ifstream graph_file(graph_filename);

            if(!graph_file.is_open()) {
                std::cerr << "error occurred, could not open file" << std::endl;
                return;
            }

            std::string line;
            while(std::getline(graph_file, line)) {
             
                std::istringstream single_line(line);
                std::string src, dest;
                int weight;

                if(single_line >> src >> dest >> weight) {

                    vertex* src_vertex = static_cast<vertex*>(hashtable.getPointer(src));
                    vertex* dest_vertex = static_cast<vertex*>(hashtable.getPointer(dest));

                    if(!src_vertex) {
                        vertex* new_src_vertex = new vertex(src, 0, nullptr, false);
                        appearance.push_back(new_src_vertex);
                        src_vertex = new_src_vertex;
                        int result = hashtable.insert(src, src_vertex);
                        assert(result == 0);
                    } 

                    if(!dest_vertex) {
                        vertex* new_dest_vertex = new vertex(dest, 0, nullptr, false);
                        appearance.push_back(new_dest_vertex);
                        dest_vertex = new_dest_vertex;
                        int result = hashtable.insert(dest, dest_vertex);
                        assert(result == 0);
                    } 

                    edge new_edge = edge(dest_vertex, weight);
                    src_vertex->adj_list.push_back(new_edge);
                    
                } 
            }

            return;

        }

        bool valid_vertex(const std::string& checked_vertex) {
            return hashtable.contains(checked_vertex);
        }

        void run_dijkstra(const std::string& start_vertex) {
            heap minHeap(10000);

            for(vertex* v : appearance) {
                //1000001 represents infinity, since all distances are positive integers less than one million
                //according to the assignment doc

                v->dist = 1000001; 
                v->known = false;
                v->prev = nullptr;
            }

            vertex* start_point = static_cast<vertex*>(hashtable.getPointer(start_vertex));

            start_point->dist = 0;
            
            minHeap.insert(start_vertex, start_point->dist, start_point);

            vertex* popped_vertex = new vertex("dummy", -1, nullptr, false);

            while(!minHeap.deleteMin(nullptr, nullptr, popped_vertex)) {
                if(popped_vertex->known) {
                    continue;
                }

                popped_vertex->known = true;

                for(edge path : popped_vertex->adj_list) {
                    if(!path.dest->known && path.weight + popped_vertex->dist < path.dest->dist) {
                        path.dest->dist = path.weight + popped_vertex->dist;
                        path.dest->prev = popped_vertex;
                        minHeap.insert(path.dest->id, path.dest->dist, path.dest);
                    }
                }
            }

            return;


        }

        void write_output(const std::string& output_filename) {
            std::ifstream output_file(output_filename);

            if(!output_file.is_open()) {
                std::cerr << "error occurred, could not open file" << std::endl;
                return;
            }

            
        }

    private:

        class vertex;

        class edge {
            public: 
                edge(vertex* dest, int weight) : dest(dest), weight(weight) {}

                vertex* dest;
                int weight;
        };

        class vertex {
            public:
                vertex(std::string id, int dist, vertex* prev, bool known) : 
                    id(id), dist(dist), prev(prev), known(known) {}


                std::string id;
                int dist;
                vertex* prev;
                bool known;
                std::vector<edge> adj_list;
            
        };


        //vector of pointers to vertex objects, in the order that they appear
        std::vector<vertex*> appearance;
        hashTable hashtable;



};

#endif