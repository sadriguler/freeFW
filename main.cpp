#include <iostream> // For std::cout
#include <fstream>  // For std::ifstream
#include <sstream>  // For std::stringstream
#include <string>   // For std::string and std::getline
#include <vector>
// Eigen includes
#include <Dense>
#include <Geometry> 

int main() {
    // Create an input file stream object
    std::string filename = "sphere_r300mm_mesh100mm.unv";
    std::ifstream inputFile(filename);
    // Check if the file was opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return 1; // Indicate an error
    }

    int file_read_start_line{0};
    std::string line;
    // Read the file line by line
    int number_of_nodes{0}, number_of_triangles{0}, counter{0};
    while (std::getline(inputFile, line)) {
        counter++;
        // Check file locations
        if (line.substr(0,6) == "    -1"){
            std::getline(inputFile, line);
            counter++;
            if (line.substr(0,6) == "    -1"){
                std::getline(inputFile, line);
                counter++;
                if (line.substr(0,6) == "  2411"){
                    file_read_start_line = counter;
                    counter = 0;
                }
                else if (line.substr(0,6) == "  2412"){
                    number_of_nodes = (counter-3)/2;
                    counter = 0;
                }
            }
        }
    }
    inputFile.close();
    number_of_triangles = (counter - 2)/2;

    std::cout << "Number of nodes: " << number_of_nodes << std::endl;
    std::cout << "Number of triangles: " << number_of_triangles << std::endl;
    std::cout << "Number of edges, guessed: " << (number_of_triangles*3)/2 << std::endl;
    
    Eigen::MatrixX3d node_coordinates(number_of_nodes, 3);
    Eigen::MatrixX3i triangle_nodes(number_of_triangles, 3);

    // std::vector<std::vector<double>> node_coordinates()
    inputFile.open(filename);
    // Check if the file was opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return 1; // Indicate an error
    }
    for (int i = 0; i < file_read_start_line; i++){
        std::getline(inputFile, line);
    }
    for (int i = 0; i < number_of_nodes; i++){
        std::getline(inputFile, line);
        std::getline(inputFile, line);
        std::stringstream ss(line);
        double x, y, z;
        ss >> x >> y >> z;
        node_coordinates(i, 0) = x;
        node_coordinates(i, 1) = y;
        node_coordinates(i, 2) = z;
        // std::cout << node_coordinates(i, 0) << " " << node_coordinates(i, 1) << " " << node_coordinates(i, 2) << std::endl;
    }
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    for (int i = 0; i < number_of_triangles; i++){
        std::getline(inputFile, line);
        std::getline(inputFile, line);
        std::stringstream ss(line);
        int x, y, z;
        ss >> x >> y >> z;
        triangle_nodes(i, 0) = x;
        triangle_nodes(i, 1) = y;
        triangle_nodes(i, 2) = z;
        // std::cout << triangle_nodes(i, 0) << " " << triangle_nodes(i, 1) << " " << triangle_nodes(i, 2) << std::endl;
    }
    // The file is automatically closed when 'inputFile' goes out of scope (RAII)
    inputFile.close();

    bool node_count = false;
    if (node_count){
        Eigen::VectorXi node_counts(number_of_nodes);
        node_counts.setZero();
        for (int i = 0; i < number_of_triangles; i++){
            node_counts(triangle_nodes(i, 0)-1)++;
            node_counts(triangle_nodes(i, 1)-1)++;
            node_counts(triangle_nodes(i, 2)-1)++;
        }
        int max_node_count = 0;
        for (int i = 0; i < number_of_nodes; i++){
            if (node_counts(i) > max_node_count){
                max_node_count = node_counts(i);
            }
        }
        std::cout << max_node_count << std::endl;
        std::cout << node_counts.mean() << std::endl;
    }

    Eigen::Matrix<int, 3, 2> selected_triangle_edges;
    Eigen::MatrixX2i edge_nodes(number_of_triangles*3/2, 2);
    Eigen::MatrixX3i triangle_edges(number_of_triangles, 3);
    Eigen::MatrixX2i edge_triangles(number_of_triangles*3/2, 2);
    edge_nodes.setZero();
    int number_of_edges = 0;
    int i_edge;
    for (int i_tri = 0; i_tri < number_of_triangles; i_tri++){
        selected_triangle_edges <<   triangle_nodes(i_tri, 0), triangle_nodes(i_tri, 1),
                            triangle_nodes(i_tri, 1), triangle_nodes(i_tri, 2),
                            triangle_nodes(i_tri, 2), triangle_nodes(i_tri, 0);

        for (int i_tri_edge = 0; i_tri_edge < 3; i_tri_edge++){
            // Check if the edge is defined before!
            for (i_edge = 0; i_edge < number_of_edges; ){
                if (edge_nodes(i_edge, 1) == selected_triangle_edges(i_tri_edge, 0) 
                        && edge_nodes(i_edge, 0) == selected_triangle_edges(i_tri_edge, 1)){
                    triangle_edges(i_tri, i_tri_edge) = i_edge;
                    edge_triangles(i_edge, 1) = i_tri;
                    break;
                }
                i_edge++;
            }
            // An error flag is needed here!
            // If the actual number of edges is grater than the guessed the number of edges,
            // it should throw an error due to the memory allocation.
            if (i_edge == number_of_edges){
                    edge_nodes(number_of_edges, 0) = selected_triangle_edges(i_tri_edge, 0);
                    edge_nodes(number_of_edges, 1) = selected_triangle_edges(i_tri_edge, 1);
                    triangle_edges(i_tri, i_tri_edge) = i_edge;
                    edge_triangles(i_edge, 0) = i_tri;
                    number_of_edges++;
                }
        }
    }   
    std::cout << "Number of edges: " << number_of_edges << std::endl;
    
    // Compute edge lengths
    Eigen::VectorXd edge_lengths(number_of_edges);
    Eigen::Vector3d diff_vector;
    int node1, node2;
    double edge_length;
    for (i_edge = 0; i_edge < number_of_edges; i_edge++){
        node1 = edge_nodes(i_edge, 0);
        node2 = edge_nodes(i_edge, 1);
        edge_length = (node_coordinates.row(node1-1) - node_coordinates.row(node2-1)).norm();
        edge_lengths(i_edge) = edge_length;
    }
    std::cout << "Average edge length is " << edge_lengths.mean() << " mm" << std::endl;
    
    // Compute triangle areas
    int node3;
    Eigen::Vector3d edge_vector1, edge_vector2;
    Eigen::VectorXd tri_areas(number_of_triangles);
    for (int i_tri = 0; i_tri < number_of_triangles; i_tri++){
        node1 = triangle_nodes(i_tri, 0);
        node2 = triangle_nodes(i_tri, 1);
        node3 = triangle_nodes(i_tri, 2);
        edge_vector1 = node_coordinates.row(node2-1) - node_coordinates.row(node1-1);
        edge_vector2 = node_coordinates.row(node3-1) - node_coordinates.row(node1-1);
        tri_areas(i_tri) = edge_vector1.cross(edge_vector2).norm();
    }
    std::cout << "Average triangle area is " << tri_areas.mean() << " mm^2" << std::endl;
    
    
    return 0; // Indicate success
}