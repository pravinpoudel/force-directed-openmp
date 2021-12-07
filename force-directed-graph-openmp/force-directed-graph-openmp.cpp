#include<iostream>
#include<cmath>
#include<vector>
#include<omp.h>

using namespace std;

//this initialization will not throw error 
struct NodeLayout {
	int index = 0;
	struct Position {
		float x = rand() % 1000 / 100;
		float y = rand() % 1000 / 100;
	}position;

	struct Displacement {
		float x = 0.0;
		float y = 0.0;
	}displacement;
};

struct EdgeLayout {
	NodeLayout& node1;
	NodeLayout& node2;
	float weight;
};

//size_t is used as data type for unsigned type or non negative number
//size_t could be used for unsigned char, unsigned short, unsigned int, unsigned long or
//unsigned long long

struct GraphLayout {

	vector<NodeLayout> nodeList;
	vector<EdgeLayout> edgeList;
	int n_count = 0;
	int e_count = 0;
	void addNode(size_t node_count);
	void addEdge(size_t v0, size_t v1, float weight);
	void start(size_t iteration);

};

void GraphLayout::addNode(size_t node_count) {
	n_count = node_count;
	for (int i = 0; i < node_count; i++) {
		NodeLayout node;
		node.index = i;
		printf("noce %d has position of (%f , %f) with displacement of (%f , %f) \n", i, node.position.x, node.position.y, node.displacement.x, node.displacement.y);
		nodeList.push_back(node);
	}
}

void GraphLayout::addEdge(size_t v0, size_t v1, float weight) {
	if (v0 == v1 || weight == 0.0f || (nodeList.size() < max(v0, v1))) {
		return;
	}
	//please check why other initialization method not working; learn struct more in c++
	//EdgeLayout NewEdge;

	struct EdgeLayout NewEdge = { nodeList[v0], nodeList[v1], weight };
	e_count++;
	edgeList.push_back(NewEdge);
}

void GraphLayout::start(size_t max_iteration_count) {
	size_t nodeCount = nodeList.size();

	int WIDTH = 600;
	int HEIGHT = 400;
	float area = WIDTH * HEIGHT;
	float temperature = WIDTH / 10.0f;
	//optimal edge/link length 
	float k = sqrt(area / nodeCount);
	k = 10.0;
	float kSquare = area / nodeCount;
	kSquare = 100.0;
	//i am not checking epsilon now
	//but will put once i know that is the value range
	//it's not good to only check the iteration count

	int iterationCount = 0;
	while (iterationCount < max_iteration_count || temperature > 0.00001f)
	{
		temperature *= (1.0 - ((iterationCount * 1.0) / max_iteration_count));
		iterationCount++;
		//repulsive force effect


		#pragma omp parallel for num_threads(n_count) collapse(2)
		for (int i = 0; i < n_count; i++) {
			for (int j = 0; j < n_count; j++) {
				nodeList[i].displacement = { 0.0f, 0.0f };
				if (i != j) {
					float dx = nodeList[j].position.x - nodeList[i].position.x;
					float dy = nodeList[j].position.y - nodeList[i].position.y;
						float d = dx * dx + dy * dy;
						if (d > 0.01) {
						float fr = (kSquare / sqrt(d));
						float cofficientx = dx / sqrt(d);
						float cofficienty = dy / sqrt(d);
						nodeList[j].displacement.x += fr * cofficientx;
						nodeList[j].displacement.y += fr * cofficienty;
					}
				}
			}
		}

		//calculate the attractive force
		#pragma omp parallel for num_threads(e_count)
		for (int i = 0; i < e_count; i++) {
			float dx = edgeList[i].node1.position.x - edgeList[i].node2.position.x;
			float dy = edgeList[i].node1.position.y - edgeList[i].node2.position.y;
				float dSquare = dx * dx + dy * dy;
				if (dSquare>0.01) {
					float d = sqrt(dSquare);
					float fa = (dSquare / k);
					edgeList[i].node1.displacement.x -= (dx / d) * fa;
					edgeList[i].node1.displacement.y -= (dy / d) * fa;
					edgeList[i].node2.displacement.x += (dx / d) * fa;
					edgeList[i].node2.displacement.y += (dy / d) * fa;
			}
		}




		//limit displacement to the temperature
		#pragma omp parallel for num_threads(n_count)
		for (int i = 0; i < n_count; i++) {
			float d = sqrt(nodeList[i].displacement.x * nodeList[i].displacement.x + nodeList[i].displacement.y * nodeList[i].displacement.y);
			if (d > 0.01) {
				nodeList[i].position.x += ((nodeList[i].displacement.x) / d) * min(d, temperature);
				nodeList[i].position.y += (nodeList[i].displacement.y / d) * min(d, temperature);
			}

		}

	}

}

int main(int argc, char** argv) {

	GraphLayout graph;
	graph.addNode(20);
	graph.addEdge(0, 1, 1.0);
	graph.addEdge(0, 2, 1.0);
	graph.addEdge(1, 3, 1.0);
	graph.addEdge(2, 3, 1.0);
	graph.addEdge(3, 4, 1.0);
	graph.addEdge(1, 4, 1.0);
	graph.addEdge(10, 1, 1.0);
	graph.addEdge(10, 2, 1.0);
	graph.addEdge(11, 3, 1.0);
	graph.addEdge(12, 3, 1.0);
	graph.addEdge(13, 4, 1.0);
	graph.addEdge(11, 4, 1.0);
	graph.addEdge(1, 14, 1.0);
	graph.addEdge(10, 11, 1.0);
	graph.addEdge(10, 12, 1.0);
	graph.addEdge(12, 13, 1.0);
	graph.addEdge(13, 14, 1.0);
	graph.addEdge(11, 14, 1.0);

	graph.start(500);
	for (auto iterator = graph.nodeList.begin(); iterator != graph.nodeList.end(); iterator++) {
		printf(" node %d  coordinate is ( %f, %f)\n", iterator->index, iterator->position.x, iterator->position.y);
	}
}