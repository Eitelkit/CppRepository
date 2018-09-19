/////////////////////////////////////////////////////////////////////
// ProducerConsumer.cpp 								                           //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: A demo for the Producer & consumer problem         //
// Author:      Xinyu Zhang, Syracuse University, 2016 Fall MS     //
//																																 //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <ctime>
#include <random>
#include <stdlib.h>
using namespace std;

mutex mtx;
vector<int> buffer(4);
vector<int> buffer_capacity = { 6, 5, 4, 3 }; //buffer capacity
vector<pair<condition_variable, condition_variable> > cv(4);



void PartWorker(int id) {
	int run = 1;

	while (run <= 5) {
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		srand(time(NULL) + id);
		unique_lock<mutex> lck(mtx);
		vector<bool> place_can_update(4); //where the buffer is not full & placeRequest still have parts then it can be updated
		vector<int> placeRequest(4);
		for (int i = 0; i < 3; i++) {
			placeRequest[rand() % 4]++;
		}
		for (int i = 0; i < 4; i++) {
			if (placeRequest[i] != 0) place_can_update[i] = true;
		}

		while (!(placeRequest[0] == 0 && placeRequest[1] == 0 && placeRequest[2] == 0 && placeRequest[3] == 0 )) {
			cout << "PartWorker ID:" << id << endl;
			cout << "Iteration: " << run << endl;
			cout << "Buffer State: (" << buffer[0] << "," << buffer[1]
				<< "," << buffer[2] << "," << buffer[3] << ")" << endl;
			cout << "Place Request: (" << placeRequest[0] << "," << placeRequest[1] << ","
				<< placeRequest[2] << "," << placeRequest[3] << ")" << endl;
			for (int i = 0; i < 4; i++) {
				if (!place_can_update[i]) continue;
				if (buffer[i] == buffer_capacity[i]) continue;
				if ((buffer[i] + placeRequest[i]) < buffer_capacity[i]) {
					buffer[i] += placeRequest[i];
					placeRequest[i] = 0;
					place_can_update[i] = false; //added
				} else {
					placeRequest[i] -= (buffer_capacity[i] - buffer[i]);
					buffer[i] = buffer_capacity[i];
				}
				//notify some product worker who needs the i part
				cv[i].second.notify_one();
			}
			cout << "Updated Buffer State: (" << buffer[0] << ","
				<< buffer[1] << "," << buffer[2] << "," << buffer[3] << ")" << endl;
			cout << "Updated Place Request: (" << placeRequest[0] << ","
				<< placeRequest[1] << "," << placeRequest[2] << "," << placeRequest[3] << ")" << endl;
			cout << endl;

			for (int i = 0; i < 4; i++) {
				if (placeRequest[i] != 0) {
					//cv[i].first.wait(lck);
					//there are still some parts left in the part worker's hand while the buffer[i] is full.
					//wait for 1 seconds if it is not waken, it is possible that the thread encountered deadlock.
					while (cv[i].first.wait_for(lck, std::chrono::seconds(1)) == std::cv_status::timeout) {
						cout << "PartWorker ID:" << id << endl;
						cout << "Iteration: " << run << endl;
						cout << "Buffer State: (" << buffer[0] << "," << buffer[1]
							<< "," << buffer[2] << "," << buffer[3] << ")" << endl;
						cout << "Place Request: (" << placeRequest[0] << "," << placeRequest[1] << ","
							<< placeRequest[2] << "," << placeRequest[3] << ")" << endl;
						cout << "Updated Buffer State: (" << buffer[0] << ","
							<< buffer[1] << "," << buffer[2] << "," << buffer[3] << ")" << endl;
						cout << "Updated Place Request: (" << placeRequest[0] << ","
							<< placeRequest[1] << "," << placeRequest[2] << "," << placeRequest[3] << ")" << endl;
						std::cout << "Return" << std::endl;
						cout << endl;
						return;
					}

					for (int i = 0; i < 4; i++) {
						//there is place in buffer & placeRequest isn't 0, then update
						if (placeRequest[i] != 0 && buffer[i] != buffer_capacity[i]) place_can_update[i] = true;
						else place_can_update[i] = false;
					}
					if (place_can_update[0] == false && place_can_update[1] == false && place_can_update[2] == false && place_can_update[3] == false) { i--; continue; }
					break;
				}
			}
		}
		run++;
	}
}

void ProductWorker(int id) {
	int run = 1;

	while (run <= 5) {

		srand(time(NULL) + id);
		unique_lock<mutex> lck(mtx);
		vector<bool> pickup_can_update(4);
		vector<int> pickUpRequest(4);
		// select two parts that the product worker does not needed.
		int part1 = rand() % 4;
		int part2 = rand() % 4;
		while (part1 == part2) {
			part1 = rand() % 4;
		}

		for (int i = 0; i < 4; i++) {
			int part = rand() % 4;
			if (part == part1 || part == part2) {
				i--;
				continue;
			}
			while (pickUpRequest[part] == 3) {
				part = rand() % 4;
			}
			pickUpRequest[part]++;
		}


		for (int i = 0; i < 4; i++) {
			if (pickUpRequest[i] != 0) pickup_can_update[i] = true;
		}
		while (!(pickUpRequest[0] == 0 && pickUpRequest[1] == 0 && pickUpRequest[2] == 0 && pickUpRequest[3] == 0)) {
			cout << "ProductWorker ID:" << id << endl;
			cout << "Iteration: " << run << endl;
			cout << "Buffer State: (" << buffer[0] << "," << buffer[1] << ","
				<< buffer[2] << "," << buffer[3] << ")" << endl;
			cout << "Pickup Request: (" << pickUpRequest[0] << ","
				<< pickUpRequest[1] << "," << pickUpRequest[2] << "," << pickUpRequest[3] << ")" << endl;
			for (int i = 0; i < 4; i++) {
				if (!pickup_can_update[i]) continue;
				if (buffer[i] == 0) continue;
				if (buffer[i] >= pickUpRequest[i]) {
					buffer[i] -= pickUpRequest[i];
					pickUpRequest[i] = 0;
				}
				else {
					pickUpRequest[i] -= buffer[i];
					buffer[i] = 0;
				}
				cv[i].first.notify_one();
			}

			cout << "Updated Buffer State: (" << buffer[0] << "," << buffer[1]
				<< "," << buffer[2] << "," << buffer[3] << ")" << endl;
			cout << "Updated Pickup Request: (" << pickUpRequest[0] << ","
				<< pickUpRequest[1] << "," << pickUpRequest[2] << "," << pickUpRequest[3] << ")" << endl;
			cout << endl;
			for (int i = 0; i < 4; i++) {
				if (pickUpRequest[i] != 0) {
					//cv[i].second.wait(lck);
					//wait for 1 seconds if it is not waken, it is possible that the thread encountered deadlock.
					while (cv[i].second.wait_for(lck, std::chrono::seconds(1)) == std::cv_status::timeout) {
						cout << "ProductWorker ID:" << id << endl;
						cout << "Iteration: " << run << endl;
						cout << "Buffer State: (" << buffer[0] << "," << buffer[1] << ","
							<< buffer[2] << "," << buffer[3] << ")" << endl;
						cout << "Pickup Request: (" << pickUpRequest[0] << ","
							<< pickUpRequest[1] << "," << pickUpRequest[2] << "," << pickUpRequest[3] << ")" << endl;
						cout << "Updated Buffer State: (" << buffer[0] << "," << buffer[1]
							<< "," << buffer[2] << "," << buffer[3] << ")" << endl;
						cout << "Updated Pickup Request: (" << pickUpRequest[0] << ","
							<< pickUpRequest[1] << "," << pickUpRequest[2] << "," << pickUpRequest[3] << ")" << endl;
						std::cout << "Return" << std::endl;
						cout << endl;
						return;
					}


					for (int i = 0; i < 4; i++) {
						if (pickUpRequest[i] != 0 && buffer[i] != 0) pickup_can_update[i] = true;
						else pickup_can_update[i] = false;
					}
					if (pickup_can_update[0] == false && pickup_can_update[1] == false
						&& pickup_can_update[2] == false && pickup_can_update[3] == false) { i--; continue; }
					break;
				}
			}
		}
		run++;
	}
}


int main() {

	const int m = 16, n = 12; //m: number of Part Workers
							  //n: number of Product Workers
	thread partW[m];
	thread prodW[n];
	for (int i = 0; i < n; i++) {
		partW[i] = thread(PartWorker, i);
		prodW[i] = thread(ProductWorker, i);
	}

	for (int i = n; i<m; i++) {
		partW[i] = thread(PartWorker, i);
	}
	/* Join the threads to the main threads */
	for (int i = 0; i < n; i++) {
		partW[i].join();
		prodW[i].join();
	}
	for (int i = n; i<m; i++) {
		partW[i].join();
	}
	cout << "Finish!" << endl;

	getchar();
	getchar();

	return 0;

}
