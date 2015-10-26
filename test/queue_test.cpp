#include <iostream>
#include "trigger_queue.hpp"

using namespace std;

int main(int argc, char** argv) {
	trigger_queue<int> trig_q;
	trig_q.push(23);
	cout << 23 << " " << trig_q.distance() << endl;
	trig_q.push(12);
	cout << 12 << " " << trig_q.distance() << endl;
	trig_q.push(36);
	cout << 36 << " " << trig_q.distance() << endl;
	trig_q.push(84);
	cout << 84 << " " << trig_q.distance() << endl;
	trig_q.push(10);
	cout << 10 << " " << trig_q.distance() << endl;
	trig_q.push(4);
	cout << 4 << " " << trig_q.distance() << endl;
	cout << "-----------" << endl;
	while (!trig_q.empty()) {
		cout << trig_q.top() << endl;
		trig_q.pop();
	}
	cout << endl;

	return 0;
}
